#ifndef sound_HPP
#define sound_HPP

#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <SFML/Audio.hpp>
#include "../tools/settings.hpp"

using namespace std;
using namespace sf;

enum SOUND
{
	SND_JUMP = 0,
	SND_SPELL_A = 1,
	SND_SPELL_B = 2,
	SND_SPELL_C = 3,
	SND_SPELL_D = 4,
	SND_SPELL_E = 5,
	SND_EXPLOSION_A = 6,
	SND_COUNTERACT = 7,
	SND_FALLING = 8,
	SND_COUNTERING = 9,
	SND_WALLSLIDING = 10,
	SND_DYING = 11,
	SND_TICK = 12
};

enum MUSIC
{
	Random = -1,
	Chibi_Ninja = 0,
	A_Night_Of_Dizzy_Spells = 1,
	HHavok = 2,
	Searching = 3,
	All_of_Us = 4
};

class sound
{
public:
    sound();
    Sound* PlaySound(SOUND snd, bool loop, int volume = NULL);
    bool IsMusicPaused();
    void PlayMusic(MUSIC msc = MUSIC::Random);
    void PauseMusic(bool pause);
    void PerformSounds();
    void UpdateVolumes(settings* settings);
private:
    //Variables
    int _volumeMusic;
    int _volumeEffects;

    //Music - Sounds
    Music _music;
    SoundBuffer _jumpBuffer;
    SoundBuffer _fallingBuffer;
    SoundBuffer _spell1Buffer;
    SoundBuffer _spell2Buffer;
    SoundBuffer _spell3Buffer;
    SoundBuffer _spell4Buffer;
    SoundBuffer _spell5Buffer;
    SoundBuffer _magicExplosionBuffer;
    SoundBuffer _rollingBuffer;
    SoundBuffer _counteringBuffer;
    SoundBuffer _wallSlidingBuffer;
    SoundBuffer _dyingBuffer;
    SoundBuffer _clockTickingBuffer;
    vector<Sound*> _sounds;
};
#endif
