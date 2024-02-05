#include "wizard.hpp"

static const float SCALE = 30.f;
static const float WALKING_SPEED = 10.f;
static const float RUNNING_SPEED = 15.f;
static const float FALLING_SPEED = 20.f;
static const float DASHING_SPEED = 25.f;
static const float SPELL_SPEED = 17.5f;

bool wizard::PerformAttack()
{
	if (_stuck || _stamina == 0.f)
		return false;

	//Attacking
	if (_input->isKeyPressed(CMD_ATTACK) && _counteract == 0.f && _attacking == 0.f && _dash == 0.f &&
		_attackingCooldown == 0.f && !_hasAttacked && _hooked == 0 && _canWallJump == 0.f)
	{
		//Calculating correct body position if looping
		Vector2f position;
		if (_position.x - 13 <= 0 && _animation->GetReversed())
			position = Vector2f(utils::VIEW_WIDTH + _position.x, _position.y);
		else if (_position.x + 13 >= utils::VIEW_WIDTH && !_animation->GetReversed())
			position = Vector2f(_position.x - utils::VIEW_WIDTH, _position.y);
		else if (_position.y - 30 <= 0 && _input->isKeyPressed(CMD_UP))
			position = Vector2f(_position.x, _position.y + utils::VIEW_HEIGHT);
		else if (_position.y + 30 >= utils::VIEW_HEIGHT && _input->isKeyPressed(CMD_DOWN))
			position = Vector2f(_position.x, _position.y - utils::VIEW_HEIGHT);
		else
			position = _position;

		//Useful variables to cast spell later
		Vector2f analogDir = _input->GetStickDirection();
		float signe = _animation->GetReversed() ? -1.f : 1.f;
		_attacking = signe*36.f;
		_attackingCooldown = (float)signe*40.f;
		_isAiming = false;
		_canClimb = 0.f;
		_crushAttackAnimation = false;
		_spellAngle = _input->GetStickAngle(signe > 0.f ? 90 : 270) - 90.f;
		_projectileSpeed = analogDir != Vector2f(0, 0) ? Vector2f(analogDir.x*SPELL_SPEED, analogDir.y*SPELL_SPEED) : Vector2f(signe*SPELL_SPEED, 0.f);
		_projectilePos = analogDir != Vector2f(0, 0) ? Vector2f(analogDir.x * 40.f, analogDir.y * 40.f) : Vector2f(signe * 40.f, 0.f);

		//Changing body size if was crouching
		if (_crouch)
		{
			ResetBody(20, 35);
			_body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y - 7.5f / SCALE), 0);
			_crouch = false;
		}

		//Cancelling falling speed
		if (_body != NULL && _speed.y >= 0)
		{
			_body->SetLinearVelocity(b2Vec2(_speed.x, 0));
			_speed.y = 0;
		}
	}
	else if (!_hasAttacked && fabs(_attacking) > 0.f && fabs(_attacking) <= 24.f) //Spell cast
	{
		//Changing state variables
		_selfProjectileInvincible = 8.f;
		_hasAttacked = true;
		_stamina = utils::StepCooldown(_stamina, 50.f, _deltaTime);
		_staminaCooldown = _stamina > 0.f ? 42.f : 100.f;
		_position = Vector2f(_body->GetPosition().x * SCALE, _body->GetPosition().y * SCALE);

		//Adding spell + effects
		Vector2f spellPos = Vector2f(_position.x + _projectilePos.x, _position.y + _projectilePos.y);
		_lastProjectile = _projectileBox->AddSpell(spellPos, 0.3, 35, 25, 6, _colorID, _spellAngle, _projectileSpeed, GetHeroColor(_colorID), _body);
		_soundBox->PlaySound(static_cast<SOUND>(_colorID + 1), false);
		Sprite * sfxSprite = _effectBox->AddAnimation(T_SPELLS_EXPLOSION, _colorID * 2 + 1, 0.3, false, drawOption(Vector2f(75, 75)), fadeOption(0), physics(false));
		sfxSprite->setRotation(_spellAngle);
		sfxSprite->setPosition(spellPos);
		
		//Simulating wind blowing behind wizard
		_effectBox->AddCircleShape(drawOption(Vector2f(spellPos.x - _projectileSpeed.x, spellPos.y), Vector2f(15, 15), Color::Transparent),
			fadeOption(0.f, 12.f), physics(true, _world, Vector2f(-_projectileSpeed.x*35.f, 0.f), 0.5f, 0.05f, 0.0f));
	}
	if (_body != NULL && _hasAttacked && fabs(_attacking) >= 21.f && fabs(_attacking) <= 24.f)
	{
		//Wizard recoil while casting spell
		if (fabs(_projectileSpeed.x) > 7.f)
			_body->SetLinearVelocity(b2Vec2(-_projectileSpeed.x / fabs(_projectileSpeed.x) * 5.f, _speed.y));
		else if (_projectileSpeed.y > 0.f)
			_body->SetLinearVelocity(b2Vec2(_speed.x, -8.f));
		return true;
	}
	if (_attacking == 0.f && !_input->isKeyPressed(CMD_ATTACK))
	{
		_hasAttacked = false;
		_crushAttackAnimation = true;
	}
	return false;
}

