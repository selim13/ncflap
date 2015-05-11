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
#include <locale.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#else
#include <signal.h>
#endif

#include "curses.h"

short term_w, term_h;
HANDLE stdout_handler;

static bool resized = false;
void terminal_resize()
{
        resized = true;
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32) || defined(__CYGWIN__)
        signal(SIGWINCH, terminal_resize);
#endif
}

void term_init()
{
        setlocale(LC_ALL, "");

        initscr();
        cbreak();
        noecho();
        nonl();
        intrflush(stdscr, false);
        keypad(stdscr, true);
        curs_set(0);
        start_color();

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)       
        CONSOLE_SCREEN_BUFFER_INFO cb_info;

        stdout_handler = GetStdHandle(STD_OUTPUT_HANDLE);
        if (GetConsoleScreenBufferInfo(stdout_handler, &cb_info)) {
                term_w = cb_info.dwSize.X;
                term_h = cb_info.dwSize.Y;
        }
#else
        signal(SIGWINCH, terminal_resize);
#endif;
}

bool term_resized()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        CONSOLE_SCREEN_BUFFER_INFO cb_info;

        if (GetConsoleScreenBufferInfo(stdout_handler, &cb_info)) {
                if (cb_info.dwSize.X != term_w || cb_info.dwSize.Y != term_h) {
                        term_w = cb_info.dwSize.X;
                        term_h = cb_info.dwSize.Y;

                        
                        resized = true;
                }
        }
#endif

        if (resized) {
                resized = false;
                endwin();
                refresh();
                clear();

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
                resize_term(term_h, term_w);
#endif
                
                return true;
        }

        return false;
}