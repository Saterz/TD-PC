/**
 * @file map.h
 * @brief Tile/sprite declarations and map rendering API.
 */
#pragma once
#include <gint/display.h>

extern bopti_image_t const spr_road_bl; /**< Bottom-left road corner sprite. */
extern bopti_image_t const spr_road_br; /**< Bottom-right road corner sprite. */
extern bopti_image_t const spr_road_h;  /**< Horizontal road sprite. */
extern bopti_image_t const spr_road_v;  /**< Vertical road sprite. */
extern bopti_image_t const spr_road_tl; /**< Top-left road corner sprite. */
extern bopti_image_t const spr_road_tr; /**< Top-right road corner sprite. */

extern bopti_image_t const spr_grass; /**< Grass tile sprite. */
extern bopti_image_t const spr_rock;  /**< Rock tile sprite. */
extern bopti_image_t const spr_blank; /**< Blank background tile sprite. */

extern bopti_image_t const spr_spawn; /**< Spawn point sprite. */
extern bopti_image_t const spr_goal;  /**< Goal point sprite. */

extern bopti_image_t const spr_turret_1; /**< Basic turret sprite. */

/** Logical screen width in pixels. */
#define SCREEN_W 128
/** Logical screen height in pixels. */
#define SCREEN_H 64

/** Size of a square map tile in pixels. */
#define TILE_SIZE 9

/** Map width in tiles (14 * 9 = 126 px). */
#define MAP_W 14
/** Map height in tiles (7 * 9 = 63 px). */
#define MAP_H 7

/**
 * @brief Tile identifiers used in the level grid.
 */
typedef enum
{
    T_BLANK = 0, /**< Empty background tile. */
    T_ROAD = 1,  /**< Road tile used by bots. */
    T_SPAWN = 2, /**< Spawn point tile. */
    T_GOAL = 3,  /**< Goal tile (life penalty on reach). */
    T_GRASS = 4, /**< Grass decorative tile. */
    T_ROCK = 5,  /**< Rock decorative tile. */
} Tile;

/**
 * @brief 2D grid coordinate used for waypoints on the map.
 *
 * Represents a tile position as unsigned bytes to match map dimensions.
 */
typedef struct
{
    uint8_t x; /**< Tile X coordinate (0..MAP_W-1). */
    uint8_t y; /**< Tile Y coordinate (0..MAP_H-1). */
} Point;


/**
 * @brief Read-only level layout, expressed as Tile IDs.
 * 
 * Active level tile grid; updated by the level selector.
 */
extern uint8_t level_tiles[MAP_H][MAP_W];

/**
 * @brief Render the map, entities and selector.
 * @param selector_pixel_x Selector X coordinate in pixels.
 * @param selector_pixel_y Selector Y coordinate in pixels.
 */
void render_map(int selector_pixel_x, int selector_pixel_y, int toggle_overlay);
