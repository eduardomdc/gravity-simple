#include "gravity.hpp"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <random>

#define G_CONST 0.1
#define RES 500
#define sign(x) ((x<0)-(x>0))
#define min(a,b) ((a)<(b)?(a):(b))


Gravity::Gravity(int particleAmount){
    this->particleAmount = particleAmount;
    this->exit = false;
    this->particlePoints = new SDL_FPoint[particleAmount];
    this->particleVel = new SDL_FPoint[particleAmount];
    randomizeParticles();
}

SDL_FPoint randomFPoint(){
    SDL_FPoint p;
    p.x = -1+2*float(rand())/RAND_MAX;
    p.y = -1+2*float(rand())/RAND_MAX;
    p.x = p.x/2;
    p.y = p.y/2;
    return p;
}

void Gravity::randomizeParticles(){
    for (int i=0; i<this->particleAmount; i++){
        this->particleVel[i] = randomFPoint();
        SDL_FPoint pos = randomFPoint();
        this->particlePoints[i].x = RES*(1+pos.x)/2;
        this->particlePoints[i].y = RES*(1+pos.y)/2;
    }
}

void Gravity::initSDL(){
    if (SDL_Init(SDL_INIT_VIDEO) == 0){
        this->window = SDL_CreateWindow("my gravity sim",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                RES,
                RES,
                SDL_WINDOW_SHOWN
                );
    }
    else return;
    this->renderer = SDL_CreateRenderer(this->window, -1, 0);
    if (this->renderer){
        SDL_SetRenderDrawColor(this->renderer, 0,0,0,255);
        std::cout << "Renderer Created!"<<std::endl;
    }
}

void Gravity::closeSDL(){
    SDL_DestroyWindow(this->window);
    SDL_DestroyRenderer(this->renderer);
    SDL_Quit();
    std::cout << "SDL cleaned out." << std::endl;
}

void Gravity::getInput(){
    while(SDL_PollEvent(&this->currentEvent)){
        switch(this->currentEvent.type){
        case SDL_QUIT:
            this->closeSDL();
            this->exit = true;
            return;
        }
    }
}

void Gravity::render(){
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i=0; i<this->particleAmount; i++){
        float vel = particleVel[i].x*particleVel[i].x+particleVel[i].y*particleVel[i].y;
        SDL_SetRenderDrawColor(renderer, 255, 100*vel, 0, 255);
        SDL_RenderDrawPoint(renderer, particlePoints[i].x, particlePoints[i].y);
        //SDL_RenderDrawPointsF(this->renderer, this->particlePoints, this->particleAmount);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderPresent(renderer);
}

void Gravity::update(float delta){
    // move by velocities
    for (int i = 0; i < this->particleAmount; i++){
        //check boundaries
        if (particlePoints[i].x < 0 || particlePoints[i].x > RES) {
            particleVel[i].x *= -1;
            if (particlePoints[i].x < 0) particlePoints[i].x = 0;
            if (particlePoints[i].x > RES) particlePoints[i].x = RES;
        }

        if (particlePoints[i].y < 0 || particlePoints[i].y > RES) {
            particleVel[i].y *= -1;
            if (particlePoints[i].y < 0) particlePoints[i].y = 0;
            if (particlePoints[i].y > RES) particlePoints[i].y = RES;
        }
        // calculate acceleration with newtonian formula
        SDL_FPoint acc = {0.0, 0.0};
        float soft = 10;
        for (int j = i+1; j < this->particleAmount; j++){
            float x_dis = particlePoints[i].x-particlePoints[j].x;
            float y_dis = particlePoints[i].y-particlePoints[j].y;
            float r2 = x_dis*x_dis+y_dis*y_dis+soft;
            acc.x += G_CONST*sign(x_dis)*x_dis*x_dis/(r2*r2);
            acc.y += G_CONST*sign(y_dis)*y_dis*y_dis/(r2*r2);
        }

        for (int j = i-1; j >= 0; j--){
            float x_dis = particlePoints[i].x-particlePoints[j].x;
            float y_dis = particlePoints[i].y-particlePoints[j].y;;
            float r2 = x_dis*x_dis+y_dis*y_dis+soft;
            acc.x += G_CONST*sign(x_dis)*x_dis*x_dis/(r2*r2);
            acc.y += G_CONST*sign(y_dis)*y_dis*y_dis/(r2*r2);
        }
        
        // full equation of motion
        float decay_const = 0.001;
        particlePoints[i].x += particleVel[i].x*delta + 0.5*acc.x*delta*delta;
        particlePoints[i].y += particleVel[i].y*delta + 0.5*acc.y*delta*delta;
        particleVel[i].x += acc.x*delta + sign(particleVel[i].x)*decay_const;
        particleVel[i].y += acc.y*delta + sign(particleVel[i].y)*decay_const;
    }
}
