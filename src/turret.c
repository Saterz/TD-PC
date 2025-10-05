/**
 * @file turret.c
 * @brief Tower grid, placement rules, and targeting logic.
 */
#include <stdbool.h>
#include "turret.h"
#include "map.h"

/** Grid of player-built towers; non-zero means a tower is present. */
uint8_t towers[MAP_H][MAP_W] = {0};

/** Maximum number of towers supported simultaneously. */
#define MAX_TOWERS (MAP_W * MAP_H)

/** Storage for active towers. */
static Tower g_turrets[MAX_TOWERS];
/** Current number of active towers. */
static int g_turret_count = 0;

/** Default tower archetype. */
static const TowerType TOWER_BASIC = {
    .name = "Basic",
    .range_px = 3 * TILE_SIZE,
    .fire_cooldown_ticks = 20,
    .damage = 1,
    .sprite = &spr_turret_1,
};

/**
 * @brief Convert a pixel position to tile coordinates.
 * @param px Pixel coordinate to convert.
 * @return Tile index obtained by integer division.
 */
static inline int px_to_tile(int px)
{
    return px / TILE_SIZE;
}

/**
 * @brief Clamp an integer between inclusive bounds.
 * @param v Value to clamp.
 * @param a Minimum bound.
 * @param b Maximum bound.
 * @return Clamped value.
 */
static inline int clampi(int v, int a, int b)
{
    if (v < a)
        return a;
    if (v > b)
        return b;
    return v;
}

/**
 * @brief Square an integer, avoiding floating-point operations.
 * @param v Integer to square.
 * @return v squared.
 */
static inline int sqi(int v)
{
    return v * v;
}

/**
 * @brief Determine whether a tile blocks line of sight.
 * @param tx Tile X coordinate.
 * @param ty Tile Y coordinate.
 * @return true if the tile is blocking or out-of-bounds.
 */
bool is_blocking_tile(int tx, int ty)
{
    if (!in_bounds(tx, ty))
        return true;
    uint8_t tile = level_tiles[ty][tx];
    return (tile == T_ROCK);
}

/**
 * @brief Bresenham-based line-of-sight test over tile coordinates.
 * @param ax Start tile X.
 * @param ay Start tile Y.
 * @param bx End tile X.
 * @param by End tile Y.
 * @return true when the segment is unobstructed.
 */
