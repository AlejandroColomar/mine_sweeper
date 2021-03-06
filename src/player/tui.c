/******************************************************************************
 *	Copyright (C) 2015	Alejandro Colomar Andrés		      *
 ******************************************************************************/


/******************************************************************************
 ******* headers **************************************************************
 ******************************************************************************/
#include "mine-sweeper/player/tui.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <ncurses.h>

#include <libalx/extra/ncurses/ncurses.h>

#include "mine-sweeper/game/iface.h"
#include "mine-sweeper/player/iface.h"


/******************************************************************************
 ******* macros ***************************************************************
 ******************************************************************************/
#define REFRESH_TIME_MS		(100)


/******************************************************************************
 ******* variables ************************************************************
 ******************************************************************************/
/* Global --------------------------------------------------------------------*/
	bool	flag_color;
/* Static --------------------------------------------------------------------*/
static	WINDOW	*win_board;
static	WINDOW	*win_help;
static	int	last_help;


/******************************************************************************
 ******* static functions *****************************************************
 ******************************************************************************/
	/* Start */
static	void	show_board_start(const struct Player_Iface_Position  *position,
				const char *title, const char *subtitle);

static	void	board_loop_start(const struct Player_Iface_Position  *position);

	/* Play */
static	void	show_board	(const struct Game_Iface_Out  *board,
				const struct Player_Iface_Position  *position,
				const char *title, const char *subtitle);

static	void	board_loop	(const struct Game_Iface_Out  *board,
				const struct Player_Iface_Position  *position);

static	void	highlight_cursor(int c,
				const struct Player_Iface_Position  *position);

static	int	set_char	(int tile);
static	void	show_char	(int row, int col, int c);
	/* Input */
static	int	usr_input	(void);
	/* Help */
static	void	show_help		(const struct Game_Iface_Out  *board);
static	void	show_help_start		(void);
static	void	show_help_play		(void);
static	void	show_help_pause		(void);
static	void	show_help_xyzzy		(void);
static	void	show_help_cheat		(void);
static	void	show_help_safe		(void);
static	void	show_help_gameover	(void);


/******************************************************************************
 ******* main *****************************************************************
 ******************************************************************************/
void	player_tui_init		(ptrdiff_t rows, ptrdiff_t cols)
{
	int	h1;
	int	w1;
	int	r1;
	int	c1;
	int	h2;
	int	w2;
	int	r2;
	int	c2;

	alx_ncurses_resume();

	flag_color	= false;
	if (has_colors()) {
		flag_color	= true;
		/* Clear */
		init_pair(PAIR_1, COLOR_BLUE, COLOR_WHITE);
		init_pair(PAIR_2, COLOR_GREEN, COLOR_WHITE);
		init_pair(PAIR_3, COLOR_RED, COLOR_WHITE);
		init_pair(PAIR_4, COLOR_BLUE, COLOR_WHITE);
		init_pair(PAIR_5, COLOR_RED, COLOR_WHITE);
		init_pair(PAIR_6, COLOR_CYAN, COLOR_WHITE);
		init_pair(PAIR_7, COLOR_BLACK, COLOR_WHITE);
		init_pair(PAIR_8, COLOR_BLACK, COLOR_WHITE);
		init_pair(PAIR_BLANK, COLOR_BLACK, COLOR_WHITE);
		/* Hidden */
		init_pair(PAIR_MINE, COLOR_WHITE, COLOR_BLACK);
		init_pair(PAIR_HIDDEN, COLOR_WHITE, COLOR_BLACK);
		/* Possible */
		init_pair(PAIR_fail, COLOR_RED, COLOR_BLACK);
		init_pair(PAIR_POSSIBLE, COLOR_BLUE, COLOR_BLACK);
		/* Flag */
		init_pair(PAIR_FAIL, COLOR_RED, COLOR_BLACK);
		init_pair(PAIR_FLAG, COLOR_RED, COLOR_BLACK);
		/* Kboom */
		init_pair(PAIR_KBOOM, COLOR_BLACK, COLOR_RED);
		/* Highlight */
		init_pair(PAIR_HILITE, COLOR_YELLOW, COLOR_RED);
		/* Terminal */
		init_pair(PAIR_TERM, -1, -1);
	}

	/* Dimensions: board */
	h1	= rows + 2;
	w1	= 2 * cols + 3;
	r1	= 0;
	c1	= 11;
	win_board	= newwin(h1, w1, r1, c1);

	/* Dimensions: help */
	h2	= 24;
	w2	= 10;
	r2	= 0;
	c2	= 0;
	win_help	= newwin(h2, w2, r2, c2);

	/* Activate keypad, don't echo input, and set timeout = REFRESH_TIME_MS ms */
	keypad(win_board, true);
	noecho();
	wtimeout(win_board, REFRESH_TIME_MS);
}

