#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>


/* Constants
 ************/
const int   SCREEN_WIDTH = 640;
const int   SCREEN_HEIGHT = 480;
#define     VIEWPORT_WIDTH (float)SCREEN_WIDTH / SCREEN_HEIGHT
const float VIEWPORT_HEIGHT = 1.0;
const float PI = 3.14159265358979;
// Constants controlling particle generation
const int   NUM_FLAKES = 500;
const float MIN_TIME_TO_FALL = 3.0;
const float MAX_TIME_TO_FALL = 4.5;
const float MIN_WIND_RATIO = 0.1;
const float MAX_WIND_RATIO = 0.5;
const float MIN_FLUTTER_RATE = 0.3;
const float MAX_FLUTTER_RATE = 0.5;
const float MIN_FLUTTER_MAG = 0.01;
const float MAX_FLUTTER_MAG = 0.035;


/* Structs
 **********/
typedef struct _Flake {
    float xOffset;         // X/Y offsets used to determine final screen location -
    float yOffset;         //  otherwise they'd all be on top of each other
    float gravity;         // Amount the snowflake falls in one millisecond
    float wind;            // Like gravity, but for horizontal movement. May be negative.
    float flutterRate;     // Rate at which the flake oscillates
    float flutterOffset;   // Each flake's flutter starts at a different point else it looks like a damn snow disco
    float flutterRad;      // Amount the flake drifts to each side (flutter radius)
    int   shade;           // Color of the snowflake - use same value for all three colors
} Flake;


/* Headers
 **********/


/* Functions
 ************/
int init(SDL_Window** window, SDL_Renderer** rend)
{
    //TODO: check for errors
    SDL_Init(SDL_INIT_VIDEO);
    
    *window = SDL_CreateWindow("Particle demo (Snow)",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               SCREEN_WIDTH, SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);
    *rend = SDL_CreateRenderer(*window, -1, 0);
}

Flake* initFlakes()
{
    float  timeToFall;
    float  windRatio;
    float  flutterAmount;
    float  flutterMag;
    Flake* flakes;
    int    i;
    Flake *flake;
    
    flakes = calloc(NUM_FLAKES, sizeof(Flake));
    
    for (i = 0, flake=flakes; i < NUM_FLAKES; i++, flake++)
    {
        timeToFall = MIN_TIME_TO_FALL  + (((float) rand() / RAND_MAX) * (MAX_TIME_TO_FALL - MIN_TIME_TO_FALL));
        windRatio = MIN_WIND_RATIO + (((float) rand() / RAND_MAX) * (MAX_WIND_RATIO - MIN_WIND_RATIO));
        flutterAmount = MIN_FLUTTER_RATE + (((float) rand() / RAND_MAX) * (MAX_FLUTTER_RATE - MIN_FLUTTER_RATE));
        flutterMag = MIN_FLUTTER_MAG + (((float) rand() / RAND_MAX) * (MAX_FLUTTER_MAG - MIN_FLUTTER_MAG));
        flake->xOffset = ((float) rand() / RAND_MAX) * VIEWPORT_WIDTH;
        flake->yOffset = (float) rand() / RAND_MAX;
        flake->gravity = (VIEWPORT_HEIGHT / (timeToFall * 1000));   // convert seconds to ms
        flake->wind = (flake->gravity * windRatio);
        flake->flutterRate = ((2*PI) / (1000/ flutterAmount));
        flake->flutterOffset = (2*PI) * ((float) rand() / RAND_MAX);
        flake->flutterRad = flutterMag;
        flake->shade = 255 - (192 * ((timeToFall - MIN_TIME_TO_FALL) / (MAX_TIME_TO_FALL - MIN_TIME_TO_FALL)));
    }
    
    return flakes;
}

void render(SDL_Renderer* renderer, Flake* flakes, unsigned int elapsed) {
    Flake* flake;
    int    x;
    int    y;
    float  flutter;
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    for (int flakeid = 0; flakeid < NUM_FLAKES; flakeid++)
    {
        flake = &flakes[flakeid];
        
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
