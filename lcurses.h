#ifndef MINICURSES_H
#define MINICURSES_H

#include <stdio.h>

#define ANSI_RESET "\x1B[0m"
#define ANSI_FG_BLACK "\x1B[30m"
#define ANSI_FG_RED "\x1B[31m"
#define ANSI_FG_GREEN "\x1B[32m"
#define ANSI_FG_YELLOW "\x1B[33m"
#define ANSI_FG_BLUE "\x1B[34m"
#define ANSI_FG_PURPLE "\x1B[35m"
#define ANSI_FG_CYAN "\x1B[36m"
#define ANSI_FG_WHITE "\x1B[37m"

#define ANSI_BG_BLACK "\x1B[40m"
#define ANSI_BG_RED "\x1B[41m"
#define ANSI_BG_GREEN "\x1B[42m"
#define ANSI_BG_YELLOW "\x1B[43m"
#define ANSI_BG_BLUE "\x1B[44m"
#define ANSI_BG_PURPLE "\x1B[45m"
#define ANSI_BG_CYAN "\x1B[46m"
#define ANSI_BG_WHITE "\x1B[47m"

#define ANSI_CLEAR_SCRN "\x1B[2J"
#define ANSI_MOVE_UP "\x1B[%dA"
#define ANSI_MOVE_DOWN "\x1B[%dB"
#define ANSI_MOVE_FORWARD "\x1B[%dC"
#define ANSI_MOVE_BACK "\x1B[%dD"
#define ANSI_CLEAR_LINE "\x1B[2K"
#define ANSI_BG_BLACK "\x1B[40m"
#define ANSI_SAVE_CURSOR "\x1B[s"
#define ANSI_RESTORE_CURSOR "\x1B[u"
#define ANSI_CLEAR_BELOW "\x1B[J"
#define ANSI_HIDE_CURSOR "\x1B[?25l"
#define ANSI_SHOW_CURSOR "\x1B[?25h"

static inline int lcurses_move_cursor(int row, int col) {
    return printf("\x1B[%d;%dH", row, col);
}

static inline int lcurses_move_cursor_up(int rows) {
    return printf(ANSI_MOVE_UP, rows);
}

static inline int lcurses_move_cursor_down(int rows) {
    return printf(ANSI_MOVE_DOWN, rows);
}

static inline int lcurses_move_cursor_forward(int cols) {
    return printf(ANSI_MOVE_FORWARD, cols);
}

static inline int lcurses_move_cursor_back(int cols) {
    return printf(ANSI_MOVE_BACK, cols);
}

static inline int lcurses_clear_line() {
    return printf(ANSI_CLEAR_LINE);
}

static inline int lcurses_hide_cursor() {
    return printf(ANSI_HIDE_CURSOR);
}

static inline int lcurses_show_cursor() {
    return printf(ANSI_SHOW_CURSOR);
}

static inline int lcurses_clear_all() {
    return printf(ANSI_CLEAR_SCRN);
}

static inline int lcurses_font_black() {
    return printf(ANSI_FG_BLACK);
}

static inline int lcurses_font_red() {
    return printf(ANSI_FG_RED);
}

static inline int lcurses_font_green() {
    return printf(ANSI_FG_GREEN);
}

static inline int lcurses_font_yellow() {
    return printf(ANSI_FG_YELLOW);
}

static inline int lcurses_font_blue() {
    return printf(ANSI_FG_BLUE);
}

static inline int lcurses_font_purple() {
    return printf(ANSI_FG_PURPLE);
}

static inline int lcurses_font_cyan() {
    return printf(ANSI_FG_CYAN);
}

static inline int lcurses_font_white() {
    return printf(ANSI_FG_WHITE);
}

static inline int lcurses_background_black() {
    return printf(ANSI_BG_BLACK);
}

static inline int lcurses_background_red() {
    return printf(ANSI_BG_RED);
}

static inline int lcurses_background_green() {
    return printf(ANSI_BG_GREEN);
}

static inline int lcurses_background_yellow() {
    return printf(ANSI_BG_YELLOW);
}

static inline int lcurses_background_blue() {
    return printf(ANSI_BG_BLUE);
}

static inline int lcurses_background_purple() {
    return printf(ANSI_BG_PURPLE);
}

static inline int lcurses_background_cyan() {
    return printf(ANSI_BG_CYAN);
}

static inline int lcurses_background_white() {
    return printf(ANSI_BG_WHITE);
}

static inline int lcurses_save_cursor_position() {
    return printf(ANSI_SAVE_CURSOR);
}

static inline int lcurses_restore_cursor_position() {
    return printf(ANSI_RESTORE_CURSOR);
}

static inline int lcurses_clear_below() {
    return printf(ANSI_CLEAR_BELOW);
}

static inline int lcurses_reset_attr() {
    return printf(ANSI_RESET);
}

#undef ANSI_RESET
#undef ANSI_FG_BLACK
#undef ANSI_FG_RED
#undef ANSI_FG_GREEN
#undef ANSI_FG_YELLOW
#undef ANSI_FG_BLUE
#undef ANSI_FG_PURPLE
#undef ANSI_FG_CYAN
#undef ANSI_FG_WHITE

#undef ANSI_BG_BLACK
#undef ANSI_BG_RED
#undef ANSI_BG_GREEN
#undef ANSI_BG_YELLOW
#undef ANSI_BG_BLUE
#undef ANSI_BG_PURPLE
#undef ANSI_BG_CYAN
#undef ANSI_BG_WHITE

#undef ANSI_CLEAR_SCRN
#undef ANSI_MOVE_UP
#undef ANSI_MOVE_DOWN
#undef ANSI_MOVE_FORWARD
#undef ANSI_MOVE_BACK
#undef ANSI_CLEAR_LINE
#undef ANSI_BG_BLACK
#undef ANSI_SAVE_CURSOR
#undef ANSI_RESTORE_CURSOR
#undef ANSI_CLEAR_BELOW
#undef ANSI_HIDE_CURSOR
#undef ANSI_SHOW_CURSOR

#endif