int	player_tui_start	(const struct Player_Iface_Position  *position,
				const char *title, const char *subtitle,
				int *action)
{

	show_help_start();
	show_board_start(position, title, subtitle);
	*action	= usr_input();

	return	0;
}

int	player_tui		(const struct Game_Iface_Out  *board,
				const struct Player_Iface_Position  *position,
				const char *title, const char *subtitle,
				int *action)
{

	show_help(board);
	show_board(board, position, title, subtitle);
	*action	= usr_input();

	return	0;
}

void	player_tui_save_name	(const char *fpath,
				char fname[static restrict FILENAME_MAX])
{
	int	w;
	int	r;

	w	= 60;
	r	= 10;
	alx_ncurses_get_fname(fpath, fname, false, w, r, "File name:", NULL, 2);
}

void	player_tui_score_name	(char player_name[static restrict BUFSIZ])
{
	int	w;
	int	r;

	w	= 60;
	r	= 10;
	alx_ncurses_get_nstr(BUFSIZ, player_name, w, r, "Your name:", NULL);
}

void	player_tui_cleanup	(void)
{

	alx_ncurses_delwin(win_board);
	alx_ncurses_delwin(win_help);
	alx_ncurses_pause();
}


/******************************************************************************
 ******* static functions *****************************************************
 ******************************************************************************/
/*	*	*	*	*	*	*	*	*	*
 *	*	* Start	*	*	*	*	*	*	*
 *	*	*	*	*	*	*	*	*	*/
static	void	show_board_start(const struct Player_Iface_Position  *position,
				const char *title, const char *subtitle)
{
	werase(win_board);
	box(win_board, 0, 0);

	alx_ncurses_title(win_board, title);
	alx_ncurses_subtitle(win_board, subtitle);
	board_loop_start(position);
	wmove(win_board, 1 + position->row, 2 + 2 * position->col);

	wrefresh(win_board);
}

static	void	board_loop_start(const struct Player_Iface_Position  *position)
{
	ptrdiff_t	k;
	ptrdiff_t	l;
	int		c;

	c	= PLAYER_TUI_CHAR_HIDDEN_FIELD;

	for (ptrdiff_t i = 0; i < position->rows; i++) {
		k = 1 + i;

		/* hidden */
		for (ptrdiff_t j = 0; j < position->cols; j++) {
			l = 2 + 2 * j;

			show_char(k, l, c);
		}
	}

	/* Highlight cursor */
	if (position->highlight)
		highlight_cursor(c, position);
}

/*	*	*	*	*	*	*	*	*	*
 *	*	* Play	*	*	*	*	*	*
 *	*	*	*	*	*	*	*	*	*/
static	void	show_board	(const struct Game_Iface_Out  *board,
				const struct Player_Iface_Position  *position,
				const char *title, const char *subtitle)
{

	werase(win_board);
	box(win_board, 0, 0);

	alx_ncurses_title(win_board, title);
	alx_ncurses_subtitle(win_board, subtitle);

	board_loop(board, position);
	wmove(win_board, 1 + position->row, 2 + 2 * position->col);

	wrefresh(win_board);
}

static	void	board_loop	(const struct Game_Iface_Out  *board,
				const struct Player_Iface_Position  *position)
{
	ptrdiff_t	k;
	ptrdiff_t	l;
	int		c;

	for (ptrdiff_t i = 0; i < board->rows; i++) {
		k = 1 + i;

		/* clear */
		for (ptrdiff_t j = 0; j < board->cols; j++) {
			l = 2 + 2 * j;
			if (board->usr[i][j] == GAME_IFACE_USR_CLEAR) {
				c	= set_char(board->visible[i][j]);
				show_char(k, l, c);
			}
		}
		/* xyzzy */
		/* hidden */
		for (ptrdiff_t j = 0; j < board->cols; j++) {
			l = 2 + 2 * j;
			if (board->usr[i][j] != GAME_IFACE_USR_CLEAR) {
				c	= set_char(board->visible[i][j]);
				show_char(k, l, c);
			}
		}
		/* kboom */
		for (ptrdiff_t j = 0; j < board->cols; j++) {
			l = 2 + 2 * j;
			if (board->usr[i][j] == GAME_IFACE_USR_KBOOM) {
				c	= set_char(board->visible[i][j]);
				show_char(k, l, c);
			}
		}
	}

	/* Highlight cursor */
	if (position->highlight) {
		c	= set_char(board->visible[position->row][position->col]);
		highlight_cursor(c, position);
	}
}

