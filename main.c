#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>


/* Constants
 ************/
#define NUM_FLAKES 300
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define VIEWPORT_WIDTH (double)SCREEN_WIDTH / SCREEN_HEIGHT
#define VIEWPORT_HEIGHT 1.0
#define PI 3.14159265358979


/* Structs
 **********/
typedef struct _Flake {
    double xOffset;         // X/Y offsets used to determine final screen location -
    double yOffset;         //  otherwise they'd all be on top of each other
    double gravity;         // Amount the snowflake falls in one millisecond
    double wind;            // Like gravity, but for horizontal movement. May be negative.
    double flutterRate;     // Rate at which the flake oscillates
    double flutterOffset;   // Each flake's flutter starts at a different point else it looks like a damn snow disco
    double flutterRad;      // Amount the flake drifts to each side (flutter radius)
    int    shade;           // Color of the snowflake - use same value for all three colors
} Flake;


/* Headers
 **********/


/* Functions
 ************/
int init(SDL_Window** wind, SDL_Renderer** rend)
{
    //TODO: check for errors
    SDL_Init(SDL_INIT_VIDEO);
    
    *wind = SDL_CreateWindow("Particle demo (Snow)",
                             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             SCREEN_WIDTH, SCREEN_HEIGHT,
                             SDL_WINDOW_SHOWN);
    *rend = SDL_CreateRenderer(*wind, -1, 0);
}

Flake* initFlakes()
{
    double minTimeToFall = 4.5;
    double maxTimeToFall = 5.5;
    double minWindRatio = 0.1;
    double maxWindRatio = 0.3;
    double minFlutterAmount = 0.3;
    double maxFlutterAmount = 0.5;
    double minFlutterMag = 0.02;
    double maxFlutterMag = 0.02;
    double timeToFall;
    double windRatio;
    double flutterAmount;
    double flutterMag;
    Flake* flakes;
    int i;
    Flake *flake;
    
    flakes = calloc(NUM_FLAKES, sizeof(Flake));
    
    for (i = 0, flake=flakes; i < NUM_FLAKES; i++, flake++)
    {
        timeToFall = minTimeToFall + (((double) rand() / RAND_MAX) * (maxTimeToFall - minTimeToFall));
        windRatio = minWindRatio + (((double) rand() / RAND_MAX) * (maxWindRatio - minWindRatio));
        flutterAmount = minFlutterAmount + (((double) rand() / RAND_MAX) * (maxFlutterAmount - minFlutterAmount));
        flutterMag = minFlutterMag + (((double) rand() / RAND_MAX) * (maxFlutterMag - minFlutterMag));
        flake->xOffset = ((double) rand() / RAND_MAX) * VIEWPORT_WIDTH;
        flake->yOffset = (double) rand() / RAND_MAX;
        flake->gravity = (VIEWPORT_HEIGHT / (timeToFall * 1000));
        flake->wind = (flake->gravity * windRatio);
        flake->flutterRate = ((2*PI) / (1000/ flutterAmount));
        flake->flutterOffset = (2*PI) * ((double) rand() / RAND_MAX);
        flake->flutterRad = flutterMag;
        flake->shade = (rand() % 80) + 175;
    }
    
    return flakes;
}

void render(SDL_Renderer* renderer, Flake* flakes, unsigned int elapsed) {
    Flake* flake;
    int x;
    int y;
    double flutter;
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    for (int i = 0; i < NUM_FLAKES; i++)
    {
        flake = &flakes[i];
        
        flutter = cos(flake->flutterOffset + (flake->flutterRate * elapsed)) * flake->flutterRad;
        // SCREEN_HEIGHT is not a typo. VIEWPORT_WIDTH * SCREEN_HEIGHT == SCREEN_WIDTH.
        // Otherwise, we would need to divide the first part by VIEWPORT_WIDTH before
        // correcting to screen coordinates, adding an unnecessary operation.
        x = fmod(flake->xOffset + (flake->wind * elapsed) + flutter, VIEWPORT_WIDTH) * SCREEN_HEIGHT;
        y = fmod(flake->yOffset + (flake->gravity * elapsed), VIEWPORT_HEIGHT) * SCREEN_HEIGHT;
        SDL_SetRenderDrawColor(renderer, flake->shade, flake->shade, flake->shade, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawPoint(renderer, x, y);
    }
    SDL_RenderPresent(renderer);
}

int main (int argc, char** argv)
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    int doQuit = false;
    unsigned int currentTick;
    int lastFps = 0;
    int framesThisSecond = 0;
    char windowTitle[100];      // WARNING - buffer overrun potential, check this at some point
    Flake* flakes;
    
    // Initialize code and get the renderer
    init(&window, &renderer);
    
    flakes = initFlakes();
    
    // Main loop
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
        
        currentTick = SDL_GetTicks();
        render(renderer, flakes, currentTick);
        framesThisSecond++;
        
        if (currentTick - lastFps > 1000)
        {
            lastFps = currentTick;
            // If you edit this line, make sure windowTitle has enough space
            sprintf(windowTitle, "Snow particles demo - %dFPS", framesThisSecond);
            SDL_SetWindowTitle(window, windowTitle);
            framesThisSecond = 0;
        }
    }
    
    SDL_Quit();
    return 0;
}
