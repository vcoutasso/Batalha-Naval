#ifndef _jogo_h
#define _jogo_h

#include <string.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define	MAX_ITEM_LENGTH 20

enum direcao { vertical = 1, horizontal };

struct tabuleiro {
	float x;
	float y;
	float largura;
	float altura;
};

struct quadradinhos {
	int disponivel;
	int tiro;
	int morreu;
};

struct barquinhos {
	ALLEGRO_BITMAP *bitmap;
	float x;
	float y;
	int tamanho;
	int linha;
	int coluna;
	enum direcao dir;
	float tiros;
};

struct player {
	char nome[5];
	int score;
	int acertos;
	int erros;
};

int o_jogo(int tab_size, int game_mode, ALLEGRO_TIMER *timer, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT event, int check_rand, ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, ALLEGRO_SAMPLE *click);

#endif
