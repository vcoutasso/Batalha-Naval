#ifdef _WIN32
#define WINDOWS 1
#endif

#ifdef linux
#define LINUX 1
#endif

#ifndef  _menu_principal_h
#define _menu_principal_h

#include <stdio.h>

#include "opcoes_menu.h"

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define NORMAL_FSIZE 64
#define HOVER_FSIZE 74
#define GROW_FSIZE 100
#define TUT_FSIZE 40
#define PI 3.14159265358979323846

void invoke_main_menu();

int check_pointers(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, ALLEGRO_FONT *tut_font,
	ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_SAMPLE *click, ALLEGRO_TIMER *display_timer);

int check_items(struct item_menu *item1, struct item_menu *item2, struct item_menu *item3, struct item_menu *item4);

void print_item(int is_hovering, ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, struct item_menu *item, int marcado);

#endif