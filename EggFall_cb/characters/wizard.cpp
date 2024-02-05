#include "wizard.hpp"

static const float SCALE = 30.f;
static const float WALKING_SPEED = 10.f;
static const float RUNNING_SPEED = 15.f;
static const float FALLING_SPEED = 20.f;
static const float DASHING_SPEED = 25.f;
static const float SPELL_SPEED = 17.5f;

using namespace std;
using namespace sf;

bool wizard::PerformAttack()
{
	if (_stuck)
		return false;

	//Attacking
	if (_input->isKeyPressed(CMD_ATTACK) && _counteract == 0.f && _attacking == 0.f && _dash == 0.f &&
		_attackingCooldown == 0.f && !_hasAttacked && _hooked == 0 && _canWallJump == 0.f)
	{
		//Calculating correct body position if looping
		Vector2f position;
		if (_position.x - 13 <= 0 && _animation.GetReversed())
			position = Vector2f(utils::VIEW_WIDTH + _position.x, _position.y);
		else if (_position.x + 13 >= utils::VIEW_WIDTH && !_animation.GetReversed())
			position = Vector2f(_position.x - utils::VIEW_WIDTH, _position.y);
		else if (_position.y - 30 <= 0 && _input->isKeyPressed(CMD_UP))
			position = Vector2f(_position.x, _position.y + utils::VIEW_HEIGHT);
		else if (_position.y + 30 >= utils::VIEW_HEIGHT && _input->isKeyPressed(CMD_DOWN))
			position = Vector2f(_position.x, _position.y - utils::VIEW_HEIGHT);
		else
			position = _position;

		//Useful variables to cast spell later
		Vector2f analogDir = _input->GetStickDirection();
		float signe = _animation.GetReversed() ? -1.f : 1.f;
		_attacking = signe*36.f;
		_attackingCooldown = (float)signe*40.f;
		_isAiming = false;
		_canClimb = 0.f;
		_crushAttackAnimation = false;
		_spellAngle = _input->GetStickAngle(signe > 0.f ? 90 : 270) - 90.f;
		_spellSpeed = analogDir != Vector2f(0, 0) ? Vector2f(analogDir.x*SPELL_SPEED, analogDir.y*SPELL_SPEED) : Vector2f(signe*SPELL_SPEED, 0.f);
		_spellPos = analogDir != Vector2f(0, 0) ? Vector2f(analogDir.x * 40.f, analogDir.y * 40.f) : Vector2f(signe * 40.f, 0.f);

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

		//Adding spell + effects
		Vector2f spellPos = Vector2f(_position.x + _spellPos.x, _position.y + _spellPos.y);
		_lastProjectile = _projectileBox->AddSpell(spellPos, 0.3, 35, 25, 6, _colorID, _spellAngle, _spellSpeed, GetHeroColor(_colorID));
		_lastProjectile->SetUserData(_body); //The player who threw the projectile will be identified with that userdata
		_soundBox->PlaySound(static_cast<SOUND>(_colorID + 1), false);
		_spellEffect = _effectBox->AddAnimation(T_SPELLS, _colorID * 2 + 1, 0.3, false, drawOption(Vector2f(75, 75)), fadeOption(0), physics(false));
		_spellEffect->setRotation(_spellAngle);
		_spellEffect->setPosition(spellPos);
		
		//Simulating wind blowing behind wizard
		_effectBox->AddCircleShape(drawOption(Vector2f(spellPos.x - _spellSpeed.x, spellPos.y), Vector2f(15, 15), Color::Transparent),
			fadeOption(0.f, 12.f), physics(true, _world, Vector2f(-_spellSpeed.x*35.f, 0.f), 0.5f, 0.05f, 0.0f));
	}
	if (_body != NULL && _hasAttacked && fabs(_attacking) >= 21.f && fabs(_attacking) <= 24.f)
	{
		//Wizard recoil while casting spell
		if (fabs(_spellSpeed.x) > 7.f)
			_body->SetLinearVelocity(b2Vec2(-_spellSpeed.x / fabs(_spellSpeed.x) * 5.f, _speed.y));
		else if (_spellSpeed.y > 0.f)
			_body->SetLinearVelocity(b2Vec2(_speed.x, -8.f));
		return true;
	}
	if (_attacking == 0.f)
	{
		_hasAttacked = false;
		_crushAttackAnimation = true;
	}
	return false;
}

