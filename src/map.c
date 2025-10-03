/**
 * @file map.c
 * @brief Map rendering and tile helpers.
 *
 * Renders the static level tiles, towers, bots, and selector in order.
 */
#include "map.h"
#include "game.h"
#include "turret.h"

/**
 * @brief Read-only level tile grid.
 *
 * Each entry is a Tile ID defined in map.h. This layout drives the
 * environment and the bot path.
 */
const uint8_t level_tiles[MAP_H][MAP_W] = {
    {2, 1, 1, 1, 1, 1, 0, 4, 0, 5, 4, 0, 5, 4},
    {0, 5, 4, 0, 4, 1, 0, 0, 5, 0, 4, 0, 5, 0},
    {0, 4, 0, 5, 0, 1, 0, 4, 0, 0, 5, 0, 4, 0},
    {0, 4, 5, 0, 4, 1, 0, 0, 0, 5, 0, 4, 0, 0},
    {4, 0, 0, 5, 0, 1, 1, 1, 1, 1, 1, 0, 5, 4},
    {0, 0, 4, 0, 5, 0, 0, 4, 0, 0, 1, 0, 4, 5},
    {4, 5, 0, 4, 0, 0, 5, 0, 0, 0, 1, 1, 1, 3},
};

/**
 * @brief Check if a tile connects to the road network.
 * @param grid_x X coordinate in grid units.
 * @param grid_y Y coordinate in grid units.
 * @return 1 if the tile is a road-like tile (road/spawn/goal), 0 otherwise.
 */
static inline int is_roadlike_at(int grid_x, int grid_y)
{
    if (!in_bounds(grid_x, grid_y))
        return 0;
    uint8_t t = level_tiles[grid_y][grid_x];
    return (t == T_ROAD || t == T_SPAWN || t == T_GOAL);
}

/**
 * @brief Draw a single tile at the given grid/pixel position.
 * @param tile_id Tile identifier to draw.
 * @param grid_x  X coordinate in grid units (used for road connection logic).
 * @param grid_y  Y coordinate in grid units (used for road connection logic).
 * @param pixel_x X coordinate in pixels (top-left of tile).
 * @param pixel_y Y coordinate in pixels (top-left of tile).
 */
static void draw_tile(uint8_t tile_id, int grid_x, int grid_y, int pixel_x, int pixel_y)
{
    switch (tile_id)
    {
    case T_BLANK: /* background */
        dimage(pixel_x, pixel_y, &spr_blank);
        break;
    case T_GRASS:
        dimage(pixel_x, pixel_y, &spr_grass);
        break;
    case T_ROCK:
        dimage(pixel_x, pixel_y, &spr_rock);
        break;
    case T_ROAD:
    {
        /** Pick a road sprite variant based on neighbors. */
        int up = is_roadlike_at(grid_x, grid_y - 1);
        int down = is_roadlike_at(grid_x, grid_y + 1);
        int left = is_roadlike_at(grid_x - 1, grid_y);
        int right = is_roadlike_at(grid_x + 1, grid_y);

        bopti_image_t const *img = &spr_road_h;

        if (left && right && !up && !down)
            img = &spr_road_h;
        else if (up && down && !left && !right)
            img = &spr_road_v;

        else if (up && right && !down && !left)
            img = &spr_road_tr;
        else if (up && left && !down && !right)
            img = &spr_road_tl;
        else if (down && right && !up && !left)
            img = &spr_road_br;
        else if (down && left && !up && !right)
            img = &spr_road_bl;

        else if (left || right)
            img = &spr_road_h;
        else if (up || down)
            img = &spr_road_v;

        dimage(pixel_x, pixel_y, img);
    }
    break;
    case T_SPAWN:
        dimage(pixel_x, pixel_y, &spr_spawn);
        break;
    case T_GOAL:
        dimage(pixel_x, pixel_y, &spr_goal);
        break;
    }
}

/**
 * @brief Draw the selector rectangle aligned to the tile grid.
 * @param pixel_x X pixel coordinate of top-left corner.
 * @param pixel_y Y pixel coordinate of top-left corner.
 */
static void draw_selector(int pixel_x, int pixel_y)
{
    int pixel_x2 = pixel_x + TILE_SIZE - 1;
    int pixel_y2 = pixel_y + TILE_SIZE - 1;
    dline(pixel_x, pixel_y, pixel_x2, pixel_y, C_BLACK);
    dline(pixel_x2, pixel_y, pixel_x2, pixel_y2, C_BLACK);
    dline(pixel_x2, pixel_y2, pixel_x, pixel_y2, C_BLACK);
    dline(pixel_x, pixel_y2, pixel_x, pixel_y, C_BLACK);
}

/**
 * @brief Render the entire map frame.
 *
 * Draws background tiles, towers, bots, then the selection rectangle and
 * updates the display buffer.
 *
 * @param selector_pixel_x Selector X coordinate in pixels (top-left of tile).
 * @param selector_pixel_y Selector Y coordinate in pixels (top-left of tile).
 */
void render_map(int selector_pixel_x, int selector_pixel_y)
{
    dclear(C_WHITE);

    // Draw base terrain tiles first
    for (int grid_y = 0; grid_y < MAP_H; ++grid_y)
        for (int grid_x = 0; grid_x < MAP_W; ++grid_x)
        {
            int pixel_x = grid_x * TILE_SIZE;
            int pixel_y = grid_y * TILE_SIZE;
            draw_tile(level_tiles[grid_y][grid_x], grid_x, grid_y, pixel_x, pixel_y);
        }

    // Then draw player-built towers
    for (int grid_y = 0; grid_y < MAP_H; ++grid_y)
        for (int grid_x = 0; grid_x < MAP_W; ++grid_x)
        {
            if (towers[grid_y][grid_x])
            {
                int pixel_x = grid_x * TILE_SIZE;
                int pixel_y = grid_y * TILE_SIZE;
                dimage(pixel_x, pixel_y, &spr_turret_1);
            }
        }

    // Then active bots
    render_bots();

    draw_selector(selector_pixel_x, selector_pixel_y);

    dupdate();
}
