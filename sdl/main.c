//
//  main.c
//  sdl
//
//  Created by Klesh Wong on 1/17/16.
//  Copyright © 2016 Klesh Wong. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "types.h"

#define SCREEN_W 640
#define SCREEN_H 480
#define BRICK_W 256
#define BRICK_H 65

#define GRAVITY 0.35f
#define VELOCITY 0.3f
#define SPEED 2

int loadStage(SDL_Renderer *renderer, Stage *stage) {
    stage->man.x = 220;
    stage->man.y = 200;
    stage->man.dx = 0.0f;
    stage->man.dy = 0.0f;
    stage->man.onLand = 0;
    stage->man.frameX = 0;
    stage->man.frameY = 0;
    stage->man.facingLeft = 0;
    stage->man.w = 24;
    stage->man.h = 46;
    stage->man.down = 0;
    stage->man.index = 0;
    stage->scrollX = 0;
    
    stage->time = 0;
    
    // 加载砖块
    SDL_Surface *ledgeSurface = IMG_Load("brick.jpg");
    if (ledgeSurface == 0) {
        printf("brick.jpg 文件不存在\n\n");
        return 1;
    }
    stage->ledge = SDL_CreateTextureFromSurface(renderer, ledgeSurface);
    SDL_FreeSurface(ledgeSurface);
    for (int i = 0; i < 100; i++) {
        stage->ledges[i].w = BRICK_W;
        stage->ledges[i].h = BRICK_H;
        stage->ledges[i].x = i * BRICK_W;
        stage->ledges[i].y = SCREEN_H - BRICK_H;
    }
    stage->ledges[99].x = 256;
    stage->ledges[99].y = 300;
    
    
    // 加载人物
    SDL_Surface *manSurface = IMG_Load("man.png");
    if (manSurface == 0) {
        printf("contra_man.gif 文件不存在\n\n");
        return 1;
    }
    stage->manFrames = SDL_CreateTextureFromSurface(renderer, manSurface);
    SDL_FreeSurface(manSurface);

    // 加载字体
    stage->font = TTF_OpenFont("Unique.ttf", 48);
    if (!stage->font) {
        printf("字体文件 Unique.ttf 不存在\n\n");
        return 1;
    }
    return 0;
}


int processEvent(Stage *stage) {
    Man *man = &stage->man;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT_CLOSE:
            case SDL_QUIT:
                return 0;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        return 0;
                    case SDLK_UP:
                        if (stage->man.onLand && !man->down)
                            stage->man.dy = -8;
                        break;
                }
        }
    }
    

    const Uint8 *state = SDL_GetKeyboardState(NULL);
//    if (state[SDL_SCANCODE_UP] && !man->down) {
//        if (man->dy)
//            man->dy -= 0.23f;
//    }
    
    if (man->onLand) {
        if (state[SDL_SCANCODE_DOWN]) {
            man->down = 1;
            man->w = 34;
            man->h = 20;
            //stage->man.y += 25;
            man->frameX = 0;
            man->frameY = 118;
            man->dx = 0;
        } else {
            stage->man.down = 0;
            
            if (man->dx && stage->time % 8 == 0) {
                man->index++;
                if (man->index >= 5) man->index = 0;
                man->w = 24;
                man->h = 46;
                man->frameX = man->w * (man->index  + 4);
                man->frameY = 0;
            } else if (man->dx == 0) {
                man->w = 24;
                man->h = 46;
                man->frameX = 0;
                man->frameY = 0;
            }

        }
    } else {
        if (stage->time % 8 == 0) {
            man->w = 24;
            man->h = 20;
            man->index++;
            if (man->index >= 4) man->index = 0;
            man->frameX = man->w * (man->index + 2);
            man->frameY = 118;
        }
    }
    
    if (state[SDL_SCANCODE_RIGHT] && !man->down) {
        short allowChange = man->onLand || man->dx >= 0;
        if (allowChange) {
            if (man->dx < SPEED) {
                man->dx += VELOCITY;
            } else {
                man->dx = SPEED;
            }
        }
        man->facingLeft = 0;
    } else if (state[SDL_SCANCODE_LEFT] && !man->down) {
        if (man->dx > -SPEED) {
            man->dx -= VELOCITY;
        } else {
            man->dx = -SPEED;
        }
        man->facingLeft = 1;
    } else {
        man->dx = man->dx * 0.8f;
        if (fabsf(man->dx) < 0.1f) {
            man->dx = 0;
        }
    }
    
    stage->scrollX = -man->x + 100;

    return 1;
}

