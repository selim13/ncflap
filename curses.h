#ifndef __CURSES_H_
#define __CURSES_H_

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <curses.h>
#else
#include <ncurses.h>
#endif

#endif