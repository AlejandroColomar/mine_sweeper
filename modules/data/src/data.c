/******************************************************************************
 *	Copyright (C) 2015	Alejandro Colomar Andrés		      *
 ******************************************************************************/

		/* bool */
	#include <stdbool.h>
		/* srand() */
	#include <stdlib.h>
		/* strcpy() */
	#include <string.h>
		/* clock_t & clock() & time() */
	#include <time.h>
		/* getpid() */
	#include <unistd.h>

		/* seedf() */
	#include "alx_seed.h"

	#include "data.h"


/*
 *	[][][0] = Valor real	< 0:	0 minas cercanas y todas las cercanas descubiertas
 *				0-8:	N.º de minas cercanas
 *				> 8:	Mina
 *
 *	[][][1] = Estado	0:	Oculta
 *				1:	Descubierta
 *				2:	Bandera V
 *				3:	Bandera F
 *
 *	Interfaz		+:	Celda oculta
 *				·:	0 minas cercanas
 *				1-8:	N.º de minas cercanas
 *				#:	Mina marcada
 *				M:	Mina no explotada
 *				N:	No había mina
 *				X:	Mina explotada
 */


/******************************************************************************
 ******* variables ************************************************************
 ******************************************************************************/


int	prb_lvl [2];
int	prb_sol [3] [DIM_MAX] [DIM_MAX];

int	prb_mtx [2] [DIM_MAX] [DIM_MAX];
int	prb_mtx_rows;
int	prb_mtx_cols;
int	prb_mtx_mines [2];
int	prb_mtx_cleared;
double	p;

int	count;

clock_t	tim_0;
clock_t	tim_1;
double	tim_tot;

int	flag_s;
int	flag_V;
bool	flag_x;

char	file_path [FILENAME_MAX];
int	seed;


/******************************************************************************
 ******* functions ************************************************************
 ******************************************************************************/
void	init_values	(void)
{
	p =		0.05;

	prb_mtx_rows =	10;
	prb_mtx_cols =	10;

	strcpy(file_path, "files/DATA.txt");

	flag_s =	0;
	flag_V =	1;
	flag_x =	false;

	seed =		seedf(clock(), time(NULL), getpid());
	srand(seed);
}

/******************************************************************************
 ******************************************************************************
 **** END OF FILE *************************************************************
 ******************************************************************************
 ******************************************************************************/

/******************************************************************************
 ******************************************************************************
 **** END OF FILE *************************************************************
 ******************************************************************************
 ******************************************************************************/
