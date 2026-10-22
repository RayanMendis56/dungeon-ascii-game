#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

int disableBuffer(struct termios *original_mode);
int enableBuffer(const struct termios *original_mode);

#endif /* TERMINAL_H */