static	void	highlight_cursor(int c,
				const struct Player_Iface_Position  *position)
{
	ptrdiff_t	k;
	ptrdiff_t	l;
	int		pair;

	k	= 1 + position->row;
	l	= 2 + 2 * position->col;

	pair	= PAIR_HILITE;
	if (flag_color)
		wattron(win_board, A_BOLD | COLOR_PAIR(pair));
	mvwaddch(win_board, k, l - 1, '<');
	mvwaddch(win_board, k, l, c);
	mvwaddch(win_board, k, l + 1, '>');
	if (flag_color)
		wattroff(win_board, A_BOLD | COLOR_PAIR(pair));
}

static	int	set_char	(int tile)
{
	int	c;

	switch (tile) {
	case GAME_IFACE_VIS_KBOOM:
		c	= PLAYER_TUI_CHAR_KBOOM;
		break;
	case GAME_IFACE_VIS_HIDDEN_FIELD:
		c	= PLAYER_TUI_CHAR_HIDDEN_FIELD;
		break;
	case GAME_IFACE_VIS_HIDDEN_MINE:
		c	= PLAYER_TUI_CHAR_HIDDEN_MINE;
		break;
	case GAME_IFACE_VIS_HIDDEN_SAFE:
		c	= PLAYER_TUI_CHAR_HIDDEN_SAFE;
		break;
	case GAME_IFACE_VIS_SAFE_MINE:
		c	= PLAYER_TUI_CHAR_SAFE_MINE;
		break;
	case GAME_IFACE_VIS_0:
		c	= PLAYER_TUI_CHAR_0;
		break;
	case GAME_IFACE_VIS_1:
		c	= PLAYER_TUI_CHAR_1;
		break;
	case GAME_IFACE_VIS_2:
		c	= PLAYER_TUI_CHAR_2;
		break;
	case GAME_IFACE_VIS_3:
		c	= PLAYER_TUI_CHAR_3;
		break;
	case GAME_IFACE_VIS_4:
		c	= PLAYER_TUI_CHAR_4;
		break;
	case GAME_IFACE_VIS_5:
		c	= PLAYER_TUI_CHAR_5;
		break;
	case GAME_IFACE_VIS_6:
		c	= PLAYER_TUI_CHAR_6;
		break;
	case GAME_IFACE_VIS_7:
		c	= PLAYER_TUI_CHAR_7;
		break;
	case GAME_IFACE_VIS_8:
		c	= PLAYER_TUI_CHAR_8;
		break;
	case GAME_IFACE_VIS_FLAG:
		c	= PLAYER_TUI_CHAR_FLAG;
		break;
	case GAME_IFACE_VIS_FLAG_FALSE:
		c	= PLAYER_TUI_CHAR_FLAG_FALSE;
		break;
	case GAME_IFACE_VIS_POSSIBLE:
		c	= PLAYER_TUI_CHAR_POSSIBLE;
		break;
	case GAME_IFACE_VIS_POSSIBLE_FALSE:
		c	= PLAYER_TUI_CHAR_POSSIBLE_FALSE;
		break;
	default:
		c	= PLAYER_TUI_CHAR_ERROR;
		break;
	}

	return	c;
}

