#include "doodler32.h"
#include "enemy.h"

typedef unsigned short u16;
typedef unsigned int u32;

// **Display**
#define OFFSET(r,c, numcols) ((r)*(numcols) + (c))
extern u16* videoBuffer;
#define SCANLINECOUNTER *(volatile unsigned short *)0x4000006
#define REG_DISPCNT *(u16*) 0x4000000
#define MODE3 3
#define BG2_ENABLE (1<<10) // bit 10
// ==Display==

// **Colour**
#define RGB(r,g,b) ((b)<<10 | (g)<<5 | (r))
#define RED RGB(31, 0, 0)
#define BLUE RGB(0, 0, 31)
#define GREEN RGB(0, 31, 0)

#define CYAN RGB(0, 31, 31)
#define MAGENTA RGB(31, 0, 31)
#define YELLOW RGB(31, 31, 0)
#define BLACK RGB(0, 0, 0)
#define WHITE RGB(31, 31, 31)

#define NYELLOW RGB(25, 25, 0)
#define NEARBLUE RGB(0, 0, 27)
// ==Colour

// **Buttons**
#define BUTTON_A	(1<<0)
#define BUTTON_B	(1<<1)
#define BUTTON_SELECT	(1<<2)
#define BUTTON_START	(1<<3)
#define BUTTON_RIGHT	(1<<4)
#define BUTTON_LEFT	(1<<5)
#define BUTTON_UP	(1<<6)
#define BUTTON_DOWN	(1<<7)
#define BUTTON_R	(1<<8)
#define BUTTON_L	(1<<9)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)

#define BUTTONS *(volatile unsigned int *)0x4000130
// ==Buttons==

// **DMA**
typedef struct
{
	const volatile void *src;
	volatile void *dst;
	volatile u32 cnt;
} DMAREC;

#define DMA ((volatile DMAREC *)0x040000B0)

/* DMA channel 0 register definitions*/
#define REG_DMA0SAD         *(vu32*)0x40000B0  /* source address*/
#define REG_DMA0DAD         *(vu32*)0x40000B4  /* destination address*/
#define REG_DMA0CNT         *(vu32*)0x40000B8  /* control register*/

/* DMA channel 1 register definitions*/
#define REG_DMA1SAD         *(vu32*)0x40000BC  /* source address*/
#define REG_DMA1DAD         *(vu32*)0x40000C0  /* destination address*/
#define REG_DMA1CNT         *(vu32*)0x40000C4  /* control register*/

/* DMA channel 2 register definitions*/
#define REG_DMA2SAD         *(vu32*)0x40000C8  /* source address*/
#define REG_DMA2DAD         *(vu32*)0x40000CC  /* destination address*/
#define REG_DMA2CNT         *(vu32*)0x40000D0  /* control register*/

/* DMA channel 3 register definitions */
#define REG_DMA3SAD         *(vu32*)0x40000D4   /* source address*/
#define REG_DMA3DAD         *(vu32*)0x40000D8  /* destination address*/
#define REG_DMA3CNT         *(vu32*)0x40000DC  /* control register*/

/* Defines*/
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

/* Destination address flags */
#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

/* Source address flags */
#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

/* How much to copy flags */
#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

/* When to DMA flags */
#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)

#define DMA_ON (1 << 31)

#define START_ON_FIFO_EMPTY 0x30000000

// ==DMA==

// **GAME**
#define INFO_GUTTER_WIDTH 40
#define DOODLER_SIZE DOODLER32_HEIGHT
#define ENEMY_SIZE ENEMY_WIDTH

typedef struct
{
	int row;
	int col;
	int facing;
} DOODLER;

enum {LEFT, RIGHT};

typedef struct
{
	int row;
	int col;
	int drow;
	int dcol;
} ENEMY;

typedef struct
{
	int row;
	int col;
} JETPACK;

// ==GAME==


// **Prototypes**
void drawImage(const unsigned short arr[]);
void drawImage3(int r, int c, int width, int height, const u16* image);
void drawDoodler(DOODLER doodler);
void drawEnemy(ENEMY sEnemy);
void drawJetpack(JETPACK sJetpack);

void setColour(u16 colour);
void clearScreen();


void setPixel(int r, int c, u16 colour);
void drawRect(int r, int c, int width, int height, u16 colour);

void drawHollowRect(int r, int c, int width, int height, u16 colour);


void waitForVblank();

int collision(int aRow, int aCol, int aWidth, int aHeight, int bRow, int bCol, int bWidth, int bHeight);
int enemyCollision(DOODLER doodler, ENEMY theEnemy);
int jetpackCollision(DOODLER doodler, JETPACK theJetpack);
// ==Prototypes==