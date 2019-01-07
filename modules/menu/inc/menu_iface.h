/******************************************************************************
 *	Copyright (C) 2015	Alejandro Colomar Andrés		      *
 ******************************************************************************/


/******************************************************************************
 ******* include guard ********************************************************
 ******************************************************************************/
# ifndef		MSW_MENU_IFACE_H
	# define	MSW_MENU_IFACE_H


/******************************************************************************
 ******* headers **************************************************************
 ******************************************************************************/
	#include <stdbool.h>


/******************************************************************************
 ******* enums ****************************************************************
 ******************************************************************************/
	enum	Menu_Iface_Mode {
		MENU_IFACE_FOO = 0,
		MENU_IFACE_CLUI,
		MENU_IFACE_TUI,
		MENU_IFACE_GUI
	};


/******************************************************************************
 ******* structs **************************************************************
 ******************************************************************************/
	struct	Menu_Iface_Variables {
		int	level;
		int	rows;
		int	cols;
		double	p;
	};


/******************************************************************************
 ******* variables ************************************************************
 ******************************************************************************/
extern	bool				flag_exit;
extern	int				menu_iface_mode;
extern	struct Menu_Iface_Variables	menu_iface_variables;


/******************************************************************************
 ******* functions ************************************************************
 ******************************************************************************/
void	menu_iface_init		(void);
void	menu_iface_init_iface	(void);
void	menu_iface_cleanup	(void);
void	menu_iface_board	(int *level, int *rows, int *cols, int *mines);
void	menu_iface		(void);


/******************************************************************************
 ******* include guard ********************************************************
 ******************************************************************************/
# endif			/* menu_iface.h */


/******************************************************************************
 ******* end of file **********************************************************
 ******************************************************************************/
