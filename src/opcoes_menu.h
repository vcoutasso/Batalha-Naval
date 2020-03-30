#ifndef _opcoes_menu_h
#define _opcoes_menu_h

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define	MAX_ITEM_LENGTH 20

struct item_menu {
	float x;
	float y;
	float width;
	float e_x;
	char str[MAX_ITEM_LENGTH];
};

void clique(int **hovering, int **vet_check, int *start, int *voltar, int *tam_tab, ALLEGRO_EVENT event);

int is_hovering(struct item_menu *item, ALLEGRO_EVENT event, int size);

void init_item(struct item_menu *item, float x, float y, char text[MAX_ITEM_LENGTH], ALLEGRO_FONT *font, ALLEGRO_FONT *e_font);

int new_game(ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_SAMPLE *click,
	ALLEGRO_SAMPLE *blop, ALLEGRO_TIMER *display_timer, ALLEGRO_EVENT event, ALLEGRO_COLOR light_blue, ALLEGRO_COLOR yellow, ALLEGRO_COLOR orange);

int how_to(ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, ALLEGRO_FONT *tut_font, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_SAMPLE *click, ALLEGRO_TIMER *display_timer,
	ALLEGRO_EVENT event, ALLEGRO_COLOR light_blue, ALLEGRO_COLOR yellow, ALLEGRO_COLOR orange);

int records(ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_SAMPLE *click, ALLEGRO_TIMER *display_timer,
	ALLEGRO_EVENT event, ALLEGRO_COLOR light_blue, ALLEGRO_COLOR yellow, ALLEGRO_COLOR orange);

#endif