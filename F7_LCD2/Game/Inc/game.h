#ifndef GAME_H_
#define GAME_H_

#define LCD_FB_START_ADDRESS       ((uint32_t)0xC0000000)
#define SCR_BUFFER_0		((uint32_t*)LCD_FB_START_ADDRESS)
#define SCR_WIDTH  272
#define SCR_HEIGHT 480

#define MAX_FRUITS 5
#define GRAVITY 0.25f

typedef struct{
	float x,y;
	float vx, vy;
	uint16_t radius;
	int is_cut;
	int active;
	uint32_t color;
} Fruit;


void GameTaskStart(void* pvParameters);
void GameInit(void);
void GameLoop(void);
void Update_Physic(void);
void Touch(void);
void Draw_Frame(void);
void Clear_Frame(void);
void Spawn_Fruit(Fruit *f);

extern int points;
extern char bufor[32];
extern Fruit fruits[MAX_FRUITS];

#endif /* __GAME_H_ */
