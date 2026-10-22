#include <stdio.h>

#include "color.h"
#include "display.h"

/* ANSI styles are applied per cell so adjacent map objects stay distinct. */
static void print_border_cell(void)
{
    setForeground("red");
    printf("*");
    setForeground("reset");
}

static void print_tile(Tile tile)
{
    printf("\033[0m");
    if (tile == TILE_BLOCK) {
        setBackground("white");
        printf(" ");
    } else if (tile == TILE_DANGER) {
        setBackground("red");
        printf(" ");
    } else if (tile == TILE_LADDER) {
        setForeground("cyan");
        printf("H");
    } else if (tile == TILE_PLAYER) {
        setBackground("blue");
        setForeground("white");
        printf("P");
    } else if (tile == TILE_GOAL) {
        setBackground("green");
        setForeground("black");
        printf("G");
    } else {
        printf(" ");
    }
    printf("\033[0m");
}

void display_render(const Game *game)
{
    int row;
    int column;

    printf("\033[2J\033[H");
    for (column = 0; column < game->map->columns + 2; column++) {
        print_border_cell();
    }
    putchar('\n');

    for (row = 0; row < game->map->rows; row++) {
        print_border_cell();
        for (column = 0; column < game->map->columns; column++) {
            if (row == game->player.row &&
                column == game->player.column) {
                print_tile(TILE_PLAYER);
            } else {
                print_tile(game->map->cells[row][column]);
            }
        }
        print_border_cell();
        putchar('\n');
    }

    for (column = 0; column < game->map->columns + 2; column++) {
        print_border_cell();
    }
    printf("\033[0m\n");
    puts("Press w to move UP");
    puts("Press s to move DOWN");
    puts("Press a to move LEFT");
    puts("Press d to move RIGHT");
    puts("Press u to UNDO");
    if (game->status == GAME_WON) {
        puts("You win!");
    } else if (game->status == GAME_LOST) {
        puts("You lose!");
    }
    fflush(stdout);
}
