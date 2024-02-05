#include "sound.hpp"

using namespace std;
using namespace sf;

sound::sound()
{
    //Volumes
    _volumeMusic = 10;
    _volumeEffects = 10;

    _music.openFromFile("resources//musics//Chibi_Ninja.ogg");
    _music.pause();

    //Sounds resources
    if(!_jumpBuffer.loadFromFile("resources//sounds//jumping.ogg") ||
       !_fallingBuffer.loadFromFile("resources//sounds//falling.ogg") ||
       !_spell1Buffer.loadFromFile("resources//sounds//spell1.ogg") ||
       !_spell2Buffer.loadFromFile("resources//sounds//spell2.ogg") ||
       !_spell3Buffer.loadFromFile("resources//sounds//spell3.ogg") ||
       !_spell4Buffer.loadFromFile("resources//sounds//spell4.ogg") ||
       !_spell5Buffer.loadFromFile("resources//sounds//spell5.ogg") ||
       !_magicExplosionBuffer.loadFromFile("resources//sounds//magicExplosion.ogg") ||
       !_rollingBuffer.loadFromFile("resources//sounds//rolling.ogg") ||
       !_counteringBuffer.loadFromFile("resources//sounds//countering.ogg") ||
       !_dyingBuffer.loadFromFile("resources//sounds//dead.ogg") ||
       !_wallSlidingBuffer.loadFromFile("resources//sounds//wallSliding.ogg") ||
       !_clockTickingBuffer.loadFromFile("resources//sounds//clockTicking.ogg"))
       {
           cout << "Some sounds files are missing" << endl;
           exit(1);
       }
}

Sound* sound::PlaySound(SOUND snd, bool loop, int volume)
{
    volume = volume*((float)_volumeEffects/10.0);
    _sounds.push_back(new Sound());
    switch(snd)
    {
		case SOUND::SND_JUMP:
            _sounds[_sounds.size()-1]->setBuffer(_jumpBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 100*((float)_volumeEffects/10.0));
            break;
		case SOUND::SND_SPELL_A:
            _sounds[_sounds.size()-1]->setBuffer(_spell1Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 40*((float)_volumeEffects/10.0));
            break;
        case SOUND::SND_SPELL_B:
            _sounds[_sounds.size()-1]->setBuffer(_spell2Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 40*((float)_volumeEffects/10.0));
            break;
        case SOUND::SND_SPELL_C:
            _sounds[_sounds.size()-1]->setBuffer(_spell3Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 40*((float)_volumeEffects/10.0));
            break;
        case SOUND::SND_SPELL_D:
            _sounds[_sounds.size()-1]->setBuffer(_spell4Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 40*((float)_volumeEffects/10.0));
            break;
        case SOUND::SND_SPELL_E:
            _sounds[_sounds.size()-1]->setBuffer(_spell5Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 40*((float)_volumeEffects/10.0));
            break;
		case SOUND::SND_EXPLOSION_A:
            _sounds[_sounds.size()-1]->setBuffer(_magicExplosionBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 70*((float)_volumeEffects/10.0));
            break;
		case SOUND::SND_ROLLING:
            _sounds[_sounds.size()-1]->setBuffer(_rollingBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 100*((float)_volumeEffects/10.0));
            break;
		case SOUND::SND_FALLING:
            _sounds[_sounds.size()-1]->setBuffer(_fallingBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 70*((float)_volumeEffects/10.0));
            break;
		case SOUND::SND_COUNTERING:
            _sounds[_sounds.size()-1]->setBuffer(_counteringBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 50*((float)_volumeEffects/10.0));
            break;
		case SOUND::SND_WALLSLIDING:
            _sounds[_sounds.size()-1]->setBuffer(_wallSlidingBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 50*((float)_volumeEffects/10.0));
            break;
		case SOUND::SND_DYING:
            _sounds[_sounds.size()-1]->setBuffer(_dyingBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 50*((float)_volumeEffects/10.0));
            break;
		case SOUND::SND_TICK:
            _sounds[_sounds.size()-1]->setBuffer(_clockTickingBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 36*((float)_volumeEffects/10.0));
            break;
        default:
            _sounds[_sounds.size()-1]->setBuffer(_dyingBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != NULL ? volume : 100*((float)_volumeEffects/10.0));
            break;
    }
    //_sounds[_sounds.size()-1]->setVolume(0);
    _sounds[_sounds.size()-1]->setLoop(loop);
    _sounds[_sounds.size()-1]->play();

    return _sounds[_sounds.size()-1];
}

bool sound::IsMusicPaused()
{
    return _music.getStatus() == SoundSource::Paused;
}

void sound::PlayMusic(MUSIC msc)
{
    int random = msc < 0 ? rand()%4 : msc;
    switch(random)
    {
        case 0:
            _music.openFromFile("resources//musics//Chibi_Ninja.ogg");
            break;
        case 1:
            _music.openFromFile("resources//musics//A_Night_Of_Dizzy_Spells.ogg");
            break;
        case 2:
            _music.openFromFile("resources//musics//HHavok.ogg");
            break;
        case 3:
            _music.openFromFile("resources//musics//Searching.ogg");
            break;
        case 4:
            _music.openFromFile("resources//musics//All_of_Us.ogg");
            break;
    }
    //_music.setVolume(0);
    _music.setVolume(25*((float)_volumeMusic/10.0));
    _music.setLoop(false);
    _music.play();
}

void sound::PauseMusic(bool pause)
{
    if(pause)
        _music.pause();
    else
        _music.play();
}

void sound::PerformSounds()
{
    if(_music.getStatus() != SoundSource::Paused && _music.getStatus() == SoundSource::Stopped)
        PlayMusic();

    for(int i=0 ; i<_sounds.size() ; i++)
    {
        if(_sounds[i]->getStatus() == SoundSource::Stopped)
        {
            delete _sounds[i];
            _sounds.erase(_sounds.begin()+i);
        }
    }
}

void sound::UpdateVolumes(settings* settings)
{
    _volumeMusic = settings->_music;
    _volumeEffects = settings->_effects;
    _music.setVolume(25*((float)_volumeMusic/10.0));
}
