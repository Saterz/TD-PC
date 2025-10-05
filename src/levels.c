/**
 * @file levels.c
 * @brief Level selection UI and level data sets.
 */

#include <gint/display.h>
#include <gint/keyboard.h>
#include <stdint.h>
#include "game.h"
#include "levels.h"
#include "bot.h"


/**
 * @brief Active path waypoints and count used by bots.
 *
 * Defined here so the level selector can update them; other modules include
 * `bot.h` to access these externs. Initialized to an empty path by default.
 */
const Point *waypoints = NULL; /**< Pointer to current level waypoint array. */
int WAYPOINT_COUNT = 0;        /**< Number of waypoints in the current path. */


static const Point wp0[] = {
    {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {5, 1}, {5, 2}, {5, 3}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {10, 5}, {10, 6}, {11, 6}, {12, 6}, {13, 6}};

static const Point wp1[] = {
    {0, 0},
    {1, 0},
    {2, 0},
    {3, 0},
    {4, 0},
    {5, 0},
    {6, 0},
    {7, 0},
    {8, 0},
    {9, 0},
    {10, 0},
    {11, 0},
    {12, 0},
    {12, 1},
    {12, 2},
    {12, 3},
    {12, 4},
    {12, 5},
    {12, 6},
    {13, 6}};

static const Point wp2[] = {
    {0, 0},
    {1, 0},
    {1, 1},
    {1, 2},
    {2, 2},
    {3, 2},
    {4, 2},
    {5, 2},
    {6, 2},
    {6, 3},
    {6, 4},
    {6, 5},
    {6, 6},
    {7, 6},
    {8, 6},
    {9, 6},
    {10, 6},
    {11, 6},
    {12, 6},
    {13, 6}};

/**
 * @brief Read-only definitions for the different levels.
 *
 * Each entry mirrors the size of the runtime `level_tiles` grid and is copied
 * into it upon selection.
 */
static const LevelDef g_levels[LEVEL_COUNT] = {
    {
        /* Level 0 */
        .map = {
            {2, 1, 1, 1, 1, 1, 0, 4, 0, 5, 4, 0, 5, 4},
            {0, 5, 4, 0, 4, 1, 0, 0, 5, 0, 4, 0, 5, 0},
            {0, 4, 0, 5, 0, 1, 0, 4, 0, 0, 5, 0, 4, 0},
            {0, 4, 5, 0, 4, 1, 0, 0, 0, 5, 0, 4, 0, 0},
            {4, 0, 0, 5, 0, 1, 1, 1, 1, 1, 1, 0, 5, 4},
            {0, 0, 4, 0, 5, 0, 0, 4, 0, 0, 1, 0, 4, 5},
            {4, 5, 0, 4, 0, 0, 5, 0, 0, 0, 1, 1, 1, 3},
        },
        .waypoints = wp0,
        .waypoint_count = (uint8_t)(sizeof wp0 / sizeof wp0[0]),
    },
    {
        /* Level 1 */
        .map = {
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4},
            {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4},
            {4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 4},
            {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 4},
            {4, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 4},
            {4, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 4},
            {4, 5, 4, 4, 4, 4, 4, 0, 1, 1, 1, 1, 1, 3},
        },
        .waypoints = wp1,
        .waypoint_count = (uint8_t)(sizeof wp1 / sizeof wp1[0]),
    },
    {
        /* Level 2 */
        .map = {
            {2, 1, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
            {0, 1, 0, 0, 0, 5, 0, 0, 4, 0, 0, 5, 0, 4},
            {0, 1, 1, 1, 1, 1, 1, 0, 4, 0, 1, 1, 0, 4},
            {0, 0, 0, 0, 0, 0, 1, 0, 4, 0, 0, 1, 0, 4},
            {4, 5, 4, 4, 4, 0, 1, 0, 4, 0, 0, 1, 0, 4},
            {4, 0, 0, 0, 4, 0, 1, 0, 4, 5, 0, 1, 0, 4},
            {4, 4, 4, 0, 4, 0, 1, 1, 1, 1, 1, 1, 1, 3},
        },
        .waypoints = wp2,
        .waypoint_count = (uint8_t)(sizeof wp2 / sizeof wp2[0]),
    },
};

/**
 * @brief Labels shown in the selector for each level.
 */
static const char *g_level_labels[LEVEL_COUNT] = {
    "Silicium",
    "Flux",
    "Nébula",
};

/**
 * @brief Copy a level layout into the active `level_tiles` buffer.
 * @param index Index of the level in the `g_levels` array.
 */
static void apply_level(int index)
{
    for (int y = 0; y < MAP_H; ++y)
        for (int x = 0; x < MAP_W; ++x)
            level_tiles[y][x] = g_levels[index].map[y][x];
    
    waypoints = g_levels[index].waypoints;
    WAYPOINT_COUNT = g_levels[index].waypoint_count;
}

/**
 * @brief Draw one frame of the level selector UI.
 * @param selected Index currently highlighted.
 */
static void draw_selector_ui(int selected)
{
    dclear(C_WHITE);

    dtext(34, 6, C_BLACK, "Choisir niveau");

    int y0 = 20;
    for (int i = 0; i < LEVEL_COUNT; ++i)
    {
        int y = y0 + i * 12;
        if (i == selected)
            dtext(20, y, C_BLACK, ">");
        dtext(28, y, C_BLACK, g_level_labels[i]);
    }

    dupdate();
}

/**
 * @brief Show a small UI to select a level and apply it.
 *
 * - Navigate with arrows or F1/F6; confirm with EXE; EXIT cancels and keeps the
 *   current level unchanged.
 */
void level_selector(void)
{
    int selected = 0;
    draw_selector_ui(selected);

    while (1)
    {
        key_event_t ev = getkey();
        if (ev.type != KEYEV_DOWN)
            continue;

        switch (ev.key)
        {
        case KEY_UP:
            if (selected > 0)
                selected--;
            draw_selector_ui(selected);
            break;
        case KEY_DOWN:
            if (selected < LEVEL_COUNT - 1)
                selected++;
            draw_selector_ui(selected);
            break;
        case KEY_EXE:
            apply_level(selected);
            return;
        case KEY_EXIT:
            return;
        }
    }
}
