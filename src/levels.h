/**
 * @file levels.h
 * @brief Level selection UI API.
 */
#pragma once
#include "map.h"

/**
 * @brief Number of predefined levels available in the selector.
 */
#define LEVEL_COUNT 3

typedef struct
{
    const uint8_t map[MAP_H][MAP_W];
    const Point *waypoints; /* pointer to a const waypoint array */
    const int waypoint_count;
} LevelDef;

/**
 * @brief Open the level selector UI and update the active level.
 */
void level_selector(void);
