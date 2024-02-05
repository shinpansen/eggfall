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

class sound
{
public:
    sound();
    Sound* PlaySound(int type, bool loop, int volume = NULL);
    bool IsMusicPaused();
    void PlayRandomMusic(int num = -1);
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
