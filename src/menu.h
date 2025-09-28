typedef struct
{
    const char *label;
} MenuItem;

static MenuItem MENU_ITEMS[] = {
    {"Plus"},
    {"Jouer"},
    {"Quitter"},
};
static const int MENU_COUNT = 3;

// Draw one frame of the menu
static void menu_draw(void);

// Menu loop
int run_menu(void);
