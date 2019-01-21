#include "menu_principal.h"
#include "funcoes_jogo.h"
#include "jogo.h"

// Calcula e retorna o angulo em radianos dependendo da dire��o do barco.
float rad(enum direcao dir) {
	if (dir == horizontal)
		return 90.0 * PI / 180.0;

	return 0;
}

// Fun��o para desenhar o tabuleiro na tela.
void desenha_tabuleiro(int tam) {
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
	int i;
	int width = SCREEN_WIDTH;
	int height = SCREEN_HEIGHT;

	al_clear_to_color(al_map_rgb(255, 255, 255));

	// Desenha o frame externo.
	for (i = 0; i <= tam; i++) {
		// Desenha linhas horizontais.
		al_draw_line(width * BORDA_LATERAL + (i * width * (1 - 2 * BORDA_LATERAL) / tam), height * OUTRA_BORDA, width * BORDA_LATERAL + (i * width * (1 - 2 * BORDA_LATERAL) / tam), height * (1 - OUTRA_BORDA), black, 2);
		al_draw_line(width * BORDA_LATERAL, height * OUTRA_BORDA + (i * height * (1 - 2 * OUTRA_BORDA) / tam), width * (1 - BORDA_LATERAL), height * OUTRA_BORDA + (i * height * (1 - 2 * OUTRA_BORDA) / tam), black, 2);
	}
}

void nome_quadradinhos(struct tabuleiro **tabuleiro, ALLEGRO_FONT *font, ALLEGRO_COLOR yellow, int tab_size) {
	int i;
	char letra[2] = "A";
	char numero[3] = "1";
	int trocou = 0;
	
	for (i = 0; i < tab_size; i++) {
		al_draw_text(font, yellow, tabuleiro[0][i].x + tabuleiro[0][i].largura / 2 - al_get_text_width(font, letra) / 2, 2, 0, letra);
		letra[0]++;
	}

	for (i = 0; i < tab_size; i++) {
		al_draw_text(font, yellow, tabuleiro[0][tab_size-1].x + tabuleiro[0][0].largura + 10, tabuleiro[i][0].y + tabuleiro[i][0].altura / 2 - al_get_font_line_height(font) / 2, 0, numero);
		if (numero[0] < '9' && !trocou)
			numero[0]++;
		else {
			if (!trocou) {
				strcpy(numero, "10");
				trocou = 1;
			}
			else
				numero[1]++;
		}

	}

} 

// Desenha os barquinhos na tela quando n for maior que 0. *barquinhos é um ponteiro alocado dinamicamente 
// conforme a quantidade de barcos que devem ser impressos (barcos que ja foram posicionados).
void desenha_barquinhos(struct barquinhos *marquinhos, int n, int tab_size) {
	int i;
	float bitmap_width;
	float bitmap_height;

	if (n >= NUM_BARCOS + 1)
		n = NUM_BARCOS;

	for (i = 0; i < n; i++) {
		bitmap_width = al_get_bitmap_width(marquinhos[i].bitmap);
		bitmap_height = al_get_bitmap_height(marquinhos[i].bitmap);

		al_draw_rotated_bitmap(marquinhos[i].bitmap, bitmap_width / 2, bitmap_height / 2, marquinhos[i].x, marquinhos[i].y, rad(marquinhos[i].dir), 0);
	}

}

// Verifica em qual quadradinho o barco deve ser impresso. Desta forma o barco não será simplesmente posicionado onde o
// cursor estava no momento do clique, mas no quadradinho que melhor corresponde à essa posição.
// Se a posição for invalida o quadradinho não é impresso (atribui -1 para linha e/ou coluna).
void qual_quadradinho(const float x, const float y, int *linha, int *coluna, struct tabuleiro **tabuleiro, int qtd) {
	int i = 0, j;

	for (j = 0; j < qtd; j++) {
		if (j == qtd - 1) {
			if (x > tabuleiro[i][j].x && x < SCREEN_WIDTH * (1 - BORDA_LATERAL))
				*coluna = j;
		}
		else {
			if (x > tabuleiro[i][j].x && x < tabuleiro[i][j + 1].x) {
				*coluna = j;
				break;
			}
			else
				*coluna = -1;
		}
	}

	j = 0;

	for (i = 0; i < qtd; i++) {
		if (i == qtd - 1) {
			if (y > tabuleiro[i][j].y && y < SCREEN_HEIGHT * (1 - OUTRA_BORDA))
				*linha = i;
		}
		else {
			if (y > tabuleiro[i][j].y && y < tabuleiro[i + 1][j].y) {
				*linha = i;
				break;
			}

			*linha = -1;
		}
	}

}


