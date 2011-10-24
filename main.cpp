#include <stdlib.h>
#include <SDL/SDL.h>


/* Constants
 ************/
#define NUM_FLAKES 300
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define PI 3.14159265358979323846


/* Structs
 **********/
typedef struct _Flake {
    int x;
    int y;
    int gravity;
    int wind;
    double flutter;
} Flake;


/* Functions
 ************/
SDL_Renderer* init(void)
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    //TODO: check for errors
    SDL_Init(SDL_INIT_VIDEO);
    
    window = SDL_CreateWindow("Particle demo (Snow)",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);
    return renderer;
}

void initFlakes(Flake* flakes)
{
    Flake *flake;
    for (int i = 0; i < NUM_FLAKES; i++)
    {
        flake = &flakes[i];
        flake->x = rand() % 640; flake->y = rand() % 480;
        flake->gravity = 3 + rand() % 3;
        flake->wind = 1 + rand() % 3;
        flake->flutter = ((double) rand() / RAND_MAX) * 2 * PI;
    }
}

void updateFlakes(Flake* flakes)
{
    Flake* flake;
    for (int i = 0; i < NUM_FLAKES; i++)
    {
        flake = &flakes[i];
        flake->x += flake->wind;
        if (flake->x > SCREEN_WIDTH)
            flake->x = 0;
        flake->y += flake->gravity;
        if (flake->y > SCREEN_HEIGHT)
            flake->y = 0;
        flake->flutter += 0.1;
    }
}

void render(SDL_Renderer* renderer, Flake* flakes)
{
    Flake* flake;
    int x;
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < NUM_FLAKES; i++)
    {
        flake = &flakes[i];
        x = flake->x + cos(flake->flutter) * 10;
        SDL_RenderDrawPoint(renderer, x, flake->y);
    }
    SDL_RenderPresent(renderer);
}

int main ( int argc, char** argv )
{
    SDL_Renderer* renderer;
    SDL_Event event;
    int doQuit = false;
    unsigned int lastFrameTick;
    unsigned int currentTick;
    
    // Initialize code and get the renderer
    renderer = init();
    
    Flake flakes[NUM_FLAKES];
    initFlakes(flakes);
    
    // Main loop
    lastFrameTick = SDL_GetTicks();
    while (doQuit == false)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                case SDL_QUIT:
                    doQuit = true;
                    break;
            }
        }
        
        // Currently capped at 50fps. Move to a framerate-independate design later
        currentTick = SDL_GetTicks();
        if (currentTick - lastFrameTick > 20)
        {
            lastFrameTick = currentTick;
            updateFlakes(flakes);
            render(renderer, flakes);
        }
    }
    
    SDL_Quit();
    return 0;
}
