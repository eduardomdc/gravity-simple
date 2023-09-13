#include "main.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <cstdlib>
#include <random>

#define FPS 120
#define MIN_DELTA 1000/FPS

Gravity* gravity;

Uint32 lastUpdateTime;
Uint32 updateTime;

int main(){
    srand(time(NULL));
	gravity = new Gravity(1500);
    gravity->initSDL();
	while(!gravity->exit){
        lastUpdateTime = updateTime;
        gravity->getInput();
        gravity->render();
        updateTime = SDL_GetTicks();
        Uint32 delta = updateTime - lastUpdateTime;
        gravity->update(0.3);
        if (delta < MIN_DELTA) {
            SDL_Delay(MIN_DELTA-delta);
        }
	}
    delete gravity;
	return 0;
}