// Verifica se a posição é válida (se está disponível ou dentro dos limites do tabuleiro).
void averigua_quadradinho(int *linha, int *coluna, struct quadradinhos **quadradinhos, struct barquinhos marquinhos, int tab_size) {
	const int tamanho_barco = marquinhos.tamanho;
	int invalido = 0;
	int i, j;

	if (*linha != -1 && *coluna != -1) {
		if (marquinhos.dir == vertical) {
			if (tamanho_barco % 2 == 0) {
				if (*linha + tamanho_barco / 2 >= tab_size || *linha - tamanho_barco / 2 < -1)
					*linha = -1;
			}
			else {
				if (*linha + (tamanho_barco - 1) / 2 >= tab_size || *linha - (tamanho_barco - 1) / 2 < 0)
					*linha = -1;
			}
		}
		else if (marquinhos.dir == horizontal) {
			if (tamanho_barco % 2 == 0) {
				if (*coluna + tamanho_barco / 2 > tab_size || *coluna - tamanho_barco / 2 < 0)
					*coluna = -1;
			}
			else {
				if (*coluna + (tamanho_barco - 1) / 2 >= tab_size || *coluna - (tamanho_barco - 1) / 2 < 0)
					*coluna = -1;
			}
		}
	}

	int linha_comeco, linha_fim;
	int coluna_comeco, coluna_fim;

	if (*linha != -1 && *coluna != -1) {
		if (marquinhos.dir == vertical) {
			coluna_comeco = *coluna;
			coluna_fim = *coluna;

			if (marquinhos.tamanho == 2) {
				linha_comeco = *linha;
				linha_fim = *linha + 1;
			}
			else if (marquinhos.tamanho != 5) {
				linha_comeco = *linha - 1;
				if (marquinhos.tamanho == 3)
					linha_fim = *linha + 1;
				else
					linha_fim = *linha + 2;
			}
			else {
				linha_comeco = *linha - 2;
				linha_fim = *linha + 2;
			}

			for (i = linha_comeco; i <= linha_fim; i++) {
				for (j = coluna_comeco - 1; j <= coluna_fim + 1; j++) {
					if (i >= 0 && j >= 0 && i < tab_size && j < tab_size) {
						if (quadradinhos[i][j].disponivel == 0)
							invalido = 1;
						if (j == coluna_comeco) {
							if (i - 1 >= 0) {
								if (quadradinhos[i - 1][j].disponivel == 0)
									invalido = 1;
							}
							if (i + 1 < tab_size) {
								if (quadradinhos[i + 1][j].disponivel == 0)
									invalido = 1;
							}
						}
					}
				}
			}
		}


		else if (marquinhos.dir == horizontal) {
			linha_comeco = *linha;
			linha_fim = *linha;

			if (marquinhos.tamanho == 2) {
				coluna_comeco = *coluna - 1;
				coluna_fim = *coluna;
			}
			else if (marquinhos.tamanho == 3) {
				coluna_comeco = *coluna - 1;
				coluna_fim = *coluna + 1;
			}
			else {
				coluna_comeco = *coluna - 2;
				if (marquinhos.tamanho == 4)
					coluna_fim = *coluna + 1;
				else
					coluna_fim = *coluna + 2;
			}

			for (i = linha_comeco - 1; i <= linha_comeco + 1; i++) {
				for (j = coluna_comeco; j <= coluna_fim; j++) {
					if (i >= 0 && j >= 0 && i < tab_size && j < tab_size) {
						if (quadradinhos[i][j].disponivel == 0)
							invalido = 1;
						if (i == linha_comeco) {
							if (j - 1 >= 0) {
								if (quadradinhos[i][j - 1].disponivel == 0)
									invalido = 1;
							}
							if (j + 1 < tab_size) {
								if (quadradinhos[i][j + 1].disponivel == 0)
									invalido = 1;
							}
						}
					}
				}
			}

		}
	}

	if (invalido) {
		*linha = -1;
		*coluna = -1;
	}

}


