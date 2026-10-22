#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

#include "map.h"

/*
 * A map owns one allocation per playable row. Keeping the player separate
 * from these tiles lets movement and undo preserve the underlying terrain.
 */
static void release_rows(Map *map, int allocated_rows)
{
    int row;

    for (row = 0; row < allocated_rows; row++) {
        free(map->cells[row]);
    }
    free(map->cells);
    map->cells = NULL;
    map->rows = 0;
    map->columns = 0;
}

int map_load(Map *map, const char *filename, int *player_row,
             int *player_column, char *error_message, size_t error_size)
{
    FILE *file;
    int row;
    int column;
    int found_player = 0;
    int result = 0;

    map->rows = 0;
    map->columns = 0;
    map->cells = NULL;
    file = fopen(filename, "r");
    if (file == NULL) {
        snprintf(error_message, error_size, "Cannot open map file: %s",
                 filename);
    } else if (fscanf(file, "%d %d", &map->rows, &map->columns) != 2 ||
               map->rows <= 0 || map->columns <= 0) {
        snprintf(error_message, error_size, "Invalid map dimensions.");
    } else {
        map->cells = malloc((size_t) map->rows * sizeof(*map->cells));
        if (map->cells == NULL) {
            snprintf(error_message, error_size,
                     "Unable to allocate memory for the map.");
        } else {
            for (row = 0; row < map->rows; row++) {
                map->cells[row] = NULL;
            }

            for (row = 0; row < map->rows && result == 0; row++) {
                map->cells[row] =
                    malloc((size_t) map->columns * sizeof(*map->cells[row]));
                if (map->cells[row] == NULL) {
                    snprintf(error_message, error_size,
                             "Unable to allocate memory for the map.");
                    result = -1;
                } else {
                    for (column = 0; column < map->columns; column++) {
                        int value;

                        if (fscanf(file, "%d", &value) != 1 ||
                            value < TILE_EMPTY || value > TILE_GOAL) {
                            snprintf(error_message, error_size,
                                     "Invalid tile data in map file.");
                            result = -1;
                        } else {
                            map->cells[row][column] = (Tile) value;
                            if (value == TILE_PLAYER) {
                                if (found_player != 0) {
                                    snprintf(error_message, error_size,
                                             "Map contains more than one player.");
                                    result = -1;
                                } else {
                                    *player_row = row;
                                    *player_column = column;
                                    found_player = 1;
                                    map->cells[row][column] = TILE_EMPTY;
                                }
                            }
                        }
                    }
                }
            }

            if (result == 0 && found_player == 0) {
                snprintf(error_message, error_size,
                         "Map does not contain a player.");
                result = -1;
            }
        }
    }

    if (file != NULL) {
        fclose(file);
    }
    if (result != 0) {
        release_rows(map, map->rows);
        result = 0;
    } else if (map->cells != NULL) {
        result = 1;
    }

    return result;
}

void map_destroy(Map *map)
{
    if (map->cells != NULL) {
        release_rows(map, map->rows);
    }
}

int map_is_solid(const Map *map, int row, int column)
{
    int solid = 0;

    if (row >= map->rows) {
        solid = 1;
    } else if (row >= 0 && column >= 0 && column < map->columns &&
               (map->cells[row][column] == TILE_BLOCK ||
                map->cells[row][column] == TILE_LADDER)) {
        solid = 1;
    }

    return solid;
}
