#ifndef __WIN_H_
#define __WIN_H_

#include <ncurses.h>

#define WIN_DEBUG 1

WINDOW *win_create(int height, int width, int y, int x);
void win_resize(WINDOW *win, int height, int width);
void win_change(WINDOW *win, int height, int width, int y, int x);
void win_clear(WINDOW *win);

#endif