static bool line_of_sight_tiles(int ax, int ay, int bx, int by)
{
    int dx = (bx > ax) ? (bx - ax) : (ax - bx);
    int dy = (by > ay) ? (by - ay) : (ay - by);
    int sx = (ax < bx) ? 1 : -1;
    int sy = (ay < by) ? 1 : -1;
    int err = dx - dy;
    int x = ax;
    int y = ay;

    for (;;)
    {
        if (is_blocking_tile(x, y))
            return false;
        if (x == bx && y == by)
            break;
        int e2 = err << 1;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
    return true;
}

/**
 * @brief Check whether a bot lies within a tower's detection radius.
 * @param tx_px Tower center X in pixels.
 * @param ty_px Tower center Y in pixels.
 * @param bx_px Bot center X in pixels.
 * @param by_px Bot center Y in pixels.
 * @param range_tiles Tower range expressed in tiles.
 * @return true when the bot is inside the circular range.
 */
static bool in_range_px(int tx_px, int ty_px, int bx_px, int by_px, int range_tiles)
{
    int range_px = range_tiles * TILE_SIZE;
    int dx = bx_px - tx_px;
    int dy = by_px - ty_px;
    return (sqi(dx) + sqi(dy)) <= sqi(range_px);
}

/**
 * @brief Locate the index of a tower at specific grid coordinates.
 * @param grid_x X coordinate in grid units.
 * @param grid_y Y coordinate in grid units.
 * @return Index in the tower array, or -1 if not found.
 */
static int find_tower_index(int grid_x, int grid_y)
{
    for (int i = 0; i < g_turret_count; ++i)
        if (g_turrets[i].x == grid_x && g_turrets[i].y == grid_y)
            return i;
    return -1;
}

/**
 * @brief Fire at a bot.
 * @param tower Tower performing the attack.
 * @param bot Target bot.
 */
static void fire_at(Tower *tower, Bot *bot)
{
    if (!tower || !bot)
        return;

    bot_apply_damage(bot, tower->type->damage);
}

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

/**
 * @brief Attempt to place a tower and register it for updates.
 * @param grid_x X coordinate in grid units.
 * @param grid_y Y coordinate in grid units.
 * @return 1 on success, 0 otherwise.
 */
int build_turret(int grid_x, int grid_y)
{
    if (!can_build(grid_x, grid_y))
        return 0;
    if (g_turret_count >= MAX_TOWERS)
        return 0;

    Tower *tower = &g_turrets[g_turret_count++];
    tower->type = &TOWER_BASIC;
    tower->x = grid_x;
    tower->y = grid_y;
    tower->center_x_px = grid_x * TILE_SIZE + TILE_SIZE / 2;
    tower->center_y_px = grid_y * TILE_SIZE + TILE_SIZE / 2;
    tower->range_tiles = (tower->type->range_px + TILE_SIZE - 1) / TILE_SIZE;
    tower->cooldown = 0;

    towers[grid_y][grid_x] = 1;
    return 1;
}

/**
 * @brief Remove an existing tower from the grid.
 * @param grid_x X coordinate in grid units.
 * @param grid_y Y coordinate in grid units.
 */
void remove_turret(int grid_x, int grid_y)
{
    int index = find_tower_index(grid_x, grid_y);
    if (index < 0)
        return;

    towers[grid_y][grid_x] = 0;
    g_turrets[index] = g_turrets[g_turret_count - 1];
    g_turret_count--;
}

/**
 * @brief Update a single tower, choosing a target and firing if possible.
 * @param tower Tower instance to update.
 */
void update_turret(Tower *tower)
{
    if (!tower || !tower->type)
        return;

    if (tower->cooldown > 0)
    {
        tower->cooldown--;
        return;
    }

    int turret_tile_x = tower->x;
    int turret_tile_y = tower->y;
    int radius = tower->range_tiles;
    int x0 = clampi(turret_tile_x - radius, 0, MAP_W - 1);
    int x1 = clampi(turret_tile_x + radius, 0, MAP_W - 1);
    int y0 = clampi(turret_tile_y - radius, 0, MAP_H - 1);
    int y1 = clampi(turret_tile_y + radius, 0, MAP_H - 1);

    Bot *best_bot = NULL;
    int best_dist2 = 0;

    for (int ty = y0; ty <= y1; ++ty)
        for (int tx = x0; tx <= x1; ++tx)
            for (Bot *bot = bot_first_in_tile(tx, ty); bot; bot = bot->next_in_tile)
            {
                if (!bot->active)
                    continue;
                int bot_center_x = bot->pixel_x + TILE_SIZE / 2;
                int bot_center_y = bot->pixel_y + TILE_SIZE / 2;
                if (!in_range_px(tower->center_x_px, tower->center_y_px,
                                 bot_center_x, bot_center_y, tower->range_tiles))
                    continue;
                if (!line_of_sight_tiles(turret_tile_x, turret_tile_y,
                                          bot->tile_x, bot->tile_y))
                    continue;
                int dist2 = sqi(bot_center_x - tower->center_x_px) +
                            sqi(bot_center_y - tower->center_y_px);
                if (!best_bot || dist2 < best_dist2)
                {
                    best_bot = bot;
                    best_dist2 = dist2;
                }
            }

    if (best_bot)
    {
        fire_at(tower, best_bot);
        tower->cooldown = tower->type->fire_cooldown_ticks;
    }
}

/**
 * @brief Update every active tower for the current frame.
 */
void update_all_turrets(void)
{
    for (int i = 0; i < g_turret_count; ++i)
        update_turret(&g_turrets[i]);
}
