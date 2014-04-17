#include <stdlib.h>
#include "mylib.h"
#include "text.h"

#define NUMENEMIES 4
#define DOODLER_SPEED_DEFAULT 2
#define LIVES_DEFAULT 3

enum {SPLASH, GAME, WIN, LOSE};

void splash();
int game();
void win();
void lose();

int main()
{
	int state = SPLASH;
	int lives = LIVES_DEFAULT;
	while(1)
	{
		switch(state)
		{
			case SPLASH:
				splash(&lives);
				state = GAME;
				break;

			case GAME:
				state = game(&lives);
				break;

			case WIN:
				win();
				state = SPLASH;
				break;

			case LOSE:
				lose();
				state = SPLASH;
				break;

			default:
				break;
		}
	}

}

void splash(int* plives)
{
	REG_DISPCNT = MODE3 | BG2_ENABLE;
	setColour(BLUE);
	drawString(20, 60, "DOODLER GETS JETPACK", WHITE);
	
	DOODLER doodler = {64, 104, RIGHT};
	drawDoodler(doodler);
	drawString(110, 37, "Doodler is sick of jumping!", WHITE);
	drawString(120, 30, "Help Doodler get his Jetpack!", WHITE);
	drawString(140, 60, "Press Start to Play", WHITE);

	*plives = LIVES_DEFAULT;

	while(!KEY_DOWN_NOW(BUTTON_START));
	while(KEY_DOWN_NOW(BUTTON_START));
}

