#include <gint/display.h>
#include <gint/keyboard.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

// --- Sprites you provide elsewhere (BOPTI images) ---
extern bopti_image_t const spr_road_bl;
extern bopti_image_t const spr_road_br;
extern bopti_image_t const spr_road_h;
extern bopti_image_t const spr_road_v;
extern bopti_image_t const spr_road_tl;
extern bopti_image_t const spr_road_tr;

extern bopti_image_t const spr_grass;
extern bopti_image_t const spr_rock;
extern bopti_image_t const spr_blank;

extern bopti_image_t const spr_spawn;
extern bopti_image_t const spr_goal;

extern bopti_image_t const spr_turret_1; // a simple tower icon (8x8)

// --- Tile IDs ---
typedef enum
{
    T_BLANK = 0,
    T_ROAD = 1,
    T_SPAWN = 2,
    T_GOAL = 3,
    T_GRASS = 4,
    T_ROCK = 5,
} Tile;

// --- Level data (roads & environment pre-placed) ---
static const uint8_t level_tiles[MAP_H][MAP_W] = {
    /* y=0 */ {4, 2, 1, 1, 1, 1, 1, 1, 1, 0, 4, 0, 5, 4}, // S at (0,1), road to (0,8)
    /* y=1 */ {0, 0, 5, 4, 4, 5, 0, 0, 1, 0, 4, 0, 5, 0}, // road at (1,8) going down
    /* y=2 */ {0, 4, 4, 0, 0, 0, 0, 4, 1, 4, 0, 0, 5, 0}, // road at (2,8)
    /* y=3 */ {0, 4, 0, 1, 1, 1, 1, 1, 1, 0, 5, 5, 0, 0}, // road (3,3)→(3,8)
    /* y=4 */ {0, 0, 0, 1, 0, 4, 4, 5, 0, 4, 0, 0, 5, 0}, // road at (4,3)
    /* y=5 */ {0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4}, // road (5,3)→(5,11)
    /* y=6 */ {4, 4, 4, 0, 5, 0, 0, 0, 0, 0, 0, 1, 1, 3}, // road down to (6,11) → right to G at (6,13)
};

// Optional: Path waypoints for bots (tile coords, not pixels)
// Creeps will follow these in order (convert to pixel centers at runtime).
// static const struct
// {
//     uint8_t x, y;
// } waypoints[] = {
//     {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0}
//     // then turn down to {13,7} if your road continues (adjust to your map)
// };
// static const int N_WP = sizeof(waypoints) / sizeof(waypoints[0]);

// --- Player-placed towers (runtime) ---
static uint8_t towers[MAP_H][MAP_W]; // 0 = none, >0 = tower id (1 = basic)

// --- Helpers ---
static inline int in_bounds(int gx, int gy)
{
    return gx >= 0 && gx < MAP_W && gy >= 0 && gy < MAP_H;
}
static inline int can_build(int gx, int gy)
{
    if (!in_bounds(gx, gy))
        return 0;
    if (level_tiles[gy][gx] != T_BLANK)
        return 0; // forbid on road/block/spawn/goal
    if (towers[gy][gx] != 0)
        return 0; // already occupied
    return 1;
}

// Determine whether a tile at (gx, gy) connects as a road-like segment
static inline int is_roadlike_at(int gx, int gy)
{
    if (!in_bounds(gx, gy))
        return 0;
    uint8_t t = level_tiles[gy][gx];
    return (t == T_ROAD || t == T_SPAWN || t == T_GOAL);
}

// Draw one tile sprite based on tile ID
static void draw_tile(uint8_t t, int gx, int gy, int px, int py)
{
    switch (t)
    {
    case T_BLANK: /* background */
        dimage(px, py, &spr_blank);
        break;
    case T_GRASS:
        dimage(px, py, &spr_grass);
        break;
    case T_ROCK:
        dimage(px, py, &spr_rock);
        break;
    case T_ROAD:
    {
        int up = is_roadlike_at(gx, gy - 1);
        int down = is_roadlike_at(gx, gy + 1);
        int left = is_roadlike_at(gx - 1, gy);
        int right = is_roadlike_at(gx + 1, gy);

        // Choose the best matching sprite
        bopti_image_t const *img = &spr_road_h; // default fallback

        // Straights
        if (left && right && !up && !down)
            img = &spr_road_h;
        else if (up && down && !left && !right)
            img = &spr_road_v;
        // Corners (turns)
        else if (up && right && !down && !left)
            img = &spr_road_tr; // connects Up + Right
        else if (up && left && !down && !right)
            img = &spr_road_tl; // connects Up + Left
        else if (down && right && !up && !left)
            img = &spr_road_br; // connects Down + Right
        else if (down && left && !up && !right)
            img = &spr_road_bl; // connects Down + Left
        // Endpoints fallback (connect to one side only)
        else if (left || right)
            img = &spr_road_h;
        else if (up || down)
            img = &spr_road_v;

        dimage(px, py, img);
    }
    break;
    case T_SPAWN:
        dimage(px, py, &spr_spawn);
        break;
    case T_GOAL:
        dimage(px, py, &spr_goal);
        break;
    }
}

