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


Gravity::Gravity(int particleAmount, int darkParticles){
    this->particleAmount = particleAmount;
    this->darkParticles = darkParticles;
    this->exit = false;
    this->paused = false;
    this->pPoints = new SDL_FPoint[particleAmount];
    this->pVel = new SDL_FPoint[particleAmount];
    this->dark = new SDL_FPoint[darkParticles];
    this->darkVel = new SDL_FPoint[darkParticles];
    randomizeParticles();
}

Gravity::~Gravity(){
    delete this->pPoints;
    delete this->pVel;
    delete this->dark;
    delete this->darkVel;
}

void Gravity::newSim(){
    delete this->pPoints;
    delete this->pVel;
    delete this->dark;
    delete this->darkVel;
    this->pPoints = new SDL_FPoint[particleAmount];
    this->pVel = new SDL_FPoint[particleAmount];
    this->dark = new SDL_FPoint[darkParticles];
    this->darkVel = new SDL_FPoint[darkParticles];
    randomizeParticles();
}

SDL_FPoint randomFPoint(){
    SDL_FPoint p;
    p.x = -1+2*float(rand())/RAND_MAX;
    p.y = -1+2*float(rand())/RAND_MAX;
    p.x = p.x/2;
    p.y = p.y/2;
    float norm = sqrt(p.x*p.x+p.y*p.y);
    p.x /= norm;
    p.y /= norm;
    norm = -1+2*float(rand())/RAND_MAX;
    p.x *= norm;
    p.y *= norm;
    return p;
}

void Gravity::randomizeParticles(){
    float spin = 753;
    if (rand()%2==0){
        spin *= -1;
    }
    float radius = 0.01;
    for (int i=0; i<this->particleAmount; i++){
        this->pVel[i] = randomFPoint();
        SDL_FPoint pos = randomFPoint();
        this->pPoints[i].x = RES*(1+radius*pos.x)/2;
        this->pPoints[i].y = RES*(1+radius*pos.y)/2;
        this->pVel[i].x += spin*(0.5-this->pPoints[i].y/RES);
        this->pVel[i].y -= spin*(0.5-this->pPoints[i].x/RES);
    }
    for (int i=0; i<this->darkParticles; i++){
        this->darkVel[i] = randomFPoint();
        SDL_FPoint pos = randomFPoint();
        this->dark[i].x = RES*(1+radius*pos.x)/2;
        this->dark[i].y = RES*(1+radius*pos.y)/2;
        this->darkVel[i].x += spin*(0.5-this->dark[i].y/RES);
        this->darkVel[i].y -= spin*(0.5-this->dark[i].x/RES);
    }
}

void Gravity::initSDL(){
    if (SDL_Init(SDL_INIT_VIDEO) == 0){
        this->window = SDL_CreateWindow("LDSG - Little Dark Spiral Galaxy 0.1",
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
        case SDL_KEYDOWN:
            switch (this->currentEvent.key.keysym.sym) {
                case SDLK_r:
                    newSim();
                    break;
                case SDLK_p:
                    this->paused = !this->paused;
                    break;
            }
            break;
        }
    }
}