int game(int* plives)
{
	REG_DISPCNT = MODE3 | BG2_ENABLE;
	setColour(BLACK);
	drawRect(0, 0, 240 - INFO_GUTTER_WIDTH, 160, WHITE);
	drawString(0, 240 - INFO_GUTTER_WIDTH, "LIVES:", WHITE);
	drawChar(10, 240 - INFO_GUTTER_WIDTH, *plives + NUMERAL_OFFSET, WHITE);


	DOODLER doodler = {160 - DOODLER_SIZE, 240 - INFO_GUTTER_WIDTH - DOODLER_SIZE, LEFT};
	DOODLER oldDoodler = doodler;
	int doodlerSpeed = DOODLER_SPEED_DEFAULT;
	JETPACK sJetpack = {rand()%50, rand()%50};



	ENEMY enemies [NUMENEMIES];
	ENEMY oldEnemies [NUMENEMIES];
	ENEMY *cur;
	int d[] = {-1, 0, 1};
	int numd = sizeof(d) / sizeof(d[0]);

	for (int i = 0; i < NUMENEMIES; i++)
	{
		enemies[i].row = rand()%60 + rand()%60;
		enemies[i].col = rand()%100 + 60;
		enemies[i].drow = d[rand()%numd];
		enemies[i].dcol = d[rand()%numd];
		while (!enemies[i].drow && !enemies[i].dcol)
		{
			enemies[i].drow = d[rand()%numd];
			enemies[i].dcol = d[rand()%numd];
		}
		oldEnemies[i] = enemies[i];
	}
	
	
	drawDoodler(doodler);
	for (int i = 0; i < NUMENEMIES; i++)
	{
		drawEnemy(enemies[i]);
	}
	drawJetpack(sJetpack);

	drawChar(76, 117, '3', BLUE);
	for (int i = 0; i < 500; i++)
	{
		waitForVblank();
	}
	
	drawChar(76, 117, '3', WHITE);
	drawChar(76, 117, '2', BLUE);
	for (int i = 0; i < 500; i++)
	{
		waitForVblank();
	}
	drawChar(76, 117, '2', WHITE);
	drawChar(76, 117, '1', BLUE);
	for (int i = 0; i < 500; i++)
	{
		waitForVblank();
	}
	drawChar(76, 117, '1', WHITE);
	while(1)
	{
		if(KEY_DOWN_NOW(BUTTON_B))
		{
			while(KEY_DOWN_NOW(BUTTON_B))
			{
				drawChar(76, 117, 'P', RED);
			}
			drawChar(76, 117, 'P', WHITE);
		}
		
		if(KEY_DOWN_NOW(BUTTON_UP))
		{
			doodler.row -= doodlerSpeed;
			if (doodler.row < 0)
			{
				doodler.row = 0;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_DOWN))
		{
			doodler.row += doodlerSpeed;
			if (doodler.row > 160 - DOODLER_SIZE)
			{
				doodler.row = 160 - DOODLER_SIZE;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_LEFT))
		{
			doodler.col -= doodlerSpeed;
			doodler.facing = LEFT;
			if (doodler.col < 0)
			{
				doodler.col = 0;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_RIGHT))
		{
			doodler.col += doodlerSpeed;
			doodler.facing = RIGHT;
			if (doodler.col > 240 - INFO_GUTTER_WIDTH - DOODLER_SIZE)
			{
				doodler.col = 240 - INFO_GUTTER_WIDTH - DOODLER_SIZE;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_SELECT))
		{
			*plives = LIVES_DEFAULT;
			return GAME;
		}
		for (int i = 0; i < NUMENEMIES; i++)
		{
			cur = enemies + i;
			cur->row = cur->row + cur->drow;
			cur->col = cur->col + cur->dcol;
			if(cur->row < 0)
			{
				cur->row = 0;
				cur->drow=-cur->drow;
			}
			if(cur->row > 160 - ENEMY_SIZE)
			{
				cur->row = 160 - ENEMY_SIZE;
				cur->drow=-cur->drow;
			}

			if(cur->col < 0)
			{
				cur->col = 0;
				cur->dcol = -cur->dcol;
			}
			if(cur->col > 240 - INFO_GUTTER_WIDTH - ENEMY_SIZE)
			{
				cur->col = 240 - INFO_GUTTER_WIDTH - ENEMY_SIZE;
				cur->dcol =-cur->dcol;
			}
		}

		for (int i = 0; i < NUMENEMIES; i++)
		{
			if(enemyCollision(doodler, enemies[i]))
			{
				if (*plives)
				{
					*plives -= 1;
					return GAME;
				}
				else
				{
					return LOSE;
				}
			}
		}

		if(jetpackCollision(doodler, sJetpack))
		{
			return WIN;
		}


		waitForVblank();


		drawRect(oldDoodler.row, oldDoodler.col, DOODLER_SIZE, DOODLER_SIZE, WHITE);
		drawDoodler(doodler);
		oldDoodler = doodler;
		for (int i = 0; i < NUMENEMIES; i++)
		{
			drawRect(oldEnemies[i].row, oldEnemies[i].col, ENEMY_SIZE, ENEMY_SIZE, WHITE);
		}
		for (int i = 0; i < NUMENEMIES; i++)
		{
			drawEnemy(enemies[i]);
			oldEnemies[i] = enemies[i];
		}

		drawJetpack(sJetpack);

	}
}

void win()
{
	REG_DISPCNT = MODE3 | BG2_ENABLE;
	setColour(GREEN);
	drawString(20, 85, "YOU WIN! :)", WHITE);
	DOODLER doodler = {64, 104, LEFT};
	drawDoodler(doodler);
	drawString(132, 45, "Press Start to Play Again", WHITE);

	while(!KEY_DOWN_NOW(BUTTON_START));
	while(KEY_DOWN_NOW(BUTTON_START));
}

void lose()
{
	REG_DISPCNT = MODE3 | BG2_ENABLE;
	setColour(RED);
	drawString(20, 85, "YOU LOSE! :(", WHITE);
	ENEMY theEnemy = {66,106,0,0};
	drawEnemy(theEnemy);
	drawString(132, 45, "Press Start to Play Again", WHITE);
	while(!KEY_DOWN_NOW(BUTTON_START));
	while(KEY_DOWN_NOW(BUTTON_START));
}