void wizard::AnimAttack()
{
	//Animation speed
	if (_attacking != 0.f)
	{
		if (fabs(_projectileSpeed.x) > 7.f || _speed.y > 0.f)
			_animation->SetFrameDelay(fabs(_attacking) >= 20.f ? 0.38f : 0.3f);
		else
			_animation->SetFrameDelay(fabs(_attacking) >= 23.f ? 0.42f : 0.25f);
	}

	//3 animations sides / top / down
	if (fabs(_projectileSpeed.x) >= SPELL_SPEED/2.f)
		_animation->SetAnimation(fabs(_attacking) >= 20.f ? 11 : 12, 5);
	else if (_projectileSpeed.y < 0.f)
	{
		//Crush on the floor animation
		if (_onTheFloor && !_crushAttackAnimation && fabs(_attacking) < 15.f)
		{
			_crushAttackAnimation = true;
			_animation->PerformFallingScale(0.5f);
			DrawEffect(DUST_LANDING);
		}
		if (_animation->GetAnimation() == 15 && _onTheFloor && _animation->GetFrame() < 3.f)
		{
			_attacking = fabs(_attacking) / _attacking * 20.f;
			_animation->SetAnimation(14, 5);
		}
		else
			_animation->SetAnimation(fabs(_attacking) >= 23.f ? 13 : _onTheFloor && _animation->GetAnimation() != 15 ? 14 : 15, 5);
	}
	else
		_animation->SetAnimation(fabs(_attacking) >= 20.f ? 16 : 17, 5);
	_animation->ReverseSprite(_projectileSpeed.x < 0.f);
}

void wizard::AnimateChild()
{
	//Shield effect
	if (_shieldEffect != NULL)
		_shieldEffect->setPosition(Vector2f(_position.x + _lastCounteractSigne * 15.f, _position.y - 8.f));
}

void wizard::PerformCounteractChildEffect()
{
	//Counteract effect
	_effectBox->AddRectangleShape(drawOption(_position, Vector2f(20, 20), Vector2f(10, 10), Color::Transparent, 0.f,
		255, Color(255, 0, 0), 1), fadeOption(Vector2f(0, 0), 15.f, 1.f, 100.f, 0.f, Vector2f(1.0001f, 1.0001f)), physics(false));
	_effectBox->AddRectangleShape(drawOption(_position, Vector2f(20, 20), Vector2f(10, 10), Color::Transparent, 45.f,
		255, Color(200, 0, 0), 1), fadeOption(Vector2f(0, 0), 15.f, 1.f, 100.f, 0.f, Vector2f(1.0001f, 1.0001f)), physics(false));
}
