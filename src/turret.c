/**
 * @file turret.c
 * @brief Tower grid and build rules implementation.
 */
#include "turret.h"
#include "map.h"

/** Grid of player-built towers; non-zero means a tower is present. */
uint8_t towers[MAP_H][MAP_W] = {0};

/**
 * @brief Check if a tower can be built at the given grid cell.
 * @param grid_x X coordinate in grid units.
 * @param grid_y Y coordinate in grid units.
 * @return 1 if the cell is empty, inside bounds and on blank tile; 0 otherwise.
 */
int can_build(int grid_x, int grid_y)
{
    if (!in_bounds(grid_x, grid_y))
        return 0;
    if (level_tiles[grid_y][grid_x] != T_BLANK)
        return 0;
    if (towers[grid_y][grid_x] != 0)
        return 0;
    return 1;
}
