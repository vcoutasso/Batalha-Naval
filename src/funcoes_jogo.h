#ifndef _funcoes_jogo_h
#define _funcoes_jogo_h

#include "jogo.h"

#define NUM_BARCOS 5
#define BORDA_LATERAL 0.23
#define OUTRA_BORDA 0.025
#define VERTICAL 1
#define HORIZONTAL 2
#define TWO_PLAYERS 2
#define SINGLE_PLAYER 1

void nome_quadradinhos(struct tabuleiro **tabuleiro, ALLEGRO_FONT *font, ALLEGRO_COLOR yellow, int tab_size);

float rad(enum direcao dir);

void desenha_tabuleiro(int tam);

void desenha_barquinhos(struct barquinhos *marquinhos, int n, int tab_size);

void qual_quadradinho(float x, float y, int *linha, int *coluna, struct tabuleiro **tabuleiro, int qtd);

void averigua_quadradinho(int *linha, int *coluna, struct quadradinhos **quadradinhos, struct barquinhos marquinhos, int tab_size);

void atualiza_tabuleiro(struct quadradinhos **quadradinhos, struct barquinhos marquinhos, int vo_buta);

void grava_tabuleiro(struct quadradinhos **tabuleiro, int tab_size, char file_name[]);

void limpa_tabuleiro(struct quadradinhos **quadradinhos, int tab_size);

int aleatorio(struct quadradinhos **quadradinhos, struct barquinhos *marquinhos, struct tabuleiro **tabuleiro, int tab_size, int qual_tab);

void posiciona_barquinho(struct barquinhos *barco, struct tabuleiro **tabuleiro, struct quadradinhos **quadradinhos, ALLEGRO_BITMAP **barquinho_bitmap, int linha, int coluna, int tab_size, int numero, char file_name[]);

void gira_barquinho(struct ALLEGRO_BITMAP *bitmap, struct barquinhos *barquinho, int tab_size, int numero, int x, int y);

void pinta_tabuleiro(struct quadradinhos **quadradinhos, struct tabuleiro **tabuleiro, int tab_size, int meu_tabuleiro);

void verifica_se_acertou(struct quadradinhos **tabuleiro, struct barquinhos *barquinhos, struct player *player, int linha, int coluna, int *atirou, int which_player);

void computador_atira(struct quadradinhos **tabuleiro, int tab_size);

void grava_recordes(struct player player, char *tempo);

#endif

