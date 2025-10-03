/**
 * @file menu.c
 * @brief Main menu rendering and selection handling.
 */
#include <gint/display.h>
#include <gint/keyboard.h>
#include <string.h>
#include "menu.h"

/** Menu items displayed on the home screen. */
static MenuItem MENU_ITEMS[] = {
    {"Plus"},
    {"Jouer"},
    {"Quitter"},
};
/** Number of menu items. */
static const int MENU_COUNT = 3;

/**
 * @brief Draw one frame of the menu (title, icons, labels).
 */
static void menu_draw(void);

// Draw one frame of the menu
static void menu_draw()
{
	dclear(C_WHITE);

	// Letter T
	drect(42, 1, 44, 13, C_BLACK);
	drect(37, 1, 49, 3, C_BLACK);
	// Letter D
	drect(51, 1, 53, 13, C_BLACK);
	drect(53, 1, 57, 3, C_BLACK);
	drect(58, 4, 60, 10, C_BLACK);
	drect(53, 11, 57, 13, C_BLACK);
	// Character '
	drect(62, 1, 64, 3, C_BLACK);
	drect(65, 1, 67, 6, C_BLACK);
	drect(62, 9, 64, 7, C_BLACK);
	// Letter P
	drect(69, 1, 71, 13, C_BLACK);
	drect(71, 1, 76, 3, C_BLACK);
	drect(71, 7, 76, 9, C_BLACK);
	drect(77, 4, 79, 6, C_BLACK);
	// Letter C
	drect(81, 4, 83, 10, C_BLACK);
	drect(84, 1, 90, 3, C_BLACK);
	drect(84, 11, 90, 13, C_BLACK);

	// Circle layout positions (3 evenly spaced)
	int centers[3] = {20, 64, 108}; // x coordinates
	int y_circle = 29;
	int radius = 10;

	for (int i = 0; i < MENU_COUNT; i++)
	{
		int cx = centers[i];

		// Draw circle
		dcircle(cx, y_circle, radius, C_NONE, C_INVERT);

		if (i == 0)
		{
			// Plus button (left)
			dline(15, 24, 25, 24, C_BLACK);
			dline(15, 29, 25, 29, C_BLACK);
			dline(15, 34, 25, 34, C_BLACK);
		}
		else if (i == 1)
		{
			// Play button (middle)
			dline(60, 24, 60, 33, C_BLACK);
			dline(60, 34, 70, 29, C_BLACK);
			dline(60, 24, 70, 29, C_BLACK);
		}
		else
		{
			// Quit button (right)
			// Frame
			dline(104, 22, 112, 22, C_BLACK);
			dline(112, 22, 112, 34, C_BLACK);
			dline(109, 34, 112, 34, C_BLACK);
			// Door
			dline(109, 24, 109, 36, C_BLACK);
			dline(104, 22, 109, 24, C_BLACK);
			dline(104, 34, 109, 36, C_BLACK);
			dline(104, 22, 104, 34, C_BLACK);
			// Handle
			dpixel(107, 30, C_BLACK);
		}

		// Draw label below
		int label_x = cx - (int)strlen(MENU_ITEMS[i].label) * 3; // approx center
		dtext(label_x, y_circle + radius + 12, C_BLACK, MENU_ITEMS[i].label);
	}

	dhline(45, C_BLACK);

	dupdate();
}

/**
 * @brief Menu loop that waits for a selection key and returns the choice.
 */
int run_menu(void)
{
	menu_draw();

	while (1)
	{
		key_event_t ev = getkey();

		if (ev.type == KEYEV_DOWN)
		{
			switch (ev.key)
			{
			case KEY_F1:
			case KEY_F2:
				return 0; // left item ("Plus")
			case KEY_F3:
			case KEY_F4:
				return 1; // middle ("Jouer")
			case KEY_F5:
			case KEY_F6:
				return 2; // right ("Quitter")
			case KEY_EXIT:
				return -1;
			}
		}
	}
}
