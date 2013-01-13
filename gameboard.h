#ifndef __GAMEBOARD_H__
#define __GAMEBOARD_H__

/* Initialise and deinitialise the screen surfaces */
extern void gameboard_init(void);
extern void gameboard_deinit(void);

/* Draw the current state of the gameboard to the screen */
extern void gameboard_render(void);

/* Receive input for the game */
extern int gameboard_process_input(void);

#endif
