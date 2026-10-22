#ifndef MAP_H
#define MAP_H

#include <stddef.h>

typedef enum {
    TILE_EMPTY = 0,
    TILE_BLOCK = 1,
    TILE_LADDER = 2,
    TILE_DANGER = 3,
    TILE_PLAYER = 4,
    TILE_GOAL = 5
} Tile;

typedef struct {
    int rows;
    int columns;
    Tile **cells;
} Map;

int map_load(Map *map, const char *filename, int *player_row,
             int *player_column, char *error_message, size_t error_size);
void map_destroy(Map *map);
int map_is_solid(const Map *map, int row, int column);

#endif /* MAP_H */
