#include "opcoes_menu.h"
#include "menu_principal.h"
#include "jogo.h"

// Gerencia os eventos que acontecem quando um há um clique no mouse.
void clique(int **hovering, int **vet_check, int *start, int *voltar, int *tam_tab, ALLEGRO_EVENT event) {
	const int tam_vet = 8;
	int i;

	// Se o clique foi com o botão esquerdo do mouse.
	// vet_check guarda as informações de qual opção foi selecionada, desmarcando as outras de maneira que apenas as de interesse permaneçam marcadas.
	if (event.mouse.button == 1) {

		for (i = 0; i < tam_vet; i++)
		{
			if (*hovering[i] == 1) {
				if (i == 0)
					*voltar = 1;
				else if (i == 1) {
					*vet_check[0] = 1;
					*vet_check[1] = 0;
				}
				else if (i == 2) {
					*vet_check[0] = 0;
					*vet_check[1] = 1;
				}
				else if (i == 3) {
					*vet_check[2] = 1;
					*vet_check[3] = 0;
					*vet_check[4] = 0;
					*tam_tab = 8;
				}
				else if (i == 4) {
					*vet_check[2] = 0;
					*vet_check[3] = 1;
					*vet_check[4] = 0;
					*tam_tab = 10;
				}
				else if (i == 5) {
					*vet_check[2] = 0;
					*vet_check[3] = 0;
					*vet_check[4] = 1;
					*tam_tab = 12;
				}
				else if (i == 6) {
					*start = 1;

				}
				else if (i == 7) {
					if (!*vet_check[5])
						*vet_check[5] = 1;
					else
						*vet_check[5] = 0;
				}
			}
		}
	}
}

// Verifica se o mouse está em cima da opção.
int is_hovering(struct item_menu *item, ALLEGRO_EVENT event, int size) {

	if (event.mouse.x >= item->x && event.mouse.x <= item->x + item->width &&
		event.mouse.y >= item->y && event.mouse.y <= item->y + size)
		return 1;

	return 0;
}

// Inicializa item do menu.
void init_item(struct item_menu *item, float x, float y, char text[MAX_ITEM_LENGTH], ALLEGRO_FONT *font, ALLEGRO_FONT *e_font) {
	item->x = x - al_get_text_width(font, text) / 2;
	item->y = y;
	item->e_x = x - al_get_text_width(e_font, text) / 2;
	strcpy(item->str, text);
	item->width = al_get_text_width(font, text);
}

