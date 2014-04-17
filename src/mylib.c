#include "mylib.h"
#include "doodler32.h"
#include "doodler32L.h"
#include "enemy.h"
#include "jetpack.h"
u16* videoBuffer = (u16*) 0x6000000;

// A function to set pixel (r, c) to the colour passed in.
void setPixel(int r, int c, u16 colour)
{
	videoBuffer[OFFSET(r, c, 240)] = colour;
}

void drawImage(const unsigned short arr[])
{
	DMA[3].src = arr;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = (240*160) | DMA_ON;
}

void drawImage3(int r, int c, int width, int height, const u16* image)
{
	for (int i = 0; i < height; i++)
	{
		DMA[3].src = &image[OFFSET(i, 0, width)];
		DMA[3].dst = videoBuffer + OFFSET(r+i, c, 240);
		DMA[3].cnt = width | DMA_ON;
	}
}

void drawDoodler(DOODLER doodler)
{
	if (doodler.facing == RIGHT)
	{
		drawImage3(doodler.row, doodler.col, 32, 32, doodler32);
	}
	else
	{
		drawImage3(doodler.row, doodler.col, 32, 32, doodler32L);
	}
	
}

void drawEnemy(ENEMY sEnemy)
{
	drawImage3(sEnemy.row, sEnemy.col, 28, 28, enemy);
}

void drawJetpack(JETPACK sJetpack)
{
	drawImage3(sJetpack.row, sJetpack.col, 16, 24, jetpack);
}


void setColour(volatile u16 colour)
{
	DMA[3].src = &colour;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = (240*160) | DMA_SOURCE_FIXED | DMA_ON;
}

void clearScreen()
{
	setColour(BLACK);
}

// A function to draw a FILLED rectangle starting at (r, c).
void drawRect(int r, int c, int width, int height, u16 colour)
{
    for (int i = 0; i < height; i++)
	{
		DMA[3].src = &colour;
		DMA[3].dst = videoBuffer + OFFSET(r+i, c, 240);
		DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_ON;
	}
}

// A function to draw a HOLLOW rectangle starting at (r, c).
void drawHollowRect(int r, int c, int width, int height, u16 colour)
{
	int row, col;
	row = 0;
	for (col = 0; col < width; col++) {
		setPixel(r+row, c+col, colour);
	}

	col = 0;
	for (row = 1; row < height - 1; row++) {
		setPixel(r+row, c+col, colour);
	}

	col = width - 1;
	for (row = 1; row < height - 1; row++) {
		setPixel(r+row, c+col, colour);
	}


    row = height - 1;
	for (col = 0; col < width; col++) {
		setPixel(r+row, c+col, colour);
	}
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}
//int collisionBufferZone
int collision(int aRow, int aCol, int aWidth, int aHeight, int bRow, int bCol, int bWidth, int bHeight)
{
	if (aRow < (bRow + bHeight) && (aRow + aHeight) > bRow)
	{
		if (aCol < (bCol + bWidth) && (aCol + aWidth) > bCol)
		{
			return 1;
		}
	}
	return 0;
}

int enemyCollision(DOODLER doodler, ENEMY theEnemy)
{
	return collision(doodler.row, doodler.col, DOODLER_SIZE, DOODLER_SIZE, theEnemy.row, theEnemy.col, ENEMY_SIZE, ENEMY_SIZE);
}

int jetpackCollision(DOODLER doodler, JETPACK theJetpack)
{
	return collision(doodler.row, doodler.col, DOODLER_SIZE, DOODLER_SIZE, theJetpack.row, theJetpack.col, JETPACK_WIDTH, JETPACK_HEIGHT);
}
