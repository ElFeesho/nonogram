#include <SDL/SDL.h>
#include <math.h>

#include "image.h"

/* Window size */
#define WINDOWW 480
#define WINDOWH 360

/* Grid size in blocks */
#define GRIDSIZEX 10
#define GRIDSIZEY 10

static SDL_Surface *screen = NULL;

static char gameboard[GRIDSIZEY][GRIDSIZEX] = { { 0 } }; /* Not particularly ANSI C, a GNU addition, initialising an array to 0's */

static int sel_x = GRIDSIZEX/2;
static int sel_y = GRIDSIZEY/2;

static int stage_x = (WINDOWW - GRIDSIZEX*24)/2;
static int stage_y = (WINDOWH - GRIDSIZEY*24)/2;

/* Forward declarations of helper functions */
static void draw_grid(void);
static void draw_highlight(void);
static void draw_number(int x, int y, int num);
static void put_pixel(int x, int y, unsigned int col);
static void do_move(int x, int y);
static void draw_gameboard(void);
static void reset_board(void);
static int num_font[10][5][5] = 
	{
		{
			{ 0, 1, 1, 1, 0 },
			{ 0, 1, 0, 1, 0 },
			{ 0, 1, 0, 1, 0 },
			{ 0, 1, 0, 1, 0 },
			{ 0, 1, 1, 1, 0 }
		},
		{
			{ 0, 1, 1, 0, 0 },
			{ 0, 0, 1, 0, 0 },
			{ 0, 0, 1, 0, 0 },
			{ 0, 0, 1, 0, 0 },
			{ 0, 1, 1, 1, 0 }
		},
		{
			{ 0, 1, 1, 1, 0 },
			{ 0, 1, 0, 1, 0 },
			{ 0, 0, 1, 1, 0 },
			{ 0, 1, 0, 0, 0 },
			{ 0, 1, 1, 1, 0 }
		},
		{
			{ 0, 1, 1, 1, 0 },
			{ 0, 0, 0, 1, 0 },
			{ 0, 0, 1, 1, 0 },
			{ 0, 0, 0, 1, 0 },
			{ 0, 1, 1, 1, 0 }
		},
		{
			{ 0, 1, 0, 1, 0 },
			{ 0, 1, 0, 1, 0 },
			{ 0, 1, 1, 1, 0 },
			{ 0, 0, 0, 1, 0 },
			{ 0, 0, 0, 1, 0 }
		},
		{
			{ 0, 1, 1, 1, 0 },
			{ 0, 1, 0, 0, 0 },
			{ 0, 1, 1, 1, 0 },
			{ 0, 0, 0, 1, 0 },
			{ 0, 1, 1, 1, 0 }
		},
		{
			{ 0, 0, 1, 1, 0 },
			{ 0, 1, 0, 0, 0 },
			{ 0, 1, 1, 1, 0 },
			{ 0, 1, 0, 1, 0 },
			{ 0, 1, 1, 1, 0 }
		},
		{
			{ 0, 1, 1, 1, 0 },
			{ 0, 0, 0, 1, 0 },
			{ 0, 0, 0, 1, 0 },
			{ 0, 0, 1, 0, 0 },
			{ 0, 0, 1, 0, 0 }
		},
		{
			{ 0, 1, 1, 1, 0 },
			{ 0, 1, 0, 1, 0 },
			{ 0, 1, 1, 1, 0 },
			{ 0, 1, 0, 1, 0 },
			{ 0, 1, 1, 1, 0 }
		},
		{
			{ 0, 1, 1, 1, 0 },
			{ 0, 1, 0, 1, 0 },
			{ 0, 1, 1, 1, 0 },
			{ 0, 0, 0, 1, 0 },
			{ 0, 1, 1, 1, 0 }
		}
	};
	

void gameboard_init(void)
{
	SDL_Init(SDL_INIT_VIDEO);

	screen = SDL_SetVideoMode(WINDOWW,WINDOWH,0,SDL_SWSURFACE);

	image_generate(GRIDSIZEX, GRIDSIZEY);
}

void gameboard_deinit(void)
{
	image_destroy();
	SDL_FreeSurface(screen);
	SDL_Quit();	
}

void gameboard_render(void)
{
	/*
		Draw blank white, then the grid, then the highlighted square
		then the currently drawn squares
	*/

	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,255,255,255));

	draw_grid();

	draw_highlight();

	draw_gameboard();

	SDL_Flip(screen);
}

