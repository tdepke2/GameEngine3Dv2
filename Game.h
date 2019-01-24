#ifndef _GAME_H
#define _GAME_H

#include <random>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class Game {
    public:
    static const float FPS_CAP;    // Maximum allowed frames per second.
    static const float PI;
    
    static int start();
    static int randomInteger(int min, int max);
    static int randomInteger(int n);
    
    private:
    enum class State {
        uninitialized, menu, playing, paused, exiting
    };
    
    static State state;
    static mt19937 mainRNG;
    static Vector3f camPosition, camRotation;
};

#endif