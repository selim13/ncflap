#include <ncurses.h>

#include "win.h"

void win_drawborder(WINDOW *win)
{
        box(win, ACS_VLINE, ACS_HLINE);

#ifdef WIN_DEBUG
        int x, y, yy, i;
        getmaxyx(win, y, x);

        mvwprintw(win, y - 1, x / 2, "%d", x);

        i = 0;
        yy = y;
        while (yy > 0) {
                mvwprintw(win, y / 2 - i, 0, "%d", yy % 10 );
                yy /= 10;
                i++;
        }

#endif
}

WINDOW *win_create(int height, int width, int y, int x)
{
	WINDOW *win = newwin(height, width, y, x);
        win_drawborder(win);
	wrefresh(win);

	return win;
}

void win_resize(WINDOW *win, int height, int width)
{
	wresize(win, height, width);
	wclear(win);
        win_drawborder(win);
	wrefresh(win);
}

void win_change(WINDOW *win, int height, int width, int y, int x)
{
	wresize(win, height, width);
	mvwin(win, y, x);
	wclear(win);
        win_drawborder(win);
	wrefresh(win);
}

void win_clear(WINDOW *win)
{
        werase(win);
        win_drawborder(win);
	wrefresh(win);
}