void collisionDectect(Stage *stage) {
    Man *man = &stage->man;
    man->y += man->dy;
    man->x += man->dx;
    
    stage->man.onLand = 0;
    float mx = stage->man.x, my = stage->man.y;
    for (int i = 0; i < 100; i++) {
        float bx = stage->ledges[i].x, by = stage->ledges[i].y, bw = stage->ledges[i].w, bh = stage->ledges[i].h;
        float mxc = mx + man->w / 2;
        
//        if (mx < 0) {
//            mx = man->x = 0;
//        } else if (mx + man->w > SCREEN_W) {
//            mx = man->x = SCREEN_W - man->w;
//        }
//        if (my < 0) {
//            my = stage->man.y = 0;
//        } else if (my + man->h > SCREEN_H) {
//            my = man->y = SCREEN_H - man->h;
//        }
        
        // 处于同一垂直区
        if (mxc > bx && mxc < bx + bw) {
            
            float brickCeilling = by + bh;
            if (my < brickCeilling && my > by) { // 检测头是不是撞进去了
                my = man->y = brickCeilling;
                man->dy = 0;
            } else {
                float manCeilling = my + man->h;
                // 检测是不是踩穿地板上
                if (manCeilling > by && manCeilling < brickCeilling) {
                    my = man->y = by - man->h;
                }
                // 确认
                if (my + man->h == by)
                    man->onLand = 1;
            }
        }
        
        // 处于同一垂直区
        if (my + man->h > by && my < by + bh) {
            // 检测是不是右边碰到了
            float manRight = mx + man->w;
            if (manRight > bx && manRight < bx + bw) {
                man->x = bx - man->w;
            } else if (mx < bx + bw && mx > bx) {
                man->x = bx + bw;
            }
        }
    }
    
    if (!man->onLand)
        man->dy += GRAVITY;
    else
        man->dy = 0;
}

void doRender(SDL_Renderer *renderer, Stage *stage) {
    // 设置蓝色
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    
    // 填充
    SDL_RenderClear(renderer);
    
    // 设置白色
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // 画砖块
    for (int i = 0 ; i < 100; i++) {
        SDL_Rect brickRect = { stage->scrollX + stage->ledges[i].x, stage->ledges[i].y, stage->ledges[i].w, stage->ledges[i].h };
        SDL_RenderCopy(renderer, stage->ledge, NULL, &brickRect);
    }
    
    // 画人
    SDL_Rect manSrcRect = { stage->man.frameX, stage->man.frameY, stage->man.w, stage->man.h };
    SDL_Rect manDesRect = { stage->scrollX + stage->man.x, stage->man.y, stage->man.w, stage->man.h };
    SDL_RenderCopyEx(renderer, stage->manFrames, &manSrcRect,  &manDesRect, 0, NULL, (stage->man.facingLeft == 1));
   
    // 显示到屏幕上
    SDL_RenderPresent(renderer);
}

int main(int argc, const char * argv[]) {
    char cwd[255];
    getcwd(cwd, 255);
    printf("%s\n", cwd);
    
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    srand((int)time(NULL));
    // 窗口
    window = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, 0);
    
    // 输出
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    Stage stage;
    int loadStageErr = loadStage(renderer, &stage);
    
    if (!loadStageErr) {
        // 事件循环
        while (processEvent(&stage)) {
            stage.time++;
            
             // 修正人物位置
            collisionDectect(&stage);  
            // 画图
            doRender(renderer, &stage);
        }
    }
    
    // 释放资源
    SDL_DestroyTexture(stage.star);
    SDL_DestroyTexture(stage.manFrames);
    SDL_DestroyTexture(stage.ledge);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    TTF_CloseFont(stage.font);
    
    TTF_Quit();
    // 退出  sdl
    SDL_Quit();
    return 0;
}