void wizard::LoadHeroTexture()
{
	//Sprite
	if (_colorID == 0 && (!_texture[0].loadFromFile(files::wizardBlueTile_1) ||
		!_texture[1].loadFromFile(files::wizardBlueTile_2) ||
		!_texture[2].loadFromFile(files::wizardBlueTile_3)))
	{
		cout << "Unable to find blue wizard texture(s). Stopping execution." << endl;
		exit(1);
	}
	else if (_colorID == 1 && !_texture[0].loadFromFile(files::wizardRedTile))
	{
		cout << "Unable to find red wizard texture. Stopping execution." << endl;
		exit(1);
	}
	else if (_colorID == 2 && !_texture[0].loadFromFile(files::wizardGreenTile))
	{
		cout << "Unable to find green wizard texture. Stopping execution." << endl;
		exit(1);
	}
	else if (_colorID == 3 && !_texture[0].loadFromFile(files::wizardPurpleTile))
	{
		cout << "Unable to find purple wizard texture. Stopping execution." << endl;
		exit(1);
	}
	else if (_colorID == 4 && !_texture[0].loadFromFile(files::wizardWhiteTile))
	{
		cout << "Unable to find white wizard texture. Stopping execution." << endl;
		exit(1);
	}
	else if (_colorID >= 0 && _colorID <= 4)
	{
		_sprite.setTexture(_texture[0]);
		_sprite.setOrigin(40, 84);

		//Flash sprite
		_textureFlash[0].loadFromImage(utils::ColorizeImage(files::wizardBlueTile_1, Color::White));
		_textureFlash[1].loadFromImage(utils::ColorizeImage(files::wizardBlueTile_2, Color::White));
		_textureFlash[2].loadFromImage(utils::ColorizeImage(files::wizardBlueTile_3, Color::White));

		//_texture[0].loadFromImage(utils::ColorizeUsingScheme(files::wizardBlueTile_1, CS_WIZARD_PURPLE));

		_spellEffect = NULL;
		if (_colorID == 0)
		{
			//Base sprite
			vector<Texture*> textures;
			for (int i = 0; i<3; i++)
				textures.push_back(&_texture[i]);
			_animation.Reset(&_sprite, 0.1, 84, 84, 6, 36, 0, false, textures);
		}
		else
			_animation.Reset(&_sprite, 0.1, 84, 84, 6, 36, 0);

		//Reverse sprite with initial position
		if (_body->GetPosition().x*SCALE > utils::VIEW_WIDTH)
			_animation.ReverseSprite(true);
	}
	else
	{
		cout << "Not a correct wizard color ID" << endl;
		exit(1);
	}
}

void wizard::AnimAttack()
{
	//Animation speed
	if (_attacking != 0.f)
	{
		if (fabs(_spellSpeed.x) > 7.f || _speed.y > 0.f)
			_animation.SetFrameDelay(fabs(_attacking) >= 20.f ? 0.38f : 0.3f);
		else
			_animation.SetFrameDelay(fabs(_attacking) >= 23.f ? 0.42f : 0.25f);
	}

	//3 animations sides / top / down
	if (fabs(_spellSpeed.x) >= SPELL_SPEED/2.f)
		_animation.SetAnimation(fabs(_attacking) >= 20.f ? 11 : 12, 5);
	else if (_spellSpeed.y < 0.f)
	{
		//Crush on the floor animation
		if (_onTheFloor && !_crushAttackAnimation && fabs(_attacking) < 15.f)
		{
			_crushAttackAnimation = true;
			_animation.PerformFallingScale(0.5f);
			DrawEffect(DUST_LANDING);
		}
		if (_animation.GetAnimation() == 15 && _onTheFloor && _animation.GetFrame() < 3.f)
		{
			_attacking = fabs(_attacking) / _attacking * 20.f;
			_animation.SetAnimation(14, 5);
		}
		else
			_animation.SetAnimation(fabs(_attacking) >= 23.f ? 13 : _onTheFloor && _animation.GetAnimation() != 15 ? 14 : 15, 5);
	}
	else
		_animation.SetAnimation(fabs(_attacking) >= 20.f ? 16 : 17, 5);
	_animation.ReverseSprite(_attacking < 0.f);
}
