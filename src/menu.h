/**
 * @file menu.h
 * @brief Menu item type and menu API.
 */
typedef struct
{
    const char *label; /**< Display label for the menu item. */
} MenuItem;

/**
 * @brief Run the main menu loop and return the selected action.
 * @return 0 for Plus, 1 for Jouer, 2 for Quitter, -1 on exit.
 */
int run_menu(void);