void Gravity::render(){
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    for (int i=0; i<this->darkParticles; i++){
        //SDL_SetRenderDrawColor(renderer, 255, 100*vel, 0, 255);
        //SDL_RenderDrawPoint(renderer, pPoints[i].x, pPoints[i].y);
        //SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderDrawPointsF(this->renderer, this->dark, this->darkParticles);
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i=0; i<this->particleAmount; i++){
        //SDL_SetRenderDrawColor(renderer, 255, 100*vel, 0, 255);
        //SDL_RenderDrawPoint(renderer, pPoints[i].x, pPoints[i].y);
        //SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderDrawPointsF(this->renderer, this->pPoints, this->particleAmount);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderPresent(renderer);
}

void Gravity::update(float delta){
    // update baryonic matter
    for (int i = 0; i < this->particleAmount; i++){
        //check boundaries
        if (pPoints[i].x < 0 || pPoints[i].x > RES) {
            pVel[i].x *= -1;
            if (pPoints[i].x < 0) pPoints[i].x = 0;
            if (pPoints[i].x > RES) pPoints[i].x = RES;
        }

        if (pPoints[i].y < 0 || pPoints[i].y > RES) {
            pVel[i].y *= -1;
            if (pPoints[i].y < 0) pPoints[i].y = 0;
            if (pPoints[i].y > RES) pPoints[i].y = RES;
        }
        // calculate acceleration with newtonian formula
        SDL_FPoint acc = {0.0, 0.0};
        float soft = 10;
        for (int j = i+1; j < this->particleAmount; j++){
            float x_dis = pPoints[i].x-pPoints[j].x;
            float y_dis = pPoints[i].y-pPoints[j].y;
            float r2 = x_dis*x_dis+y_dis*y_dis+soft;
            acc.x += G_CONST*sign(x_dis)*x_dis*x_dis/(r2*r2);
            acc.y += G_CONST*sign(y_dis)*y_dis*y_dis/(r2*r2);
        }

        for (int j = i-1; j >= 0; j--){
            float x_dis = pPoints[i].x-pPoints[j].x;
            float y_dis = pPoints[i].y-pPoints[j].y;;
            float r2 = x_dis*x_dis+y_dis*y_dis+soft;
            acc.x += G_CONST*sign(x_dis)*x_dis*x_dis/(r2*r2);
            acc.y += G_CONST*sign(y_dis)*y_dis*y_dis/(r2*r2);
        }
        for (int j = 0; j < this->darkParticles; j++){
            float x_dis = pPoints[i].x-dark[j].x;
            float y_dis = pPoints[i].y-dark[j].y;
            float r2 = x_dis*x_dis+y_dis*y_dis+soft;
            acc.x += G_CONST*sign(x_dis)*x_dis*x_dis/(r2*r2);
            acc.y += G_CONST*sign(y_dis)*y_dis*y_dis/(r2*r2);
        }
        
        // full equation of motion
        float decay_const = 0.005;
        pPoints[i].x += pVel[i].x*delta + 0.5*acc.x*delta*delta;
        pPoints[i].y += pVel[i].y*delta + 0.5*acc.y*delta*delta;
        // decay_const for electromagnetic interactions modelling
        pVel[i].x += acc.x*delta + sign(pVel[i].x)*decay_const;
        pVel[i].y += acc.y*delta + sign(pVel[i].y)*decay_const;
    }
    // update dark matter
    for (int i = 0; i < this->darkParticles; i++){
        //check boundaries
        
        if (dark[i].x < 0 || dark[i].x > RES) {
            darkVel[i].x *= -1;
            if (dark[i].x < 0) dark[i].x = 0;
            if (dark[i].x > RES) dark[i].x = RES;
        }

        if (dark[i].y < 0 || dark[i].y > RES) {
            darkVel[i].y *= -1;
            if (dark[i].y < 0) dark[i].y = 0;
            if (dark[i].y > RES) dark[i].y = RES;
        }
        // calculate acceleration with newtonian formula
        SDL_FPoint acc = {0.0, 0.0};
        float soft = 10;
        for (int j = i+1; j < this->darkParticles; j++){
            float x_dis = dark[i].x-dark[j].x;
            float y_dis = dark[i].y-dark[j].y;
            float r2 = x_dis*x_dis+y_dis*y_dis+soft;
            acc.x += G_CONST*sign(x_dis)*x_dis*x_dis/(r2*r2);
            acc.y += G_CONST*sign(y_dis)*y_dis*y_dis/(r2*r2);
        }

        for (int j = i-1; j >= 0; j--){
            float x_dis = dark[i].x-dark[j].x;
            float y_dis = dark[i].y-dark[j].y;;
            float r2 = x_dis*x_dis+y_dis*y_dis+soft;
            acc.x += G_CONST*sign(x_dis)*x_dis*x_dis/(r2*r2);
            acc.y += G_CONST*sign(y_dis)*y_dis*y_dis/(r2*r2);
        }
        for (int j = 0; j < this->particleAmount; j++){
            float x_dis = dark[i].x-pPoints[j].x;
            float y_dis = dark[i].y-pPoints[j].y;
            float r2 = x_dis*x_dis+y_dis*y_dis+soft;
            acc.x += G_CONST*sign(x_dis)*x_dis*x_dis/(r2*r2);
            acc.y += G_CONST*sign(y_dis)*y_dis*y_dis/(r2*r2);
        }
        
        // full equation of motion
        //float decay_const = 0.001;
        dark[i].x += darkVel[i].x*delta + 0.5*acc.x*delta*delta;
        dark[i].y += darkVel[i].y*delta + 0.5*acc.y*delta*delta;
        darkVel[i].x += acc.x*delta;
        //pVel[i].x += acc.x*delta + sign(pVel[i].x)*decay_const;
        darkVel[i].y += acc.y*delta;
        //pVel[i].x += acc.x*delta + sign(pVel[i].x)*decay_const;
    }
}
