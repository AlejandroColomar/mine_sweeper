/******************************************************************************
 *	Copyright (C) 2015	Alejandro Colomar Andrés		      *
 ******************************************************************************/


/******************************************************************************
 ******* headers **************************************************************
 ******************************************************************************/
/* Standard C ----------------------------------------------------------------*/
		/* errno */
	#include <errno.h>

/* Project -------------------------------------------------------------------*/
		/* game() */
	#include "game.h"
		/* game_iface() */
	#include "game_iface.h"
		/* menu_iface_board() */
	#include "menu_iface.h"
		/* player_iface() */
	#include "player_iface.h"

	#include "start.h"


/******************************************************************************
 ******* variables ************************************************************
 ******************************************************************************/
int	start_mode;


/******************************************************************************
 ******* static functions *****************************************************
 ******************************************************************************/
static	void	start_foo	(void);
static	void	start_rand	(void);
static	void	start_load	(void);


/******************************************************************************
 ******* main *****************************************************************
 ******************************************************************************/
void	start_switch	(void)
{

	switch (start_mode) {
	case START_FOO:
		start_foo();
		break;

	case START_RAND:
		start_rand();
		break;

	case START_LOAD:
		start_load();
		break;
	}
}


/******************************************************************************
 ******* static functions *****************************************************
 ******************************************************************************/
static	void	start_foo	(void)
{

	/* empty */
	;
}

static	void	start_rand	(void)
{
	int	level;
	int	rows;
	int	cols;
	int	mines;
	bool	fail;
	int	r;
	int	c;

	menu_iface_board(&level, &rows, &cols, &mines);
	player_iface_init(rows, cols);
	fail	= player_iface_start(&r, &c);
	if (!fail) {
		game_init_rand(rows, cols, mines, r, c);
		game_iface_init_rand(level, r, c);
		/* game loop */
		game_iface();
	}

	player_iface_cleanup();
}

static	void	start_load	(void)
{
	int	rows;
	int	cols;

	/* size & game init (sets errno) */
	errno	= 0;
	game_init_load(&rows, &cols);

	if (!errno) {
		/* player iface init */
		player_iface_init(rows, cols);

		/* game iface init */
		game_iface_init_load();

		/* game loop */
		game_iface();

		/* user iface cluanup */
		player_iface_cleanup();
	}
}


/******************************************************************************
 ******* end of file **********************************************************
 ******************************************************************************/
