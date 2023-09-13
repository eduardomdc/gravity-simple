#ifndef GRAVITY
#define GRAVITY

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <vector>

class Gravity {
public:
    Gravity(int particlesAmount, int darkParticles);
    ~Gravity();
    void newSim();
    void initSDL();
    void randomizeTable();
    void render();
    void update(float delta);
    void closeSDL();
    void getInput();
    void randomizeParticles();
    int particleAmount;
    int darkParticles;
    SDL_FPoint* pPoints;
    SDL_FPoint* pVel;
    SDL_FPoint* dark;
    SDL_FPoint* darkVel;
    bool exit;
    bool paused;
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event currentEvent;
};

#endif
