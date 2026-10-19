#include "font.h"
#include "menu.h"
#include "stm32f1xx_hal.h"
#include "dino.h"
#include "oled.h"
#include "key.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "delay.h"

struct Object_Position{
    uint8_t minX, maxX, minY, maxY;
};


int Sorce;
void Show_Sorce(void){
    char sorce[50] = "";
    snprintf(sorce, sizeof(sorce), "%05d", Sorce);
    OLED_PrintASCIIString(98, 0, sorce, &afont8x6, OLED_COLOR_NORMAL);
}


uint16_t Ground_Pos;
void Show_Ground(void){
    if(Ground_Pos < 128)
    {
        for(int i = 0; i < 128; i++)
        {
            OLED_GRAM[7][i] = groundData[i + Ground_Pos];
        }
    }
    else
    {
        for(int i = 0; i < 255-Ground_Pos; i++)
        {
            OLED_GRAM[7][i] = groundData[i + Ground_Pos];
        }
        for(int i = 255-Ground_Pos; i < 128; i++)
        {
            OLED_GRAM[7][i] = groundData[i + Ground_Pos - 255];
        }
    }
}

uint8_t barrierflag;
uint8_t Barrier_Pos;
struct Object_Position barrier;
void Show_Barrier(void){
    OLED_DrawImage(127-Barrier_Pos,44,barriers[barrierflag], OLED_COLOR_NORMAL);
    barrier.minX = 127-Barrier_Pos;
    barrier.maxX = 144-Barrier_Pos;
    barrier.minY = 44;
    barrier.maxY = 62;
}

uint8_t Cloud_Pos;
void Show_Cloud(void){
  OLED_DrawImage(127-Cloud_Pos,9,&cloudImg, OLED_COLOR_NORMAL);
}

uint8_t dino_jump_flag = 0;//0:奔跑 1:上跳
extern uint8_t KeyNum;
uint16_t jump_t;
uint8_t Jump_Pos;
extern float pi;
struct Object_Position dino;
void Show_Dino(void){
    KeyNum = Key_GetNum();
    if(KeyNum == 1)dino_jump_flag = 1;
    Jump_Pos = (uint8_t)(28.0f * sinf(pi * jump_t / 1000.0f));
    if(dino_jump_flag == 0){
        if(Cloud_Pos % 2 == 0)OLED_DrawImage(0,44,dinos[0], OLED_COLOR_NORMAL);
        else OLED_DrawImage(0,44,dinos[1], OLED_COLOR_NORMAL);
    }
    else OLED_DrawImage(0,44-Jump_Pos,dinos[2], OLED_COLOR_NORMAL);
    dino.maxX = 16;
    dino.minX = 0;
    dino.maxY = 62-Jump_Pos;
    dino.minY = 44-Jump_Pos;
}
int isColliding(struct Object_Position *a, struct Object_Position *b){
    if((a->maxX > b->minX) && (a->minX < b->maxX) && (a->maxY > b->minY) && (a->minY < b->maxY)){
        OLED_NewFrame();
        OLED_PrintASCIIString(28, 24,"Game Over",&afont8x6,OLED_COLOR_NORMAL);
        OLED_ShowFrame();
        Delay_s(1);
        OLED_NewFrame();
        OLED_ShowFrame();
        return 1;
    }
    return 0;
}

int DinoGame_Animation(void){
    while(1){
        int returnflag;
        OLED_NewFrame();
        Show_Sorce();
        Show_Ground();
        Show_Barrier();
        Show_Cloud();
        Show_Dino();
        OLED_ShowFrame();
        returnflag = isColliding(&dino, &barrier);
        if(returnflag == 1){
            return 0;
        }
    }
}

void Dino_Tick(void){
    static uint8_t Sorce_cnt,Ground_cnt,Cloud_cnt;
    Sorce_cnt++;
    Ground_cnt++;
    Cloud_cnt++;
    if(Sorce_cnt >= 100){
        Sorce_cnt = 0;
        Sorce++;
    }
    if(Ground_cnt >= 20){
        Ground_cnt = 0;
        Ground_Pos++;
        Barrier_Pos++;
        if(Ground_Pos >= 256)Ground_Pos = 0;
        if(Barrier_Pos >= 144) {
            Barrier_Pos = 0;
            barrierflag = rand() % 3;
        }
    }
    if(Cloud_cnt >= 50){
        Cloud_cnt = 0;
        Cloud_Pos++;
        if(Cloud_Pos >= 200)Cloud_Pos = 0;
    }
    if(dino_jump_flag == 1){
        jump_t++;
        if(jump_t >= 1000){
            jump_t = 0;
            dino_jump_flag = 0;
        }
    }
}

void DinoGame_Pos_Init(void){
    Sorce = Ground_Pos = Barrier_Pos = Cloud_Pos = 0;
}