static	void	show_char	(int row, int col, int c)
{
	int	pair;

	switch (c) {
	case PLAYER_TUI_CHAR_1:
		pair	= PAIR_1;
		break;
	case PLAYER_TUI_CHAR_2:
		pair	= PAIR_2;
		break;
	case PLAYER_TUI_CHAR_3:
		pair	= PAIR_3;
		break;
	case PLAYER_TUI_CHAR_4:
		pair	= PAIR_4;
		break;
	case PLAYER_TUI_CHAR_5:
		pair	= PAIR_5;
		break;
	case PLAYER_TUI_CHAR_6:
		pair	= PAIR_6;
		break;
	case PLAYER_TUI_CHAR_7:
		pair	= PAIR_7;
		break;
	case PLAYER_TUI_CHAR_8:
		pair	= PAIR_8;
		break;
	case PLAYER_TUI_CHAR_0:
		pair	= PAIR_BLANK;
		break;
	case PLAYER_TUI_CHAR_HIDDEN_MINE:
	case PLAYER_TUI_CHAR_SAFE_MINE:
		pair	= PAIR_MINE;
		break;
	case PLAYER_TUI_CHAR_HIDDEN_FIELD:
	case PLAYER_TUI_CHAR_HIDDEN_SAFE:
		pair	= PAIR_HIDDEN;
		break;
	case PLAYER_TUI_CHAR_POSSIBLE_FALSE:
		pair	= PAIR_fail;
		break;
	case PLAYER_TUI_CHAR_POSSIBLE:
		pair	= PAIR_POSSIBLE;
		break;
	case PLAYER_TUI_CHAR_FLAG_FALSE:
		pair	= PAIR_FAIL;
		break;
	case PLAYER_TUI_CHAR_FLAG:
		pair	= PAIR_FLAG;
		break;
	case PLAYER_TUI_CHAR_KBOOM:
		pair	= PAIR_KBOOM;
		break;
	default:
		pair	= PAIR_KBOOM;
		break;
	}

	if (flag_color)
		wattron(win_board, A_BOLD | COLOR_PAIR(pair));
	mvwaddch(win_board, row, col - 1, ' ');
	mvwaddch(win_board, row, col, c);
	mvwaddch(win_board, row, col + 1, ' ');
	if (flag_color)
		wattroff(win_board, A_BOLD | COLOR_PAIR(pair));
}

/*	*	*	*	*	*	*	*	*	*
 *	*	* Input	*	*	*	*	*	*	*
 *	*	*	*	*	*	*	*	*	*/
static	int	usr_input	(void)
{
	int	c;
	int	action;

	c = wgetch(win_board);

	switch (c) {
	case '+':
	case '\r':
	case '\n':
		action	= PLAYER_IFACE_ACT_STEP;
		break;
	case ' ':
		action	= PLAYER_IFACE_ACT_FLAG;
		break;
	case 'f':
		action	= PLAYER_IFACE_ACT_FLAG_POSSIBLE;
		break;

		/* ASCII 0x08 is BS */
	case KEY_BACKSPACE:
	case 0x08:
		action	= PLAYER_IFACE_ACT_RM_FLAG;
		break;
	case KEY_BREAK:
	case 'p':
		action	= PLAYER_IFACE_ACT_PAUSE;
		break;
	case 's':
		action	= PLAYER_IFACE_ACT_SAVE;
		break;
	case 'x':
		/* Wait for special sequence "xyzzy" */
		wtimeout(win_board, 1000);

		c = wgetch(win_board);
		if (c == 'y') {
			c = wgetch(win_board);
			if (c == 'z') {
				c = wgetch(win_board);
				if (c == 'z') {
					c = wgetch(win_board);
					if (c == 'y')
						action	= PLAYER_IFACE_ACT_XYZZY_ON;
				}
			}
		}

		/* Resume */
		wtimeout(win_board, REFRESH_TIME_MS);
		break;
	case '0':
		action	= PLAYER_IFACE_ACT_XYZZY_OFF;
		break;
	case '1':
		action	= PLAYER_IFACE_ACT_XYZZY_LIN;
		break;
	case '2':
		action	= PLAYER_IFACE_ACT_XYZZY_P;
		break;
	case '3':
		action	= PLAYER_IFACE_ACT_XYZZY_NP;
		break;
	case 'q':
		action	= PLAYER_IFACE_ACT_QUIT;
		break;
	case KEY_LEFT:
	case 'h':
		action	= PLAYER_IFACE_ACT_MOVE_LEFT;
		break;
	case KEY_DOWN:
	case 'j':
		action	= PLAYER_IFACE_ACT_MOVE_DOWN;
		break;
	case KEY_UP:
	case 'k':
		action	= PLAYER_IFACE_ACT_MOVE_UP;
		break;
	case KEY_RIGHT:
	case 'l':
		action	= PLAYER_IFACE_ACT_MOVE_RIGHT;
		break;
	case 'c':
		action	= PLAYER_IFACE_ACT_HIGHLIGHT;
		break;
	default:
		action	= PLAYER_IFACE_ACT_FOO;
		break;
	}

	return	action;
}