// Grava as posições que estão sendo ocupadas na variavel do tipo struct quadradinhos.
void atualiza_tabuleiro(struct quadradinhos **quadradinhos, struct barquinhos marquinhos, int vo_buta) {
	const int linha = marquinhos.linha;
	const int coluna = marquinhos.coluna;
	int x;

	// Verifica se é pra liberar ou ocupar a posiçao do tabuleiro.
	if (vo_buta)
		x = 0;
	else
		x = 1;

	quadradinhos[linha][coluna].disponivel = x;

	if (marquinhos.dir == horizontal) {
		quadradinhos[linha][coluna - 1].disponivel = x;

		if (marquinhos.tamanho > 2)
			quadradinhos[linha][coluna + 1].disponivel = x;

		if (marquinhos.tamanho > 3)
			quadradinhos[linha][coluna - 2].disponivel = x;

		if (marquinhos.tamanho > 4)
			quadradinhos[linha][coluna + 2].disponivel = x;
	}
	else if (marquinhos.dir == vertical) {
		quadradinhos[linha + 1][coluna].disponivel = x;

		if (marquinhos.tamanho > 2)
			quadradinhos[linha - 1][coluna].disponivel = x;

		if (marquinhos.tamanho > 3)
			quadradinhos[linha + 2][coluna].disponivel = x;

		if (marquinhos.tamanho > 4)
			quadradinhos[linha - 2][coluna].disponivel = x;

	}
}

// Grava, em um arquivo de texto, a representação do tabuleiro.
// - representa um espaço vazio no tabuleiro
// # representa um espaço ocupado por uma parte de um barco
void grava_tabuleiro(struct quadradinhos **tabuleiro, int tab_size, char file_name[]) {
	int c = 'A';
	int x = 1;
	int i, j;


	FILE *fp = fopen(file_name, "w+");

	for (i = 0; i < tab_size; i++)
		fprintf(fp, "%c ", c + i);

	fprintf(fp, "\n");

	for (i = 0; i < tab_size; i++) {
		for (j = 0; j < tab_size; j++) {
			if (tabuleiro[i][j].disponivel == 1)
				fprintf(fp, "- ");
			else
				fprintf(fp, "# ");
		}
		fprintf(fp, "%d\n", x + i);
	}

	fclose(fp);
}

// Libera todas as posições do tabuleiro.
void limpa_tabuleiro(struct quadradinhos **quadradinhos, int tab_size) {
	int i, j;

	for (i = 0; i < tab_size; i++) {
		for (j = 0; j < tab_size; j++)
			quadradinhos[i][j].disponivel = 1;
	}
}


