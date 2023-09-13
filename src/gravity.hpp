#ifndef GRAVITY
#define GRAVITY

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <vector>

class Gravity {
public:
    Gravity(int particles);
    void initSDL();
    void randomizeTable();
    void render();
    void update(float delta);
    void closeSDL();
    void getInput();
    void randomizeParticles();
    int particleAmount;
    SDL_FPoint* particlePoints;
    SDL_FPoint* particleVel;
    bool exit;
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event currentEvent;
};

#endif