/*	*	*	*	*	*	*	*	*	*
 *	*	* Help	*	*	*	*	*	*	*
 *	*	*	*	*	*	*	*	*	*/
static	void	show_help		(const struct Game_Iface_Out	*board)
{

	if (last_help == board->state)
		return;

	werase(win_help);

	switch (board->state) {
	case GAME_IFACE_STATE_PLAYING:
		show_help_play();
		break;
	case GAME_IFACE_STATE_PAUSE:
		show_help_pause();
		break;
	case GAME_IFACE_STATE_XYZZY:
		show_help_xyzzy();
		break;
	case GAME_IFACE_STATE_CHEATED:
		show_help_cheat();
		break;
	case GAME_IFACE_STATE_SAFE:
		show_help_safe();
		break;
	case GAME_IFACE_STATE_GAMEOVER:
		show_help_gameover();
		break;
	}

	wrefresh(win_help);
	last_help	= board->state;
}

static	void	show_help_start		(void)
{
	int	r;
	int	c;

	werase(win_help);

	r	= 0;
	c	= 0;
	mvwaddstr(win_help, r++, c++, "Move:");
	/* hjkl */
	mvwaddch(win_help, r, c, 'h');
	c += 2;
	mvwaddch(win_help, r, c, 'j');
	c += 2;
	mvwaddch(win_help, r, c, 'k');
	c += 2;
	mvwaddch(win_help, r, c, 'l');
	/* Arrows */
	r++;
	c = 1;
	mvwaddch(win_help, r, c, ACS_LARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_DARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_UARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_RARROW);

	r++;
	c = 0;
	mvwaddstr(win_help, r++, c, "Cursor:");
	mvwprintw(win_help, r++, c, " %c", 'c');

	mvwaddstr(win_help, r++, c, "Step:");
	mvwprintw(win_help, r++, c, " Enter / %c", '+');

	mvwaddstr(win_help, r++, c, "Quit:");
	mvwprintw(win_help, r++, c, " %c", 'q');

	wrefresh(win_help);
	last_help	= GAME_IFACE_STATE_FOO;
}

static	void	show_help_play		(void)
{
	int	r;
	int	c;

	r	= 0;
	c	= 0;
	mvwaddstr(win_help, r++, c++, "Move:");
	/* hjkl */
	mvwaddch(win_help, r, c, 'h');
	c += 2;
	mvwaddch(win_help, r, c, 'j');
	c += 2;
	mvwaddch(win_help, r, c, 'k');
	c += 2;
	mvwaddch(win_help, r, c, 'l');
	/* Arrows */
	r++;
	c = 1;
	mvwaddch(win_help, r, c, ACS_LARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_DARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_UARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_RARROW);

	r++;
	c = 0;
	mvwaddstr(win_help, r++, c, "Cursor:");
	mvwprintw(win_help, r++, c, " %c", 'c');

	mvwaddstr(win_help, r++, c, "Step:");
	mvwprintw(win_help, r++, c, " Enter / %c", '+');

	mvwaddstr(win_help, r++, c, "Flag:");
	mvwprintw(win_help, r++, c, " Space");

	mvwaddstr(win_help, r++, c, "Possible:");
	mvwprintw(win_help, r++, c, " %c", 'f');

	mvwaddstr(win_help, r++, c, "rm flag:");
	mvwprintw(win_help, r++, c, " Backspace");

	mvwaddstr(win_help, r++, c, "Pause:");
	mvwprintw(win_help, r++, c, " Break / %c", 'p');

	mvwaddstr(win_help, r++, c, "Save:");
	mvwprintw(win_help, r++, c, " %c", 's');

	mvwaddstr(win_help, r++, c, "Quit:");
	mvwprintw(win_help, r++, c, " %c", 'q');
}

static	void	show_help_pause		(void)
{
	int	r;
	int	c;

	r	= 0;
	c	= 0;
	mvwaddstr(win_help, r++, c, "Cursor:");
	mvwprintw(win_help, r++, c, " %c", 'c');

	mvwaddstr(win_help, r++, c, "Continue:");
	mvwprintw(win_help, r++, c, " Break / %c", 'p');

	mvwaddstr(win_help, r++, c, "Save:");
	mvwprintw(win_help, r++, c, " %c", 's');

	mvwaddstr(win_help, r++, c, "Quit:");
	mvwprintw(win_help, r++, c, " %c", 'q');
}