// Escolhe aleatoriamente a posição dos barquinhos.
int aleatorio(struct quadradinhos **quadradinhos, struct barquinhos *marquinhos, struct tabuleiro **tabuleiro, int tab_size, int qual_tab) {
	int linha, coluna;
	int numero = 0;
	int deu_ruim = 0;
	char file_name[15];

	if (qual_tab == 1)
		strcpy(file_name, "tabuleiro1.txt");
	else
		strcpy(file_name, "tabuleiro2.txt");


	while (numero < 5) {
		int tentativas = 1;
		while (tentativas <= 10) {
			linha = rand() % tab_size;
			coluna = rand() % tab_size;

			marquinhos[numero].linha = linha;
			marquinhos[numero].coluna = coluna;

			// Escolhe aleatoriamente a direçao do barco.
			if (rand() % 2 == 0)
				marquinhos[numero].dir = vertical;
			else
				marquinhos[numero].dir = horizontal;

			averigua_quadradinho(&linha, &coluna, quadradinhos, marquinhos[numero], tab_size);

			if (linha == -1 || coluna == -1) {
				if (marquinhos[numero].dir == vertical)
					marquinhos[numero].dir = horizontal;
				else
					marquinhos[numero].dir = vertical;


				averigua_quadradinho(&linha, &coluna, quadradinhos, marquinhos[numero], tab_size);
			}

			if (linha == -1 || coluna == -1) {
				tentativas++;
			}
			else
				break;
		}

		if (linha == -1 || coluna == -1) {
			deu_ruim = 1;
			break;
		}
		// Se a posição for válida ajusta as coordenadas em que o barco será impresso.
		else {
			if (marquinhos[numero].dir == vertical) {
				marquinhos[numero].x = tabuleiro[linha][coluna].x + tabuleiro[linha][coluna].largura / 2;
				if (marquinhos[numero].tamanho % 2 == 0)
					marquinhos[numero].y = tabuleiro[linha][coluna].y + tabuleiro[linha][coluna].altura;
				else
					marquinhos[numero].y = tabuleiro[linha][coluna].y + tabuleiro[linha][coluna].altura / 2;
			}
			else {
				marquinhos[numero].y = tabuleiro[linha][coluna].y + tabuleiro[linha][coluna].altura / 2;
				if (marquinhos[numero].tamanho % 2 == 0)
					marquinhos[numero].x = tabuleiro[linha][coluna].x;
				else
					marquinhos[numero].x = tabuleiro[linha][coluna].x + tabuleiro[linha][coluna].largura / 2;
			}
		}


		atualiza_tabuleiro(quadradinhos, marquinhos[numero], 1);
		grava_tabuleiro(quadradinhos, tab_size, file_name);
		numero++;
	}

	if (deu_ruim) {
		limpa_tabuleiro(quadradinhos, tab_size);

		aleatorio(quadradinhos, marquinhos, tabuleiro, tab_size, qual_tab);

	}
	
	return 1;
}

void posiciona_barquinho(struct barquinhos *barco, struct tabuleiro **tabuleiro, struct quadradinhos **quadradinhos, ALLEGRO_BITMAP **barquinho_bitmap, int linha, int coluna, int tab_size, int numero, char file_name[]) {

	barco[numero - 1].linha = linha;
	barco[numero - 1].coluna = coluna;

	atualiza_tabuleiro(quadradinhos, barco[numero - 1], 1);

	barco[numero].dir = barco[numero - 1].dir;

	barco[numero - 1].bitmap = barquinho_bitmap[numero - 1];

	// Se estiver na vertical
	if (barco[numero - 1].dir == vertical) {
		barco[numero - 1].x = tabuleiro[linha][coluna].x + tabuleiro[linha][coluna].largura / 2;
		if (barco[numero - 1].tamanho % 2 == 0)
			barco[numero - 1].y = tabuleiro[linha][coluna].y + tabuleiro[linha][coluna].altura;
		else
			barco[numero - 1].y = tabuleiro[linha][coluna].y + tabuleiro[linha][coluna].altura / 2;
	}
	// Se estiver na horizontal
	else {
		barco[numero - 1].y = tabuleiro[linha][coluna].y + tabuleiro[linha][coluna].altura / 2;
		if (barco[numero - 1].tamanho % 2 == 0)
			barco[numero - 1].x = tabuleiro[linha][coluna].x;
		else
			barco[numero - 1].x = tabuleiro[linha][coluna].x + tabuleiro[linha][coluna].largura / 2;
	}

	grava_tabuleiro(quadradinhos, tab_size, file_name);
}

void gira_barquinho(ALLEGRO_BITMAP *bitmap, struct barquinhos *barquinho, int tab_size, int numero, int x, int y) {

	desenha_tabuleiro(tab_size);

	if (barquinho[numero].dir == vertical)
		barquinho[numero].dir = horizontal;
	else
		barquinho[numero].dir = vertical;

	al_draw_rotated_bitmap(bitmap, al_get_bitmap_width(bitmap) / 2, al_get_bitmap_height(bitmap) / 2, x + al_get_bitmap_width(bitmap) / 2, y + al_get_bitmap_height(bitmap) / 2, rad(barquinho[numero].dir), 0);
}