int gameboard_process_input(void)
{
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		if(ev.type == SDL_QUIT)
			return 0;

		if(ev.type == SDL_KEYDOWN)
		{
			if(ev.key.keysym.sym == SDLK_a)
				return 0;
			if(ev.key.keysym.sym == SDLK_DOWN)
			{
				sel_y++;
				if(sel_y>=GRIDSIZEY)
					sel_y = GRIDSIZEY-1;
			}
			else if(ev.key.keysym.sym == SDLK_UP)
			{
				sel_y--;
				if(sel_y<0)
					sel_y = 0;
			}
			else if(ev.key.keysym.sym == SDLK_RIGHT)
			{
				sel_x++;
				if(sel_x>GRIDSIZEX-1)
					sel_x = GRIDSIZEX-1;
			}
			else if(ev.key.keysym.sym == SDLK_LEFT)
			{
				sel_x--;
				if(sel_x<0)
					sel_x = 0;
			}
			else if(ev.key.keysym.sym == SDLK_SPACE)
			{
				do_move(sel_x, sel_y);
			}
			else if(ev.key.keysym.sym == SDLK_ESCAPE)
			{
				reset_board();
			}
		}
	}

	return 1;
}


/*
	Static helper functions
*/

void draw_grid(void)
{
	int x, y;
	for(y = 0;y<=GRIDSIZEY;y++)
	{
		SDL_Rect t;

		t.x = stage_x+1;
		t.y = stage_y+y*24;
		t.w = GRIDSIZEX*24-1;
		t.h = 1;

		SDL_FillRect(screen, &t, 0);
	}

	for(x = 0;x<=GRIDSIZEY;x++)
	{
		SDL_Rect t;

		t.x = stage_x+x*24;
		t.y = stage_y+1;
		t.w = 1;
		t.h = GRIDSIZEY*24-1;

		SDL_FillRect(screen, &t, 0);
	}

	/*
		Processing power is cheap...
	*/

	for(y = 0;y<GRIDSIZEY;y++)
	{
		int *img_row = image_get_row(y);
		int cx = 0; /* Count of x lines */
		int f = 0; /* How many continuous lines we've found */
		for(x = GRIDSIZEX-1;x>=0;x--)
		{
			if(img_row[x] == 1)
			{
				cx++;
			}
			else /* Just hit a 0 */
			{
				if(cx>0)
				{
					draw_number((stage_x-12)-(f*10),stage_y+y*24+10,cx);
					cx = 0;
					f++;
				}
			}
		}
		if(f == 0 || cx!=0) /* Empty or full row ... lets notify the player */
		{
			draw_number((stage_x-12)-(f*10),stage_y+y*24+10,cx);
		}
	}

	/*
		Processing wise... calculating the columns isn't so great...
	*/

	for(x = 0;x<GRIDSIZEX;x++)
	{
		int cy = 0;
		int f = 0;
		for(y = GRIDSIZEY-1;y>=0;y--)
		{
			int *img_row = image_get_row(y);
			if(img_row[x] == 1)
			{
				cy++;
			}
			else
			{
				if(cy>0)
				{
					draw_number(stage_x+x*24+5, (stage_y-12)-(f*10),cy);
					cy = 0;
					f++;
				}
			}
		}
		if(f == 0 || cy!=0) /* Empty or full column */
		{
			draw_number(stage_x+x*24+5, (stage_y-12)-(f*10),cy);
		}
	}
}

void draw_highlight(void)
{
	SDL_Rect t;

	t.x = stage_x+sel_x*24+1;
	t.y = stage_y+sel_y*24+1;
	t.w = 23;
	t.h = 23;

	SDL_FillRect(screen, &t, SDL_MapRGB(screen->format,0,128,255));	
}

void draw_number(int x, int y, int num)
{
	int ix, iy;
	int n = -1;
	int t = log10(num)+1;
	while(t>0)
	{
		n = num%10;
		for(iy = 0;iy<5;iy++)
		{
			for(ix = 0;ix<5;ix++)
			{
				if(num_font[n][iy][ix] == 1)
				{
					put_pixel(x+t*5+ix,y+iy,SDL_MapRGB(screen->format,0,0,0));
				}
			}		
		}
		num /= 10;
		t--;
	}
}

void put_pixel(int x, int y, unsigned int col)
{
	/* Hate this work-around, rather just alter the memory myself */

	SDL_Rect pos;
	
	pos.x = x;
	pos.y = y;
	pos.w = 1;
	pos.h = 1;
	
	SDL_FillRect(screen, &pos, col);
}

void do_move(int x, int y)
{
	int *row = image_get_row(y);
	if(row[x] == 1)
		gameboard[y][x] = 1-gameboard[y][x];
}

void draw_gameboard(void)
{
	int x, y;

	for(y = 0;y<GRIDSIZEY;y++)
	{
		for(x = 0;x<GRIDSIZEX;x++)
		{
			if(gameboard[y][x] == 1)
			{
				SDL_Rect t;
				t.x = stage_x+x*24+3;
				t.y = stage_y+y*24+3;
				t.w = 19;
				t.h = 19;
				
				SDL_FillRect(screen, &t, SDL_MapRGB(screen->format,0,0,0));
			}
		}
	}
}

void reset_board(void)
{
	image_generate(GRIDSIZEX, GRIDSIZEY);
	int x, y;
	for(y = 0;y<GRIDSIZEY;y++)
	{
		for(x = 0;x<GRIDSIZEX;x++)
		{
			gameboard[y][x] = 0;
		}
	}
}