static	void	show_help_xyzzy		(void)
{
	int	r;
	int	c;

	r	= 0;
	c	= 0;
	mvwaddstr(win_help, r++, c++, "XYZZY:");
	mvwprintw(win_help, r, c, "%c", '1');
	c += 2;
	mvwprintw(win_help, r, c, "%c", '2');
//	c += 2;
//	mvwprintw(win_help, r, c, "%c", 'd');

	r++;
	c = 0;
	mvwaddstr(win_help, r++, c, "XYZZY off:");
	mvwprintw(win_help, r++, c, " %c", '0');

	mvwaddstr(win_help, r++, c++, "Move:");
	/* hjkl */
	mvwaddch(win_help, r, c, 'h');
	c += 2;
	mvwaddch(win_help, r, c, 'j');
	c += 2;
	mvwaddch(win_help, r, c, 'k');
	c += 2;
	mvwaddch(win_help, r, c, 'l');
	/* Arrows */
	r++;
	c = 1;
	mvwaddch(win_help, r, c, ACS_LARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_DARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_UARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_RARROW);

	r++;
	c = 0;
	mvwaddstr(win_help, r++, c, "Cursor:");
	mvwprintw(win_help, r++, c, " %c", 'c');

	mvwaddstr(win_help, r++, c, "Step:");
	mvwprintw(win_help, r++, c, " Enter / %c", '+');

	mvwaddstr(win_help, r++, c, "Flag:");
	mvwprintw(win_help, r++, c, " Space");

	mvwaddstr(win_help, r++, c, "Possible:");
	mvwprintw(win_help, r++, c, " %c", 'f');

	mvwaddstr(win_help, r++, c, "rm flag:");
	mvwprintw(win_help, r++, c, " Backspace");

	mvwaddstr(win_help, r++, c, "Save:");
	mvwprintw(win_help, r++, c, " %c", 's');

	mvwaddstr(win_help, r++, c, "Quit:");
	mvwprintw(win_help, r++, c, " %c", 'q');
}

static	void	show_help_cheat		(void)
{
	int	r;
	int	c;

	r	= 0;
	c	= 0;
	mvwaddstr(win_help, r++, c++, "Move:");
	/* hjkl */
	mvwaddch(win_help, r, c, 'h');
	c += 2;
	mvwaddch(win_help, r, c, 'j');
	c += 2;
	mvwaddch(win_help, r, c, 'k');
	c += 2;
	mvwaddch(win_help, r, c, 'l');
	/* Arrows */
	r++;
	c = 1;
	mvwaddch(win_help, r, c, ACS_LARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_DARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_UARROW);
	c += 2;
	mvwaddch(win_help, r, c, ACS_RARROW);

	r++;
	c = 0;
	mvwaddstr(win_help, r++, c, "Cursor:");
	mvwprintw(win_help, r++, c, " %c", 'c');

	mvwaddstr(win_help, r++, c, "Step:");
	mvwprintw(win_help, r++, c, " Enter / %c", '+');

	mvwaddstr(win_help, r++, c, "Flag:");
	mvwprintw(win_help, r++, c, " Space");

	mvwaddstr(win_help, r++, c, "Possible:");
	mvwprintw(win_help, r++, c, " %c", 'f');

	mvwaddstr(win_help, r++, c, "rm flag:");
	mvwprintw(win_help, r++, c, " Backspace");

	mvwaddstr(win_help, r++, c, "Save:");
	mvwprintw(win_help, r++, c, " %c", 's');

	mvwaddstr(win_help, r++, c, "Quit:");
	mvwprintw(win_help, r++, c, " %c", 'q');
}

static	void	show_help_safe		(void)
{
	int	r;
	int	c;

	r	= 0;
	c	= 0;
	mvwaddstr(win_help, r++, c, "Save:");
	mvwprintw(win_help, r++, c, " %c", 's');

	mvwaddstr(win_help, r++, c, "Quit:");
	mvwprintw(win_help, r++, c, " %c", 'q');
}

static	void	show_help_gameover	(void)
{
	int	r;
	int	c;

	r	= 0;
	c	= 0;
	mvwaddstr(win_help, r++, c, "Quit:");
	mvwprintw(win_help, r++, c, " %c", 'q');
}


/******************************************************************************
 ******* end of file **********************************************************
 ******************************************************************************/