void pinta_tabuleiro(struct quadradinhos **quadradinhos, struct tabuleiro **tabuleiro, int tab_size, int meu_tabuleiro) {
	int i, j;
	int radius;

	ALLEGRO_COLOR red = al_map_rgb(255, 0, 0), green = al_map_rgb(0, 255, 0), blue = al_map_rgb(0, 0, 255), gray = al_map_rgb(192, 192, 192);

	if (!meu_tabuleiro) {
		if (tab_size == 8)
			radius = 30;
		else if (tab_size == 10)
			radius = 25;
		else
			radius = 20;
	}
	else {
		if (tab_size == 8)
			radius = 25;
		else if (tab_size == 10)
			radius = 20;
		else
			radius = 15;
	}

	for (i = 0; i < tab_size; i++) {
		for (j = 0; j < tab_size; j++) {
			if (quadradinhos[i][j].tiro == 1) {
				if (!meu_tabuleiro) {
					if (quadradinhos[i][j].disponivel == 1)
						al_draw_filled_circle(tabuleiro[i][j].x + tabuleiro[i][j].largura / 2, tabuleiro[i][j].y + tabuleiro[i][j].altura / 2, radius, red);
					else if (quadradinhos[i][j].disponivel == 0 && quadradinhos[i][j].morreu == 1)
						al_draw_filled_circle(tabuleiro[i][j].x + tabuleiro[i][j].largura / 2, tabuleiro[i][j].y + tabuleiro[i][j].altura / 2, radius, blue);
					else
						al_draw_filled_circle(tabuleiro[i][j].x + tabuleiro[i][j].largura / 2, tabuleiro[i][j].y + tabuleiro[i][j].altura / 2, radius, green);
				}
				else {
					al_draw_filled_circle(tabuleiro[i][j].x + tabuleiro[i][j].largura / 2, tabuleiro[i][j].y + tabuleiro[i][j].altura / 2, radius, gray);
				}
			}
		}
	}




}


void verifica_se_acertou(struct quadradinhos **tabuleiro, struct barquinhos *barquinhos, struct player *player, int linha, int coluna, int *atirou, int which_player) {
	int acertou = 0;
	int j, i = which_player;

	if (tabuleiro[linha][coluna].tiro == 0) {
		tabuleiro[linha][coluna].tiro = 1;
		*atirou = 1;

		if (tabuleiro[linha][coluna].disponivel == 0) {
			player[i].score += 5;
			player[i].acertos++;
			for (j = 0; j < 5; j++) {
				if (barquinhos[j].tamanho == 2) {
					if ((barquinhos[j].dir == vertical && barquinhos[j].linha <= linha && barquinhos[j].linha + barquinhos[j].tamanho >= linha && barquinhos[j].coluna == coluna)
						|| (barquinhos[j].dir == horizontal && barquinhos[j].linha == linha && barquinhos[j].coluna >= coluna && barquinhos[j].coluna - barquinhos[j].tamanho <= coluna))
						acertou = 1;
				}

				if (barquinhos[j].tamanho == 3) {
					if ((barquinhos[j].dir == vertical && barquinhos[j].linha + 1 >= linha && barquinhos[j].linha - 1 <= linha && barquinhos[j].coluna == coluna)
						|| (barquinhos[j].dir == horizontal && barquinhos[j].linha == linha && barquinhos[j].coluna + 1 >= coluna && barquinhos[j].coluna - 1 <= coluna))
						acertou = 1;
				}

				if (barquinhos[j].tamanho == 4) {
					if ((barquinhos[j].dir == vertical && barquinhos[j].linha + 2 >= linha && barquinhos[j].linha - 1 <= linha && barquinhos[j].coluna == coluna)
						|| (barquinhos[j].dir == horizontal && barquinhos[j].linha == linha && barquinhos[j].coluna + 1 >= coluna && barquinhos[j].coluna - 2 <= coluna))
						acertou = 1;
				}
 
				if (barquinhos[j].tamanho == 5) {
					if ((barquinhos[j].dir == vertical && barquinhos[j].linha + 2 >= linha && barquinhos[j].linha - 2 <= linha && barquinhos[j].coluna == coluna)
						|| (barquinhos[j].dir == horizontal && barquinhos[j].linha == linha && barquinhos[j].coluna + 2 >= coluna && barquinhos[j].coluna - 2 <= coluna))
						acertou = 1;
				}


				if (acertou) {
					barquinhos[j].tiros++;
					if (barquinhos[j].tiros == barquinhos[j].tamanho)
						tabuleiro[linha][coluna].morreu = 1;
					acertou = 0;
					break;
				}
			}


		}
		else {
			player[i].erros++;
			if (player[i].score > 0) player[i].score--;
		}
	}
}


