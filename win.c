#include <ncursesw/ncurses.h>

WINDOW *win_create(int height, int width, int y, int x)
{
	WINDOW *win = newwin(height, width, y, x);
	box(win, ACS_VLINE, ACS_HLINE);
	wrefresh(win);

	return win;
}

void win_resize(WINDOW *win, int height, int width)
{
	wresize(win, height, width);
	wclear(win);
	box(win, ACS_VLINE, ACS_HLINE);
	wrefresh(win);
}

void win_change(WINDOW *win, int height, int width, int y, int x)
{
	wresize(win, height, width);
	mvwin(win, y, x);
	wclear(win);
	box(win, ACS_VLINE, ACS_HLINE);
	wrefresh(win);
}

void win_clear(WINDOW *win)
{
        werase(win);
	box(win, ACS_VLINE, ACS_HLINE);
	wrefresh(win);
}
