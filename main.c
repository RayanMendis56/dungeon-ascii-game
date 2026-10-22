#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>

#include "display.h"
#include "game.h"
#include "map.h"
#include "terminal.h"

/*
 * The entry point owns startup and cleanup; map loading, rendering, terminal
 * control, and gameplay rules are handled by separate modules.
 */

/*
 * The entry point owns startup and cleanup; map loading, rendering, terminal
 * control, and gameplay rules are kept in their respective modules.
 */
int main(int argc, char *argv[])
{
    Map map;
    Game game;
    int player_row = 0;
    int player_column = 0;
    int terminal_mode_enabled = 0;
    int result = 0;
    char error_message[256];
    int key;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <map_file>\n", argv[0]);
        result = 1;
    } else if (!map_load(&map, argv[1], &player_row, &player_column,
                         error_message, sizeof(error_message))) {
        fprintf(stderr, "%s\n", error_message);
        result = 1;
    } else {
        game_init(&game, &map, player_row, player_column);
        display_render(&game);

        if (isatty(STDIN_FILENO)) {
            if (!disableBuffer(&game.terminal_state)) {
                fprintf(stderr, "Unable to configure terminal input.\n");
                result = 1;
                game.status = GAME_ERROR;
            } else {
                terminal_mode_enabled = 1;
            }
        }

        while (result == 0 && game.status == GAME_ACTIVE &&
               (key = getchar()) != EOF) {
            if (!game_handle_input(&game, (char) key,
                                   error_message, sizeof(error_message))) {
                fprintf(stderr, "%s\n", error_message);
                result = 1;
                game.status = GAME_ERROR;
            }
            display_render(&game);
        }

        if (terminal_mode_enabled && !enableBuffer(&game.terminal_state)) {
            fprintf(stderr, "Unable to restore terminal input settings.\n");
            result = 1;
        }

        game_destroy(&game);
        map_destroy(&map);
    }

    return result;
}
