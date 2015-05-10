/*
 * Copyright (c) 2015, Seleznyov Dmitry (Selim13) <selim013@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>
#include <time.h>
#include "curses.h"

#include "win.h"

#define NUM_OF_TUBES 10
#define TUBES_WIDTH 8
#define TUBES_GAP 20
#define TUBES_DISTANCE 30

int ticks, x;
float y, velocity;
char bird = '@';
WINDOW *main_win, *end_win;
int win_w, win_h;

struct tube {
        int x;
        bool top;
        int width;
        int height;
} tubes[NUM_OF_TUBES];

static bool resized = false;
void terminal_resize()
{
        resized = true;
        signal(SIGWINCH, terminal_resize);
}

void draw_tube(int x, bool top, int width, int height)
{
        int i, j, start;

        if (x + width <= 1)
                return;
        if (x >= win_w - 1)
                return;

        if (top)
                start = 1;
        else
                start = win_h - 1 - height;

        if ((x + width) >= (win_w - 1)) {
                width = width - (width - (win_w - 1 - x));
        }

        if (x < 1) {
                width = width - (1 - x);
                x = x + (1 - x);
        }

        for (i = 0; i < height; i++) {
                wmove(main_win, start + i, x);

                j = width;
                while (j >0) {
                        waddch(main_win, '#');
                        j--;
                }
        }
}

void next_tube(struct tube *next, struct tube *prev)
{
        next->top = !prev->top;

        if (next->top)
                next->x = prev->x + TUBES_DISTANCE;
        else
                next->x = prev->x;

        if (next->top)
                next->height = (win_h / 4) + (rand() % (win_h / 2));
        else
                next->height = win_h - prev->height - TUBES_GAP;

        next->width = TUBES_WIDTH;
}

void generate_tubes()
{
        int i;

        tubes[0].x = 40;
        tubes[0].top = true;
        tubes[0].width = TUBES_WIDTH;
        tubes[0].height = 5;

        for (i = 1; i < NUM_OF_TUBES; i++) {
                next_tube(&tubes[i], &tubes[i - 1]);
        }
}

void init_game()
{
        ticks = 0;
        x = 10;
        y = 10.0;
        velocity = 0.1;

        generate_tubes();
}

void end_game()
{
        nodelay(main_win, false);
        end_win = subwin(main_win, 5, 29, win_h / 2 - 2, win_w / 2 - 14);
        box(end_win, ACS_VLINE, ACS_HLINE);

        mvwprintw(end_win, 2, 2, "You are dead, repeat? Y/n");
        wrefresh(end_win);
}

int main(void)
{
        int keycode;
        int i;
        struct timespec ts;

        setlocale(LC_ALL, "");
	signal(SIGWINCH, terminal_resize);
        srand(time(NULL));

	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, false);
	keypad(stdscr, true);
	curs_set(0);
	start_color();

        main_win = win_create(LINES - 2, COLS - 2, 1, 1);
        getmaxyx(main_win, win_h, win_w);

        ts.tv_sec = 0;
        ts.tv_nsec = 50 * 1000 * 1000; // tick time

        init_game();
        nodelay(main_win, true);


        while (1) {
                keycode = wgetch(main_win);
                win_clear(main_win);

                if (keycode == ERR) {
                        doupdate();
                } else if (keycode == ' ') {
                        velocity = -1.0;
                } else if (keycode == 'y') {
                        init_game();

                        if (end_win != NULL)
                                delwin(end_win);

                        nodelay(main_win, true);
                } else if (keycode == 'n' || keycode == 'x') {
                        break;
                }

                // handle window resize
                if (resized) {
                        endwin();
                        refresh();
                        clear();

                        win_resize(main_win, LINES - 2, COLS - 2);
                        getmaxyx(main_win, win_h, win_w);

                        resized = false;
                }

                // checks
                if (y <= 2 || y >= win_h) {
                        end_game();
                }

                // draw tubes
                for (i = 0; i < NUM_OF_TUBES; i++) {
                        if (x >= tubes[i].x && x <= (tubes[i].x + tubes[i].width)) {
                                if ((tubes[i].top && y <= tubes[i].height + 1)
                                    || (!tubes[i].top && y >= win_h - 1 - tubes[i].height))
                                        end_game();
                        }

                        if (tubes[i].x + tubes[i].width < 2) {
                                if (i == 0)
                                        next_tube(&tubes[0], &tubes[NUM_OF_TUBES - 1]);
                                else
                                        next_tube(&tubes[i], &tubes[i - 1]);
                        }

                        if (tubes[i].x < win_w - 1)
                               draw_tube(tubes[i].x, tubes[i].top, tubes[i].width, tubes[i].height);

                        if (ticks % 2)
                                tubes[i].x--;
                }


                // draw bird
                mvwaddch(main_win, (int)y, x, bird);
                y = y + velocity;
                velocity += 0.1;

                // other
                mvwprintw(main_win, win_h - 1, win_w - 15, "%d", ticks++);

                wrefresh(main_win);
                nanosleep(&ts, NULL);
        }

        delwin(main_win);
        if (end_win != NULL)
                delwin(end_win);

        endwin();
        return 0;
}