// Função chamada ao clicar na opção de novo jogo dentro do menu principal.
int new_game(ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_SAMPLE *click, ALLEGRO_SAMPLE *blop,
	ALLEGRO_TIMER *display_timer, ALLEGRO_EVENT event, ALLEGRO_COLOR light_blue, ALLEGRO_COLOR yellow, ALLEGRO_COLOR orange) {

	ALLEGRO_COLOR red = al_map_rgb(255, 60, 60);

	int voltar = 0; // Volta para o menu anterior.
	int tam = 0; // Tamanho do tabuleiro.
	int hover_sound = 0; // Indicador que o som de passar o mouse em cima de uma opção deve ser tocado.

	// Começar jogo.
	int start = 0;

	// Variaveis para verificar se o mouse está sobre uma determinada opção do menu.
	int is_hovering_pvp = 0;
	int is_hovering_pvai = 0;
	int is_hovering_8x8 = 0;
	int is_hovering_10x10 = 0;
	int is_hovering_12x12 = 0;
	int is_hovering_play = 0;
	int is_hovering_back = 0;
	int is_hovering_rand = 0;

	// Vetor contendo as variaveis de hover. Utilizado para possibilitar uma unica chamada da função clique para checar tudo.
	int *hovering_vector[8] = { &is_hovering_back, &is_hovering_pvp, &is_hovering_pvai, &is_hovering_8x8, &is_hovering_10x10, &is_hovering_12x12, &is_hovering_play, &is_hovering_rand };


	// Variaveis para verificar se a opção do tipo selecionavel está marcada.
	int check_pvp = 0;
	int check_pvai = 0;
	int check_8x8 = 0;
	int check_10x10 = 0;
	int check_12x12 = 0;
	int check_rand = 0;
	int game_mode = 0;

	// Vetor contendo as variaveis de check. Reduz o numero de parametros necesasario para chamar a função clique.
	int *vet_check[6] = { &check_pvp, &check_pvai, &check_8x8, &check_10x10, &check_12x12, &check_rand };

	// Variaveis do tipo struct item_menu que representam as opçoes do menu que aparecem na tela.
	struct item_menu *vs_p = malloc(sizeof *vs_p);
	struct item_menu *vs_ai = malloc(sizeof *vs_ai);
	struct item_menu *eight2 = malloc(sizeof *eight2);
	struct item_menu *ten2 = malloc(sizeof *ten2);
	struct item_menu *twelve2 = malloc(sizeof *twelve2);
	struct item_menu *back = malloc(sizeof *back);
	struct item_menu *play = malloc(sizeof *play);
	struct item_menu *rand = malloc(sizeof *rand);

	// Verifica se algum dos ponteiros aponta para NULL.
	if (!check_items(vs_p, vs_ai, eight2, ten2) || !check_items(twelve2, back, play, rand)) {
		al_show_native_message_box(NULL, "Batalha Naval", "Error", "Memory allocation error!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		exit(1);
	}

	// Inicializa as opções do menu.
	init_item(vs_p, SCREEN_WIDTH / 4, 3 * NORMAL_FSIZE, "2 Jogadores", font, emph_font);
	init_item(vs_ai, SCREEN_WIDTH / 4 + 1.4 * al_get_text_width(font, "Jogador vs AI"), 3 * NORMAL_FSIZE, "Jogador vs AI", font, emph_font);
	init_item(eight2, SCREEN_WIDTH / 2 - 3 * al_get_text_width(font, "8x8"), 7 * NORMAL_FSIZE, "8x8", font, emph_font);
	init_item(ten2, SCREEN_WIDTH / 2, 7 * NORMAL_FSIZE, "10x10", font, emph_font);
	init_item(twelve2, SCREEN_WIDTH / 2 + 2 * al_get_text_width(font, "12x12"), 7 * NORMAL_FSIZE, "12x12", font, emph_font);
	init_item(back, 150, 650, "Voltar", font, emph_font);
	init_item(play, 1200, 650, "Jogar", font, emph_font);
	init_item(rand, SCREEN_WIDTH / 2, 550, "Aleatorio", font, emph_font);

	// Limpa tela.
	al_clear_to_color(light_blue);

	// Loop principal da função.
	while (!voltar) {

		// Aguarda receber o próximo evento.
		al_wait_for_event(queue, &event);

		switch (event.type) {

			// ALLEGRO_EVENT_MOUSE_AXES é gerado quando o cursor do mouse muda de posição. Aqui, essa informação é utilizada 
			// para verificar se o cursor esta sobre uma opção do menu, para emitir o som de hover e imprimir a opção na tela.
		case ALLEGRO_EVENT_MOUSE_AXES:
			if (is_hovering(back, event, NORMAL_FSIZE)) {

				if (!is_hovering_back)
					hover_sound = 1;

				is_hovering_back = 1;
			}
			else if (is_hovering(vs_p, event, NORMAL_FSIZE)) {

				if (!is_hovering_pvp & !check_pvp)
					hover_sound = 1;

				is_hovering_pvp = 1;
			}
			else if (is_hovering(vs_ai, event, NORMAL_FSIZE)) {

				if (!is_hovering_pvai & !check_pvai)
					hover_sound = 1;

				is_hovering_pvai = 1;
			}
			else if (is_hovering(eight2, event, NORMAL_FSIZE)) {

				if (!is_hovering_8x8 & !check_8x8)
					hover_sound = 1;

				is_hovering_8x8 = 1;
			}
			else if (is_hovering(ten2, event, NORMAL_FSIZE)) {

				if (!is_hovering_10x10 & !check_10x10)
					hover_sound = 1;

				is_hovering_10x10 = 1;
			}
			else if (is_hovering(twelve2, event, NORMAL_FSIZE)) {

				if (!is_hovering_12x12 & !check_12x12)
					hover_sound = 1;

				is_hovering_12x12 = 1;
			}
			else if (is_hovering(rand, event, NORMAL_FSIZE)) {
				if (!is_hovering_rand & !check_rand)
					hover_sound = 1;

				is_hovering_rand = 1;

			}

			else if (is_hovering(play, event, NORMAL_FSIZE)) {
				if (!is_hovering_play)
					hover_sound = 1;

				is_hovering_play = 1;
			}
			else {
				is_hovering_pvp = 0;
				is_hovering_pvai = 0;
				is_hovering_8x8 = 0;
				is_hovering_10x10 = 0;
				is_hovering_12x12 = 0;
				is_hovering_back = 0;
				is_hovering_play = 0;
				is_hovering_rand = 0;
			}
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			//dummy_event = event;
			break;

		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:

			// Verifica se o clique foi sobre uma opção do menu.
			clique(hovering_vector, vet_check, &start, &voltar, &tam, event);

			break;

			// O programa é fechado ao clicar no X. Apesar de não aparecer o X no modo fullscreen, funciona na miniatura.
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			return -1;

		case ALLEGRO_EVENT_TIMER:
			// Limpa buffer para impedir que sucessivos cliques realizem diversas operaçoes sem necessidade.
			al_flush_event_queue(queue);
			if (hover_sound) {
				hover_sound = 0;
				al_play_sample(click, 1.0, 0, 3, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			// Toca um efeito sonoro bobo e chama a função do jogo.
			if (start) {
				al_play_sample(blop, 1.0, 0, 2, ALLEGRO_PLAYMODE_ONCE, NULL);
				if ((check_8x8 || check_10x10 || check_12x12) && (check_pvai || check_pvp)) {
					al_flush_event_queue(queue);
					if (check_pvp)
						game_mode = 2;
					if (check_pvai)
						game_mode = 1;
					if (o_jogo(tam, game_mode, display_timer, queue, event, check_rand, font, emph_font, click) == -1)
						return -1;
				}
				start = 0;
			}

			al_clear_to_color(light_blue);

			al_draw_text(font, red, SCREEN_WIDTH / 2 - al_get_text_width(font, "Modo de Jogo") / 2, NORMAL_FSIZE, 0, "Modo de Jogo");
			al_draw_text(font, red, SCREEN_WIDTH / 2 - al_get_text_width(font, "Tamanho do Tabuleiro") / 2, 5 * NORMAL_FSIZE, 0, "Tamanho do Tabuleiro");

			// Desenha as opções do menu na tela. As variaveis check verificam se o item está selecionado se for do tipo selecionavel.

			print_item(is_hovering_pvp, font, emph_font, vs_p, check_pvp);
			print_item(is_hovering_pvai, font, emph_font, vs_ai, check_pvai);
			print_item(is_hovering_8x8, font, emph_font, eight2, check_8x8);
			print_item(is_hovering_10x10, font, emph_font, ten2, check_10x10);
			print_item(is_hovering_12x12, font, emph_font, twelve2, check_12x12);
			print_item(is_hovering_back, font, emph_font, back, 0);
			print_item(is_hovering_play, font, emph_font, play, 0);
			print_item(is_hovering_rand, font, emph_font, rand, check_rand);

			// Atualiza a tela.
			al_flip_display();

			break;

		default:
			break;
		}
	}

	free(vs_p);
	free(vs_ai);
	free(eight2);
	free(ten2);
	free(twelve2);
	free(back);
	free(play);
	free(rand);

	return 0;
}

int how_to(ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, ALLEGRO_FONT *tut_font, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_SAMPLE *click, ALLEGRO_TIMER *display_timer,
	ALLEGRO_EVENT event, ALLEGRO_COLOR light_blue, ALLEGRO_COLOR yellow, ALLEGRO_COLOR orange) {

	int hover_sound = 0;
	int is_hovering_back = 0;
	int is_hovering_prox = 0;
	int voltar = 0;
	int pagina = 1;
	struct item_menu *prox = malloc(sizeof * prox);
	struct item_menu *back = malloc(sizeof *back);

	init_item(back, 150, 650, "Voltar", font, emph_font);
	init_item(prox, 1100, 650, "Próximo", font, emph_font);

	while (!voltar) {

		al_wait_for_event(queue, &event);

		switch (event.type) {
		case ALLEGRO_EVENT_MOUSE_AXES:
			if (is_hovering(back, event, NORMAL_FSIZE)) {
				if (!is_hovering_back)
					hover_sound = 1;
				is_hovering_back = 1;
			}
			else
				is_hovering_back = 0;
			if (is_hovering(prox, event, NORMAL_FSIZE)) {
				if (!is_hovering_prox)
					hover_sound = 1;
				is_hovering_prox = 1;
			}
			else
				is_hovering_prox = 0;
			break;

			// O programa é fechado ao clicar no X. Apesar de não aparecer o X no modo fullscreen, funciona na miniatura.
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			return -1;

		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			if (is_hovering_back)
				voltar = 1;
			if (is_hovering_prox)
				pagina = 2;

			break;

		case ALLEGRO_EVENT_TIMER:

			if (pagina == 1) {

				al_clear_to_color(light_blue);

				al_draw_text(font, yellow, 60, 40, 0, "Preparando o jogo...");
				al_draw_text(tut_font, yellow, 60, 120, 0, "PASSO 1: Escolher o modo de jogo (P1 x P2 ou P1 x PC).");
				al_draw_text(tut_font, yellow, 60, 200, 0, "PASSO 2: Escolher o tamanho do tabuleiro.");
				al_draw_text(tut_font, yellow, 60, 280, 0, "PASSO 3: A opção 'aleatorio' coloca os barcos automaticamente.");
				al_draw_text(tut_font, yellow, 60, 360, 0, "PASSO 4: Com sua configuração selecionada, inicie o jogo.");


				print_item(is_hovering_back, font, emph_font, back, 0);
				print_item(is_hovering_prox, font, emph_font, prox, 0);
				al_flip_display();
			}
			if (pagina == 2) {

				al_clear_to_color(light_blue);
				al_draw_text(font, yellow, 60, 40, 0, "Como jogar?");
				al_draw_text(tut_font, yellow, 60, 120, 0, "PASSO 1: Definir a posição de cada barco se necessário.");
				al_draw_text(tut_font, yellow, 60, 200, 0, "PASSO 2: Clicar em atirar para jogar em seu turno.");
				al_draw_text(tut_font, yellow, 60, 280, 0, "PASSO 3: Escolher a posição desejada e clicar sobre a mesma.");
				al_draw_text(tut_font, yellow, 60, 360, 0, "Essa posição poderá ser pintada com vermelho, indicando um erro,");
				al_draw_text(tut_font, yellow, 60, 440, 0, "verde para acertos ou azul indicando que um navio foi afundado.");
				al_draw_text(tut_font, yellow, 60, 520, 0, "Após o seu turno, será a vez de seu advesário fazer um movimento.");
				al_draw_text(tut_font, yellow, 60, 600, 0, "Uma posição pintada em cinza indica que foi alvejada.");


				print_item(is_hovering_back, font, emph_font, back, 0);

				al_flip_display();

			}


			print_item(is_hovering_back, font, emph_font, back, 0);
			print_item(is_hovering_prox, font, emph_font, prox, 0);


			if (hover_sound) {
				hover_sound = 0;
				al_play_sample(click, 1.0, 0, 3, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			break;


		default:
			break;
		}
	}

	free(prox);
	free(back);

	return 0;
}

int records(ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_SAMPLE *click, ALLEGRO_TIMER *display_timer,
	ALLEGRO_EVENT event, ALLEGRO_COLOR light_blue, ALLEGRO_COLOR yellow, ALLEGRO_COLOR orange) {

	char ch;
	char vazio[] = "Nenhum registro encontrado.";
	char texto[20];
	char linha[50];
	char *token;
	int is_hovering_back = 0;
	int voltar = 0;
	int hover_sound = 0;
	int qtd = 0;
	int i;
	int altura;
	struct item_menu *back = malloc(sizeof(*back));

	ALLEGRO_FONT *font1 = al_load_ttf_font("Resources/CooperHewitt-Heavy.otf", 60, 0);

	init_item(back, 150, 650, "Voltar", font, emph_font);

	FILE *fp = fopen("recordes.txt", "a+");

	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '\n')
			qtd++;
	}

	rewind(fp);

	while (!voltar) {

		al_wait_for_event(queue, &event);

		switch (event.type) {
		case ALLEGRO_EVENT_MOUSE_AXES:
			if (is_hovering(back, event, NORMAL_FSIZE))
			{
				if (!is_hovering_back)
					hover_sound = 1;
				is_hovering_back = 1;
			}
			else
				is_hovering_back = 0;
			break;

		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			if (is_hovering_back)
				voltar = 1;
			break;


		case ALLEGRO_EVENT_TIMER:
			al_clear_to_color(light_blue);
			print_item(is_hovering_back, font, emph_font, back, 0);

			if (qtd == 0) {
				al_draw_text(font1, yellow, SCREEN_WIDTH / 2 - al_get_text_width(font1, vazio) / 2, 300, 0, vazio);
			}
			else {
				if (qtd > 8) qtd = 8;

				altura = 40;

				al_draw_text(font1, yellow, 60, altura, 0, "NOME");
				al_draw_text(font1, yellow, 380, altura, 0, "TIROS");
				al_draw_text(font1, yellow, 740, altura, 0, "PONTOS");
				al_draw_text(font1, yellow, 1060, altura, 0, "TEMPO");

				altura = 140;

				for (i = 0; i < qtd; i++) {
					fgets(linha, 50, fp);

					token = strtok(linha, ";");
					memset(texto, '\0', sizeof(texto));
					strcpy(texto, token);
					al_draw_text(font1, yellow, 80, altura, 0, texto);

					token = strtok(NULL, ";");
					memset(texto, '\0', sizeof(texto));
					strcpy(texto, token);
					al_draw_text(font1, yellow, 430, altura, 0, texto);

					token = strtok(NULL, ";");
					memset(texto, '\0', sizeof(texto));
					strcpy(texto, token);
					al_draw_text(font1, yellow, 810, altura, 0, texto);

					token = strtok(NULL, ";");
					memset(texto, '\0', sizeof(texto));
					strcpy(texto, token);
					texto[strlen(texto) - 1] = '\0';
					al_draw_text(font1, yellow, 1080, altura, 0, texto);

					altura += 60;
				}

				rewind(fp);
			}


			if (hover_sound)
			{
				hover_sound = 0;
				al_play_sample(click, 1.0, 0, 3, ALLEGRO_PLAYMODE_ONCE, NULL);
			}

			al_flip_display();
			break;

			// O programa é fechado ao clicar no X. Apesar de não aparecer o X no modo fullscreen, funciona na miniatura.
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			return -1;

		default:
			break;

		}
	}

	free(back);
	fclose(fp);

	al_destroy_font(font1);

	return 0;
}
