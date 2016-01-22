//
//  types.h
//  sdl
//
//  Created by Klesh Wong on 1/20/16.
//  Copyright © 2016 Klesh Wong. All rights reserved.
//

#ifndef types_h
#define types_h

#include <SDL2/SDL.h>
#include <SDL2_Image/SDL_Image.h>
#include <SDL2_ttf/SDL_ttf.h>

#define STAGE_STATUS_READY 0
#define STAGE_STATUS_GOING 1
#define STAGE_STATUS_OVER 2

typedef struct {
    float x,y,w,h;
    short life;
    float dx, dy;
    int onLand;
    int frameX, frameY, index;
    int facingLeft;
    int down;
    char *name;
} Man;

typedef struct {
    float x,y;
} Star;

typedef struct {
    float x,y,w,h;
} Ledge;

typedef struct {
    Man man;
    int time;
    Star stars[100];
    Ledge ledges[100];
    SDL_Texture *star;
    SDL_Texture *ledge;
    SDL_Texture *manFrames;
    SDL_Texture *label;
    TTF_Font *font;
    int status;
    float scrollX;
} Stage;


#endif /* types_h */