// Draw selector as alternating frame, always grid-aligned
static void draw_selector(int px, int py)
{
    int px2 = px + TILE_SIZE - 1;
    int py2 = py + TILE_SIZE - 1;
    dline(px, py, px2, py, C_BLACK);
    dline(px2, py, px2, py2, C_BLACK);
    dline(px2, py2, px, py2, C_BLACK);
    dline(px, py2, px, py, C_BLACK);
}

// --- Rendering pass order ---
// 1) Tiles (roads/environment pre-placed – from level data)
// 2) Towers (player placed)
// 3) Selector
static void render_map(int sel_px, int sel_py)
{
    dclear(C_WHITE);

    // Tiles
    for (int gy = 0; gy < MAP_H; ++gy)
        for (int gx = 0; gx < MAP_W; ++gx)
        {
            int px = gx * TILE_SIZE;
            int py = gy * TILE_SIZE;
            draw_tile(level_tiles[gy][gx], gx, gy, px, py);
        }

    // Towers
    for (int gy = 0; gy < MAP_H; ++gy)
        for (int gx = 0; gx < MAP_W; ++gx)
        {
            if (towers[gy][gx])
            {
                int px = gx * TILE_SIZE;
                int py = gy * TILE_SIZE;
                dimage(px, py, &spr_turret_1);
            }
        }

    // Selector on top
    draw_selector(sel_px, sel_py);

    dupdate();
}

// --- Main loop ---
// Roads/environment are effectively "placed" before control because we render
// them from read-only level data; the player can only place towers.
int game_loop(void)
{
    srand(time(NULL));

    // Start aligned to grid
    int sel_px = 0;
    int sel_py = 0;

    // (Optional) one-time pre-render, splash, or "Place your towers!" message
    // You still re-render every frame, but conceptually roads/env are already set.

    while (1)
    {
        key_event_t ev = getkey();

        if (ev.type == KEYEV_DOWN)
        {
            switch (ev.key)
            {
            case KEY_RIGHT:
                sel_px += TILE_SIZE;
                break;
            case KEY_LEFT:
                sel_px -= TILE_SIZE;
                break;
            case KEY_UP:
                sel_py -= TILE_SIZE;
                break;
            case KEY_DOWN:
                sel_py += TILE_SIZE;
                break;

            // Place/remove a tower with EXE, but only on buildable tiles
            case KEY_EXE:
            {
                int gx = sel_px / TILE_SIZE;
                int gy = sel_py / TILE_SIZE;
                if (can_build(gx, gy))
                {
                    towers[gy][gx] = 1; // place basic tower
                }
            }
            break;

            // Optional: DEL to remove player tower
            case KEY_DEL:
            {
                int gx = sel_px / TILE_SIZE;
                int gy = sel_py / TILE_SIZE;
                if (in_bounds(gx, gy) && towers[gy][gx])
                {
                    towers[gy][gx] = 0; // remove
                }
            }
            break;

            case KEY_EXIT:
                return 0;
            }
        }

        // Clamp selector inside map pixel bounds
        if (sel_px < 0)
            sel_px = 0;
        if (sel_py < 0)
            sel_py = 0;
        if (sel_px > MAP_W * TILE_SIZE - TILE_SIZE)
            sel_px = MAP_W * TILE_SIZE - TILE_SIZE;
        if (sel_py > MAP_H * TILE_SIZE - TILE_SIZE)
            sel_py = MAP_H * TILE_SIZE - TILE_SIZE;

        // Draw everything
        render_map(sel_px, sel_py);
    }
}
