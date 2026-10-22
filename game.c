#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

#include "display.h"
#include "game.h"
#include "newSleep.h"

#define FALL_FRAME_SECONDS 0.15f
#define FATAL_FALL_ROWS 5

/*
 * The list records accepted moves only. Falling is animated synchronously,
 * so undo always returns to the stable position before the fall began.
 */
static void free_position(void *data)
{
    free(data);
}

static int player_is_on_ladder(const Game *game)
{
    return game->map->cells[game->player.row][game->player.column] ==
           TILE_LADDER;
}

static int player_has_support(const Game *game)
{
    int supported = 0;

    if (player_is_on_ladder(game) ||
        map_is_solid(game->map, game->player.row + 1,
                     game->player.column)) {
        supported = 1;
    }

    return supported;
}

static int save_undo_position(Game *game, Position position,
                              char *error_message, size_t error_size)
{
    Position *saved_position = malloc(sizeof(*saved_position));
    int success = 0;

    if (saved_position == NULL) {
        snprintf(error_message, error_size,
                 "Unable to allocate memory for undo history.");
    } else {
        *saved_position = position;
        if (list_push_front(&game->undo_history, saved_position)) {
            success = 1;
        } else {
            free(saved_position);
            snprintf(error_message, error_size,
                     "Unable to allocate memory for undo history.");
        }
    }

    return success;
}

static int is_inside_map(const Game *game, int row, int column)
{
    return row >= 0 && row < game->map->rows &&
           column >= 0 && column < game->map->columns;
}

static int can_enter(const Game *game, int row, int column)
{
    return is_inside_map(game, row, column) &&
           game->map->cells[row][column] != TILE_BLOCK;
}

static void resolve_landing(Game *game, int fall_start_row)
{
    int fall_distance = game->player.row - fall_start_row;
    Tile landing_tile =
        game->map->cells[game->player.row][game->player.column];

    if (landing_tile == TILE_GOAL) {
        game->status = GAME_WON;
    } else if (landing_tile == TILE_DANGER ||
               fall_distance >= FATAL_FALL_ROWS) {
        game->status = GAME_LOST;
    }
}

static void animate_fall(Game *game)
{
    int fall_start_row = game->player.row;
    int falling = 1;

    while (falling != 0 && game->status == GAME_ACTIVE) {
        game->player.row++;
        display_render(game);
        newSleep(FALL_FRAME_SECONDS);

        if (game->map->cells[game->player.row][game->player.column] ==
            TILE_DANGER) {
            game->status = GAME_LOST;
            falling = 0;
        } else if (player_has_support(game)) {
            resolve_landing(game, fall_start_row);
            falling = 0;
        }
    }
}

static int try_move(Game *game, int row_delta, int column_delta,
                    char *error_message, size_t error_size)
{
    int next_row = game->player.row + row_delta;
    int next_column = game->player.column + column_delta;
    Position old_position = game->player;
    int success = 1;

    if (can_enter(game, next_row, next_column)) {
        if (!save_undo_position(game, old_position, error_message,
                                error_size)) {
            success = 0;
        } else {
            game->player.row = next_row;
            game->player.column = next_column;

            if (game->map->cells[next_row][next_column] == TILE_DANGER) {
                game->status = GAME_LOST;
            } else if (game->map->cells[next_row][next_column] == TILE_GOAL) {
                game->status = GAME_WON;
            } else if (!player_has_support(game)) {
                animate_fall(game);
            }
        }
    }

    return success;
}

static int move_player(Game *game, char key, char *error_message,
                       size_t error_size)
{
    int success = 1;
    int vertical_move_allowed =
        player_is_on_ladder(game) ||
        (key == 's' &&
         game->player.row + 1 < game->map->rows &&
         game->map->cells[game->player.row + 1][game->player.column] ==
             TILE_LADDER);

    if (key == 'a' || key == 'd') {
        if (player_has_support(game)) {
            success = try_move(game, 0, key == 'a' ? -1 : 1,
                               error_message, error_size);
        }
    } else if ((key == 'w' || key == 's') && vertical_move_allowed) {
        success = try_move(game, key == 'w' ? -1 : 1, 0,
                           error_message, error_size);
    }

    return success;
}

void game_init(Game *game, Map *map, int player_row, int player_column)
{
    game->map = map;
    game->player.row = player_row;
    game->player.column = player_column;
    game->status = GAME_ACTIVE;
    list_init(&game->undo_history);
}

int game_handle_input(Game *game, char key, char *error_message,
                      size_t error_size)
{
    int success = 1;

    if (game->status == GAME_ACTIVE) {
        if (key == 'u') {
            Position *previous_position = list_pop_front(&game->undo_history);

            if (previous_position != NULL) {
                game->player = *previous_position;
                free(previous_position);
            }
        } else if (key == 'w' || key == 's' || key == 'a' || key == 'd') {
            success = move_player(game, key, error_message, error_size);
        }
    }

    return success;
}

void game_destroy(Game *game)
{
    list_destroy(&game->undo_history, free_position);
}
