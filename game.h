#ifndef GAME_H
#define GAME_H

#include <stddef.h>
#include <termios.h>

#include "linkedList.h"
#include "map.h"

typedef enum {
    GAME_ACTIVE,
    GAME_WON,
    GAME_LOST,
    GAME_ERROR
} GameStatus;

typedef struct {
    int row;
    int column;
} Position;

typedef struct {
    Map *map;
    Position player;
    LinkedList undo_history;
    GameStatus status;
    struct termios terminal_state;
} Game;

void game_init(Game *game, Map *map, int player_row, int player_column);
int game_handle_input(Game *game, char key, char *error_message,
                      size_t error_size);
void game_destroy(Game *game);

#endif /* GAME_H */