void computador_atira (struct quadradinhos **tabuleiro, int tab_size) {
	const int linha = rand() % tab_size;
	const int coluna = rand() % tab_size;

	if (tabuleiro[linha][coluna].tiro == 1)
		computador_atira(tabuleiro, tab_size);
	else
		tabuleiro[linha][coluna].tiro = 1;
}


void grava_recordes(struct player player, char *tempo) {
	FILE *fp;
	FILE *temp;

	int i;
	int qtd_linhas = 0;
	char linha[50];
	char novo_record[30];
	char ch;
	char *token;
	char **linhas;
	char tiros[3], pontos[3];
	int nova_entrada = -1;
	int x = -1;
	int achei = 0;
	int imprimi = 0;

	tempo[5] = '\n';
	tempo[6] = '\0';

	tiros[0] = (player.acertos + player.erros) / 10 + 48;
	tiros[1] = (player.acertos + player.erros) % 10 + 48;
	tiros[2] = '\0';

	pontos[0] = player.score / 10 + 48;
	pontos[1] = player.score % 10 + 48;
	pontos[2] = '\0';


	fp = fopen("recordes.txt", "a+");

	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '\n') {
			qtd_linhas++;
		}
	}

	rewind(fp);

	linhas = (char **)malloc(qtd_linhas * sizeof(char *));
	for (i = 0; i < qtd_linhas; i++)
		linhas[i] = (char *)calloc(50, sizeof(char));


	for (i = 0; i < qtd_linhas; i++) {

		memset(linha, '\0', sizeof(linha));
		fgets(linha, 50, fp);

		strcpy(linhas[i], linha);

		token = strtok(linha, ";");
		token = strtok(NULL, ";");
		token = strtok(NULL, ";");

		if (!achei && player.score > atoi(token)) {
			nova_entrada = 1;
			x = i;
			achei = 1;
		}
	}

	if (x == -1 && qtd_linhas < 8) {
		x = qtd_linhas + 1;
		nova_entrada = 1;
	}

	fclose(fp);

	if (nova_entrada) {
		strcpy(novo_record, player.nome);
		novo_record[4] = ';';
		novo_record[5] = '\0';
		strcat(novo_record, tiros);
		novo_record[7] = ';';
		novo_record[8] = '\0';
		strcat(novo_record, pontos);
		novo_record[10] = ';';
		novo_record[11] = '\0';
		strcat(novo_record, tempo);
	}


	fp = fopen("recordes.txt", "w+");
	
	for (i = 0; i < qtd_linhas; i++) {
		if (nova_entrada && i == x) {
			fprintf(fp, novo_record);
			imprimi = 1;
		}

		fprintf(fp, linhas[i]);
	}

	if (qtd_linhas < 8 && imprimi == 0)
		fprintf(fp, novo_record);
	
	rewind(fp);

	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '\n') {
			qtd_linhas++;
		}
	}

	rewind(fp);

	if (qtd_linhas > 8)
		qtd_linhas = 8;

	temp = fopen("temp.txt", "w+");

	for (i = 0; i < qtd_linhas; i++) {
		memset(linha, '\0', sizeof(linha));
		fgets(linha, 50, fp);
		fprintf(temp, linha);
	}


	fclose(fp);
	fclose(temp);

	remove("recordes.txt");
	rename("temp.txt", "recordes.txt");
}

