#include <stdbool.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>
#include <time.h>
#include <ncurses.h>


#include "win.h"

#define NUM_OF_TUBES 10
#define TUBES_WIDTH 8
#define TUBES_GAP 20

int ticks, x;
float y, velocity;
char bird = '@';
WINDOW *main_win, *end_win;

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

        if (x + width < 1)
                return;
        if ((x + width) > (COLS - 2))
                return;

        if (top)
                start = 1;
        else
                start = LINES - 3 - height;

        if ((x + width) >= (COLS - 1)) {
                width = width - (COLS - 7 - x);
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

        if (!prev->top)
                next->x = prev->x + (6 + rand() % 40);
        else
                next->x = prev->x;

        if (prev->top)
                next->height = (LINES / 4) + (rand() % (LINES / 2));
        else
                next->height = LINES - prev->height;

        //if ((prev->x + prev->width + 15 > next->x)
        //    && (prev->height + next->height >= LINES))
        //        next->height = LINES - prev->height - 15;

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
        end_win = subwin(main_win, 5, 29, LINES / 2 - 2, COLS / 2 - 14);
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

        ts.tv_sec = 0;
        ts.tv_nsec = 50 * 1000 * 1000;

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

                        resized = false;
                }

                // checks
                if (y <= 2 || y >= LINES) {
                        end_game();
                }


                // draw tubes
                for (i = 0; i < NUM_OF_TUBES; i++) {
                        if (x >= tubes[i].x && x <= (tubes[i].x + tubes[i].width)) {
                                if ((tubes[i].top && y <= tubes[i].height)
                                    || (!tubes[i].top && y >= LINES - 3 - tubes[i].height))
                                        end_game();
                        }

                        if (tubes[i].x + tubes[i].width < 2) {
                                if (i == 0)
                                        next_tube(&tubes[i], &tubes[NUM_OF_TUBES - 1]);
                                else if (i == NUM_OF_TUBES - 1)
                                        next_tube(&tubes[i], &tubes[0]);
                                else
                                        next_tube(&tubes[i], &tubes[i - 1]);
                        }

                        if (tubes[i].x < (COLS - 2))
                               draw_tube(tubes[i].x, tubes[i].top, tubes[i].width, tubes[i].height);

                        if (ticks % 2)
                                tubes[i].x--;
                }



                // draw bird
                mvwaddch (main_win, (int)y, x, bird);
                //x++;
                y = y + velocity;
                velocity += 0.1;


                // other
                mvwprintw (main_win, 1, COLS - 15, "%d", ticks++);

                wrefresh(main_win);
                nanosleep(&ts, NULL);
        }

        delwin(main_win);
        if (end_win != NULL)
                delwin(end_win);

        endwin();
        return 0;
}
