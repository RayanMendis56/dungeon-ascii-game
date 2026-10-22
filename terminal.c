#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "terminal.h"

/* Preserve the complete terminal mode so normal input is restored afterward. */
int disableBuffer(struct termios *original_mode)
{
    struct termios mode;
    int success = 0;

    if (tcgetattr(STDIN_FILENO, original_mode) == 0) {
        mode = *original_mode;
        mode.c_lflag &= (tcflag_t) ~(ECHO | ICANON);
        mode.c_cc[VMIN] = 1;
        mode.c_cc[VTIME] = 0;
        success = tcsetattr(STDIN_FILENO, TCSANOW, &mode) == 0;
    }

    return success;
}

int enableBuffer(const struct termios *original_mode)
{
    return tcsetattr(STDIN_FILENO, TCSANOW, original_mode) == 0;
}
