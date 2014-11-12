#include <iostream>
#include "SDL.h"
#include "boids.h"

int main() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "Error initializing sdl" << std::endl ;
        return EXIT_FAILURE ;
    }
    const SDL_Window* window = SDL_CreateWindow("Flock! Herd! School!"
                                                , SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED
                                                , 600 , 600
                                                , SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL ) ;
    SDL_Delay(1000) ;
    SDL_Quit() ;
    return EXIT_SUCCESS ;
}