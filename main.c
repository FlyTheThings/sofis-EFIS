#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "ladder-gauge.h"

#include "odo-gauge.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define N_COLORS 4

OdoGauge *gauge = NULL;
LadderGauge *ladder = NULL;
OdoGauge *wheel = NULL;
OdoGauge *odo = NULL;

//float alt = 1150.0;
float alt = 9800.0;
float odo_val = 842.0;
#define ODO_INC 1

/*Return true to quit the app*/
bool handle_keyboard(SDL_KeyboardEvent *event)
{
    switch(event->keysym.sym){
        case SDLK_ESCAPE:
            if(event->state == SDL_PRESSED)
                return true;
            break;
        case SDLK_a:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 50);
            break;
        case SDLK_b:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 30);
            break;
        case SDLK_c:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 80);
            break;
        case SDLK_d:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 99);
            break;
        case SDLK_e:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 0);
            break;
        case SDLK_f:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 10);
            break;
        case SDLK_g:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 75);
            break;
        case SDLK_h:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 77.5);
            break;
        case SDLK_i:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 78);
            break;
        case SDLK_j:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 79);
            break;
        case SDLK_k:
            if(event->state == SDL_PRESSED)
                odo_gauge_set_value(gauge, 80);
            break;
        case SDLK_UP:
            if(event->state == SDL_PRESSED){
                alt += 150;
                animated_gauge_set_value(ANIMATED_GAUGE(ladder), alt);
                odo_gauge_set_value(wheel, alt);
            }
            break;
        case SDLK_DOWN:
            if(event->state == SDL_PRESSED){
                alt -= 150;
                animated_gauge_set_value(ANIMATED_GAUGE(ladder), alt);
                odo_gauge_set_value(wheel, alt);
            }
            break;
        case SDLK_l:
            if(event->state == SDL_PRESSED){
                odo_gauge_set_value(wheel, 300);
            }
            break;
        case SDLK_PAGEUP:
            if(event->state == SDL_PRESSED){
                if(odo_val < odo->max_value)
                    odo_val += ODO_INC;
                odo_gauge_set_value(odo, odo_val);
            }
            break;
        case SDLK_PAGEDOWN:
            if(event->state == SDL_PRESSED){
                if(odo_val > 0.0)
                    odo_val -= ODO_INC;
                odo_gauge_set_value(odo, odo_val);
            }
            break;
    }
    return false;
}

/*Return true to quit the app*/
bool handle_events(void)
{
    SDL_Event event;

    while(SDL_PollEvent(&event) == 1){
        switch(event.type){
            case SDL_QUIT:
                return true;
                break;
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_CLOSE)
                return true;
            break;
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                return handle_keyboard(&(event.key));
                break;
        }
    }
    return false;
}


int main(int argc, char **argv)
{
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    Uint32 colors[N_COLORS];
    bool done;
    int i;
    float oldv[3] = {0,0,0};

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    if( TTF_Init() == -1 ){
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }


    window = SDL_CreateWindow(
                "hello_sdl2",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN
                );
    if (window == NULL) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        return 1;
    }

    screenSurface = SDL_GetWindowSurface(window);
    if(!screenSurface){
        printf("Error: %s\n",SDL_GetError());
        exit(-1);
    }


    colors[0] = SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF);
    colors[1] = SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0x00);
    colors[2] = SDL_MapRGB(screenSurface->format, 0x00, 0xFF, 0x00);
    colors[3] = SDL_MapRGB(screenSurface->format, 0x11, 0x56, 0xFF);

    gauge = odo_gauge_new(digit_barrel_new(61, 0, 99,10),-1,-1);

    ladder = ladder_gauge_new(BOTTUM_UP, -1);
    animated_gauge_set_value(ANIMATED_GAUGE(ladder), alt);

    DigitBarrel *db = digit_barrel_new(18, 0, 9.999, 1);
    DigitBarrel *db2 = digit_barrel_new(18, 0, 99, 10);
    wheel = odo_gauge_new_multiple(-1, 4,
            -1, db2,
            -2, db,
            -2, db,
            -2, db
    );
    odo_gauge_set_value(wheel, alt);

    DigitBarrel *db4 = digit_barrel_new(18, 0, 9.999, 1);
    DigitBarrel *db3 = digit_barrel_new(18, 0, 99, 10);
    odo = odo_gauge_new_multiple(-1, 3,
            -1, db3,
            -2, db4,
            -2, db4
    );
    odo_gauge_set_value(odo, odo_val);


    done = false;
    Uint32 ticks;
    Uint32 last_ticks = 0;
    Uint32 elapsed = 0;
    Uint32 acc = 0;
    i = 3;
    SDL_Rect dst = {SCREEN_WIDTH/2,SCREEN_HEIGHT/2,0,0};
    SDL_Rect lrect = {150,20,0,0};
    SDL_Rect wheelrect = {400,20,0,0};
    SDL_Rect odorect = {SCREEN_WIDTH/2 - 80,SCREEN_HEIGHT/2,0,0};
    do{
        ticks = SDL_GetTicks();
        elapsed = ticks - last_ticks;
        acc += elapsed;

        done = handle_events();

        SDL_FillRect(screenSurface, NULL, colors[i]);
        SDL_BlitSurface(animated_gauge_render(ANIMATED_GAUGE(ladder), elapsed) , NULL, screenSurface, &lrect);
        SDL_BlitSurface(animated_gauge_render(ANIMATED_GAUGE(gauge), elapsed) , NULL, screenSurface, &dst);
        SDL_BlitSurface(animated_gauge_render(ANIMATED_GAUGE(wheel), elapsed) , NULL, screenSurface, &wheelrect);
        SDL_BlitSurface(animated_gauge_render(ANIMATED_GAUGE(odo), elapsed) , NULL, screenSurface, &odorect);
        SDL_UpdateWindowSurface(window);

        if(elapsed < 200){
            SDL_Delay(200 - elapsed);
        }
        if(acc >= 1000){
            if(ANIMATED_GAUGE(ladder)->value != oldv[0]){
                printf("Ladder value: %f\n",ANIMATED_GAUGE(ladder)->value);
                oldv[0] = ANIMATED_GAUGE(ladder)->value;
            }
            if(ANIMATED_GAUGE(gauge)->value != oldv[1]){
                printf("Rotary gauge value: %f\n",ANIMATED_GAUGE(gauge)->value);
                oldv[1] = ANIMATED_GAUGE(gauge)->value;
            }
            if(ANIMATED_GAUGE(odo)->value != oldv[2]){
                printf("Odo gauge value: %f\n",ANIMATED_GAUGE(odo)->value);
                oldv[2] = ANIMATED_GAUGE(odo)->value;
            }

            acc = 0;
        }

        i %= N_COLORS;
        last_ticks = ticks;
    }while(!done);

    odo_gauge_free(gauge);
    odo_gauge_free(wheel);
    odo_gauge_free(odo);
    ladder_gauge_free(ladder);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;

}

