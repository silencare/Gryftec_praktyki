#include "../../../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.h"
#include "../../../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.h"
#include "../../../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.h"
#include <stdlib.h>
#include <math.h>
#include "FreeRTOS.h"
#include "../../../Game/Inc/game.h"

Fruit fruits[MAX_FRUITS];

char bufor[32];
int points = 0;


void Update_Physic(){
	for (int i = 0; i < MAX_FRUITS; i++){
		if (!fruits[i].active)
			continue;

		fruits[i].x += fruits[i].vx;
		fruits[i].y += fruits[i].vy;
		fruits[i].vy += GRAVITY;

		if (fruits[i].x - fruits[i].radius <= 0) {
			fruits[i].x = fruits[i].radius;
			fruits[i].vx = -fruits[i].vx;
		} else if (fruits[i].x + fruits[i].radius >= BSP_LCD_GetXSize()){
			fruits[i].x = BSP_LCD_GetXSize() - fruits[i].radius;
			fruits[i].vx = -fruits[i].vx;
		}

		if (fruits[i].y - fruits[i].radius < 0){
			fruits[i].y = fruits[i].radius;
			fruits[i].vy = -fruits[i].vy * 0.8f;
		}
		if (fruits[i].y > BSP_LCD_GetYSize() + 50){
			fruits[i].active = 0;
		}
	}

}


void Touch(){
	TS_StateTypeDef tsState;
	BSP_TS_GetState(&tsState);

	if(tsState.touchDetected){
		uint16_t tx = tsState.touchX[0];
		uint16_t ty = tsState.touchY[0];

		for(int i = 0; i < MAX_FRUITS; i++){
			if(fruits[i].active && !fruits[i].is_cut){
				float dx = tx - fruits[i].x;
				float dy = ty - fruits[i].y;
				if((dx*dx+dy*dy)< (fruits[i].radius * fruits[i].radius)){
					fruits[i].is_cut = 1;
					fruits[i].vy = 2;
					points++;
				}
			}
		}

	}

}


void Draw_Frame(){
	//BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)bufor, LEFT_MODE);
	for(int i = 0; i < MAX_FRUITS; i++){
		if (!fruits[i].active) continue;
		BSP_LCD_SetTextColor(fruits[i].color);
		if(fruits[i].is_cut){
			BSP_LCD_FillCircle(fruits[i].x - 15, fruits[i].y, fruits[i].radius*0.7f);
			BSP_LCD_FillCircle(fruits[i].x + 15, fruits[i].y, fruits[i].radius*0.7f);
		}else{
			BSP_LCD_FillCircle(fruits[i].x, fruits[i].y, fruits[i].radius);
		}
	}
}

void Clear_Frame(){
	//BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)bufor, LEFT_MODE);
	for(int i = 0; i < MAX_FRUITS; i++){
		if (!fruits[i].active) continue;
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		if(fruits[i].is_cut){
			BSP_LCD_FillCircle(fruits[i].x, fruits[i].y, fruits[i].radius);
			BSP_LCD_FillCircle(fruits[i].x - 15, fruits[i].y, fruits[i].radius*0.7f);
			BSP_LCD_FillCircle(fruits[i].x + 15, fruits[i].y, fruits[i].radius*0.7f);
		}else{
			BSP_LCD_FillCircle(fruits[i].x, fruits[i].y, fruits[i].radius);
		}
	}
}

void Spawn_Fruit(Fruit *f){
	f->x = rand() % (SCR_WIDTH - 40) + 20;
	f->y = 240;
	f->vx = (rand() % 4 - 2);
	f->vy = -(rand() % 10 + 5);
	f->radius = 20;
	f->is_cut = 0;
	f->active = 1;
	f-> color = LCD_COLOR_ORANGE;
}


void GameTaskStart(void* pvParameters)
{
	//GameInit();	// to powoluje do zycia kolejny proces, ktory zajmuje sie obsluga ekranu dotykowego
	while (1)
	{
		GameLoop();
	}
}

void GameLoop(void)
{
	  Touch();
	  Clear_Frame();
	  Update_Physic();
	  Draw_Frame();

	  static uint32_t last_tick = 0;
	  if (HAL_GetTick() - last_tick > 1500){
		  for(int i=0; i<MAX_FRUITS; i++){
			  if(!fruits[i].active){
				  Spawn_Fruit(&fruits[i]);
				  break;
			  }
		  }
		  last_tick = HAL_GetTick();
	  }
	  sprintf(bufor, "Points: %d", points);
	  vTaskDelay(pdMS_TO_TICKS(25));
}
