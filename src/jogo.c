#include "menu_principal.h"
#include "jogo.h"
#include "funcoes_jogo.h"

// Esse jogo mesmo. Perdeu meu parça.
int o_jogo(int tab_size, int game_mode, ALLEGRO_TIMER *timer, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT event, int check_rand, ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, ALLEGRO_SAMPLE *click) {

	int i, j;

	// Configura o nome dos arquivos .png de acordo com o tamanho de tabuleiro, visto que cada tabuleiro tem seu
	// conjunto de arquivos.
	char bitmap_barco[4][50] = { "bin/barco2_", "bin/barco3_", "bin/barco4_", "bin/barco5_" };

	for (i = 0; i < 4; i++) {
		if (tab_size == 8)
			strcat(bitmap_barco[i], "tab8.png");
		else if (tab_size == 10)
			strcat(bitmap_barco[i], "tab10.png");
		else if (tab_size == 12)
			strcat(bitmap_barco[i], "tab12.png");
	}


	ALLEGRO_FONT *small_font = al_load_ttf_font("bin/Chunkfive.otf", 50, 0);
	ALLEGRO_FONT *font1 = al_load_ttf_font("bin/CooperHewitt-Heavy.otf", HOVER_FSIZE, 0);
	ALLEGRO_FONT *tiny_little_font = al_load_ttf_font("bin/CooperHewitt-Heavy.otf", 20, 0);

	// Vetor contendo todos os bitmaps.
	ALLEGRO_BITMAP *barquinho[] = { al_load_bitmap(bitmap_barco[0]), al_load_bitmap(bitmap_barco[1]), al_load_bitmap(bitmap_barco[1]), al_load_bitmap(bitmap_barco[2]), al_load_bitmap(bitmap_barco[3]) };

	// Recebe o ponteiro para o display atual. Elimina a necessidade de passar como parametro.
	ALLEGRO_DISPLAY *display = al_get_current_display();

	ALLEGRO_TIMER *relogio = al_create_timer((float)1 / 1);

	int minutos = 00;
	int segundos = 00;

	al_register_event_source(queue, al_get_timer_event_source(relogio));

	int sair = 0;
	int numero = 0;
	int linha, coluna;
	int is_hovering_start = 0, is_hovering_reroll = 0, is_hovering_atirar = 0;
	int tec = 0;
	int comecou = 0;
	int fechar = 0;
	int which_player = 1;
	int hora_do_pau = 0;
	int atirou = 0;
	int espera = 253;
	int game_over = 0;
	int winner = 0;

	// Cria os arquivos .txt. Se já houver algum com o mesmo nome, será substituido por um arquivo em branco.
	FILE *fp;

	fp = fopen("tabuleiro1.txt", "w");
	fclose(fp);

	fp = fopen("tabuleiro2.txt", "w");
	fclose(fp);

	struct quadradinhos **tabuleiro_p1 = (struct quadradinhos **) malloc(tab_size * sizeof(struct quadradinhos *));
	struct quadradinhos **tabuleiro_p2 = (struct quadradinhos **) malloc(tab_size * sizeof(struct quadradinhos *));
	struct barquinhos *barquinhos_p1 = (struct barquinhos *) malloc(NUM_BARCOS * sizeof(struct barquinhos));
	struct barquinhos *barquinhos_p2 = (struct barquinhos *) malloc(NUM_BARCOS * sizeof(struct barquinhos));

	struct item_menu *start = malloc(sizeof(struct item_menu));
	struct item_menu *reroll = malloc(sizeof(struct item_menu));
	struct item_menu *atirar = malloc(sizeof(struct item_menu));

	struct tabuleiro **tabuleiro = malloc(tab_size * sizeof(struct tabuleiro *));

	struct player player[2];

	ALLEGRO_COLOR yellow = al_map_rgb(255, 180, 0);

	char que_oracao[7] = "00:00";
	char score[20] = "Pontos: ";
	char hit[20] = "Acertos: ";
	char miss[20] = "Erros: ";
	char aux[20];

	init_item(start, 1220, 680, "Ready!", font, emph_font);
	init_item(reroll, 1230, 600, "Reroll", font, emph_font);
	init_item(atirar, 1220, 680, "Atirar!", font, emph_font);

	// Inicializa as structs contendo informaçôes dos players.
	for (i = 0; i < 2; i++) {
		player[i].score = 0;
		player[i].acertos = 0;
		player[i].erros = 0;
	}

	// Informações sobre o tabuleiro.
	// Quantidade de quadradinhos que cada barquinho ocupa
	barquinhos_p1[0].tamanho = 2;
	barquinhos_p1[1].tamanho = 3;
	barquinhos_p1[2].tamanho = 3;
	barquinhos_p1[3].tamanho = 4;
	barquinhos_p1[4].tamanho = 5;

	barquinhos_p2[0].tamanho = 2;
	barquinhos_p2[1].tamanho = 3;
	barquinhos_p2[2].tamanho = 3;
	barquinhos_p2[3].tamanho = 4;
	barquinhos_p2[4].tamanho = 5;

	// Valor impar em direcao indica que o barco está na vertical. Valor par indica que está na horizontal.
	for (i = 0; i < NUM_BARCOS; i++) {
		barquinhos_p1[i].dir = vertical;
		barquinhos_p1[i].tiros = 0;

		barquinhos_p2[i].dir = vertical;
		barquinhos_p2[i].tiros = 0;
	}

	// Esconde o cursor para posicionar os barcos no tabuleiro.
	al_hide_mouse_cursor(display);

	// Variaveis que definem a posição que os barquinhos serão impressos (spoiler: em cima do mouse).
	float xis = SCREEN_WIDTH / 2, ipsolon = SCREEN_HEIGHT / 2;

	// Aloca memória para os vetores.
	for (i = 0; i < tab_size; i++) {
		tabuleiro_p1[i] = (struct quadradinhos *) malloc(tab_size * sizeof(struct quadradinhos));
		tabuleiro_p2[i] = (struct quadradinhos *) malloc(tab_size * sizeof(struct quadradinhos));
		tabuleiro[i] = malloc(tab_size * sizeof(struct tabuleiro));
	}

	// Calcula as posições nos eixos x e y de cada quadradinho. 
	// i representa a linha
	// j representa a coluna
	for (i = 0; i < tab_size; i++) {
		for (j = 0; j < tab_size; j++) {
			tabuleiro_p1[i][j].disponivel = 1;
			tabuleiro_p1[i][j].tiro = 0;
			tabuleiro_p1[i][j].morreu = 0;

			tabuleiro_p2[i][j].tiro = 0;
			tabuleiro_p2[i][j].disponivel = 1;
			tabuleiro_p2[i][j].morreu = 0;

			tabuleiro[i][j].y = SCREEN_HEIGHT * OUTRA_BORDA + (i * SCREEN_HEIGHT * (1 - 2 * OUTRA_BORDA) / tab_size);
			tabuleiro[i][j].x = SCREEN_WIDTH * BORDA_LATERAL + (j * SCREEN_WIDTH * (1 - 2 * BORDA_LATERAL) / tab_size);
			tabuleiro[i][j].largura = SCREEN_WIDTH * (1 - 2 * BORDA_LATERAL) / tab_size;
			tabuleiro[i][j].altura = SCREEN_HEIGHT * (1 - 2 * OUTRA_BORDA) / tab_size;
		}
	}

	// check_rand indica que a opção de posicionar os barquinhos aleatoriamente foi selecionada.
	if (check_rand) {
		if (aleatorio(tabuleiro_p1, barquinhos_p1, tabuleiro, tab_size, 1) == -1) {
			al_show_native_message_box(NULL, "Batalha Naval", "Error", "Eita deu ruim na hora de colocar os barquinhos", NULL, ALLEGRO_MESSAGEBOX_ERROR);
			exit(1);
		}
		if (aleatorio(tabuleiro_p2, barquinhos_p2, tabuleiro, tab_size, 2) == -1) {
			al_show_native_message_box(NULL, "Batalha Naval", "Error", "Eita deu ruim na hora de colocar os barquinhos", NULL, ALLEGRO_MESSAGEBOX_ERROR);
			exit(1);
		}
		else {
			numero = 5;
			for (i = 0; i < numero; i++) {
				barquinhos_p1[i].bitmap = barquinho[i];
				barquinhos_p2[i].bitmap = barquinho[i];
			}
			numero++;
		}
	}


	// Arranja o tabuleiro e barquinhos para que o jogo possa começar.
	while (!sair) {
		al_wait_for_event(queue, &event);

		switch (event.type) {

		case ALLEGRO_EVENT_TIMER:
			desenha_tabuleiro(tab_size);
			nome_quadradinhos(tabuleiro, tiny_little_font, yellow, tab_size);


			if (!comecou) print_item(is_hovering_start, font, emph_font, start, 0);
			if (!comecou && check_rand == 1) print_item(is_hovering_reroll, font, emph_font, reroll, 0);

			al_draw_text(font, yellow, 20, 20, 0, que_oracao);

			al_draw_text(small_font, yellow, 20, 140, 0, "Pontos: 0");
			al_draw_text(small_font, yellow, 20, 200, 0, "Acertos: 0");
			al_draw_text(small_font, yellow, 20, 260, 0, "Erros: 0");

			if (which_player == 1) {
				desenha_barquinhos(barquinhos_p1, numero, tab_size);
				al_draw_text(font, yellow, 20, 680, 0, "Player 1");
			}
			else {
				desenha_barquinhos(barquinhos_p2, numero, tab_size);
				al_draw_text(font, yellow, 20, 680, 0, "Player 2");
			}


			if (numero < NUM_BARCOS) {
				if (which_player == 1)
					al_draw_rotated_bitmap(barquinho[numero], al_get_bitmap_width(barquinho[numero]) / 2, al_get_bitmap_height(barquinho[numero]) / 2, xis + al_get_bitmap_width(barquinho[numero]) / 2, ipsolon + al_get_bitmap_height(barquinho[numero]) / 2, rad(barquinhos_p1[numero].dir), 0);
				else
					al_draw_rotated_bitmap(barquinho[numero], al_get_bitmap_width(barquinho[numero]) / 2, al_get_bitmap_height(barquinho[numero]) / 2, xis + al_get_bitmap_width(barquinho[numero]) / 2, ipsolon + al_get_bitmap_height(barquinho[numero]) / 2, rad(barquinhos_p2[numero].dir), 0);
			}

			if (tec) {
				tec = 0;
				al_play_sample(click, 1.0, 0, 3, ALLEGRO_PLAYMODE_ONCE, NULL);
			}

			al_flip_display();

			break;
			// Ao pressionar espaço o barco gira 90° no sentido anti-horário. Esc sai do jogo.
		case ALLEGRO_EVENT_KEY_DOWN:
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				sair = 1;
				al_show_mouse_cursor(display);
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
				if (numero < NUM_BARCOS) {
					if (which_player == 1)
						gira_barquinho(barquinho[numero], barquinhos_p1, tab_size, numero, xis, ipsolon);
					else
						gira_barquinho(barquinho[numero], barquinhos_p2, tab_size, numero, xis, ipsolon);
				}


			}

			break;
			// "Substitui" o cursor do mouse pelo próximo barquinho que deve ser posicionado no tabuleiro.
		case ALLEGRO_EVENT_MOUSE_AXES:
			if (comecou != 1 && is_hovering(start, event, NORMAL_FSIZE) && numero > NUM_BARCOS) {
				if (!is_hovering_start)
					tec = 1;

				is_hovering_start = 1;
			}
			else
				is_hovering_start = 0;

			if (check_rand == 1 && comecou != 1 && is_hovering(reroll, event, NORMAL_FSIZE) && numero > NUM_BARCOS) {
				if (!is_hovering_reroll)
					tec = 1;

				is_hovering_reroll = 1;
			}
			else
				is_hovering_reroll = 0;


			if (numero < NUM_BARCOS && !check_rand) {
				xis = event.mouse.x - al_get_bitmap_width(barquinho[numero]) / 2;
				ipsolon = event.mouse.y - al_get_bitmap_height(barquinho[numero]) / 2;

				// Impede que o barco seja impresso fora do tabuleiro.
				if (event.mouse.x < BORDA_LATERAL * SCREEN_WIDTH)
					xis = BORDA_LATERAL * SCREEN_WIDTH - al_get_bitmap_width(barquinho[numero]) / 2;
				else if (event.mouse.x > (1 - BORDA_LATERAL) * SCREEN_WIDTH)
					xis = (1 - BORDA_LATERAL) * SCREEN_WIDTH - al_get_bitmap_width(barquinho[numero]) / 2;

				if (event.mouse.y < OUTRA_BORDA * SCREEN_HEIGHT)
					ipsolon = OUTRA_BORDA * SCREEN_HEIGHT - al_get_bitmap_height(barquinho[numero]) / 2;
				else if (event.mouse.y > (1 - OUTRA_BORDA) * SCREEN_HEIGHT)
					ipsolon = (1 - OUTRA_BORDA) * SCREEN_HEIGHT - al_get_bitmap_height(barquinho[numero]) / 2;
			}
			else if (numero >= NUM_BARCOS) {
				al_show_mouse_cursor(display);
				numero++;
			}
			break;
			// Clicar com o botão esquerdo posiciona o barquinho no tabuleiro.
			// Clicar duplamente com o botão direito remove o último barquinho posicionado.
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			// Botão esquerdo.
			if (event.mouse.button == 1) {
				if (numero <= NUM_BARCOS) {
					if (which_player == 1) {
						qual_quadradinho(event.mouse.x, event.mouse.y, &linha, &coluna, tabuleiro, tab_size);
						averigua_quadradinho(&linha, &coluna, tabuleiro_p1, barquinhos_p1[numero], tab_size);
					}
					else {
						qual_quadradinho(event.mouse.x, event.mouse.y, &linha, &coluna, tabuleiro, tab_size);
						averigua_quadradinho(&linha, &coluna, tabuleiro_p2, barquinhos_p2[numero], tab_size);
					}
					if (linha == -1 || coluna == -1)
						break;

					numero++;

					if (which_player == 1)
						posiciona_barquinho(barquinhos_p1, tabuleiro, tabuleiro_p1, barquinho, linha, coluna, tab_size, numero, "tabuleiro1.txt");
					else
						posiciona_barquinho(barquinhos_p2, tabuleiro, tabuleiro_p2, barquinho, linha, coluna, tab_size, numero, "tabuleiro2.txt");

				}


				if (is_hovering_start) {
					is_hovering_start = 0;
					if (game_mode == TWO_PLAYERS) {
						which_player = 2;
						game_mode = -1;
						if (!check_rand) {
							al_hide_mouse_cursor(al_get_current_display());
							numero = 0;
						}
					}
					else {
						comecou = 1;
						sair = 1;
						which_player = 1;
					}
				}

				if (is_hovering_reroll) {
					if (which_player == 1) {
						limpa_tabuleiro(tabuleiro_p1, tab_size);
						aleatorio(tabuleiro_p1, barquinhos_p1, tabuleiro, tab_size, 1);
					}
					else {
						limpa_tabuleiro(tabuleiro_p2, tab_size);
						aleatorio(tabuleiro_p2, barquinhos_p2, tabuleiro, tab_size, 2);
					}
				}

			}
			// Botão direito.
			else if (event.mouse.button == 2 && !check_rand) {
				if (numero >= 6)
					numero = 5;
				if (numero > 0) {
					if (numero <= NUM_BARCOS) al_hide_mouse_cursor(display);

					if (which_player == 1)
						atualiza_tabuleiro(tabuleiro_p1, barquinhos_p1[numero - 1], 0);
					else
						atualiza_tabuleiro(tabuleiro_p2, barquinhos_p2[numero - 1], 0);

					numero--;
				}
			}
			break;

			// Permite fechar o jogo fechando a tela.
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			sair = 1;
			fechar = 1;
			break;

		default:
			break;
		}
	}

	if (comecou) {
		sair = 0;

		al_flush_event_queue(queue);
		al_start_timer(relogio);

		while (!sair) {

			al_wait_for_event(queue, &event);

			switch (event.type) {


			case ALLEGRO_EVENT_TIMER:
				if (event.timer.source == timer) {


					desenha_tabuleiro(tab_size);
					nome_quadradinhos(tabuleiro, tiny_little_font, yellow, tab_size);

					if (which_player == 1)
						i = 0;
					else
						i = 1;

					if (!(which_player == 2 && game_mode == SINGLE_PLAYER)) {

						memset(score, '\0', sizeof(score));
						strcpy(score, "Pontos: ");
						memset(hit, '\0', sizeof(hit));
						strcpy(hit, "Acertos: ");
						memset(miss, '\0', sizeof(miss));
						strcpy(miss, "Erros: ");
						memset(aux, '\0', sizeof(aux));

						if (player[i].score < 10)
							aux[0] = player[i].score + 48;
						else {
							aux[0] = player[i].score / 10 + 48;
							aux[1] = player[i].score % 10 + 48;
						}
						strcat(score, aux);

						memset(aux, '\0', sizeof(aux));
						if (player[i].acertos < 10)
							aux[0] = player[i].acertos + 48;
						else {
							aux[0] = player[i].acertos / 10 + 48;
							aux[1] = player[i].acertos % 10 + 48;
						}
						strcat(hit, aux);

						memset(aux, '\0', sizeof(aux));
						if (player[i].erros < 10)
							aux[0] = player[i].erros + 48;
						else {
							aux[0] = player[i].erros / 10 + 48;
							aux[1] = player[i].erros % 10 + 48;
						}
						strcat(miss, aux);
					}

					al_draw_text(small_font, yellow, 20, 140, 0, score);
					al_draw_text(small_font, yellow, 20, 200, 0, hit);
					al_draw_text(small_font, yellow, 20, 260, 0, miss);

					if (which_player == 1 || (game_mode == SINGLE_PLAYER && which_player == 2))
						al_draw_text(font, yellow, 20, 680, 0, "Player 1");

					else if (game_mode != SINGLE_PLAYER)
						al_draw_text(font, yellow, 20, 680, 0, "Player 2");

					if (hora_do_pau) {
						if (which_player == 1)
							pinta_tabuleiro(tabuleiro_p2, tabuleiro, tab_size, 0);
						else
							pinta_tabuleiro(tabuleiro_p1, tabuleiro, tab_size, 0);
					}

					al_draw_text(font, yellow, 20, 20, 0, que_oracao);

					if (espera < 30) {
						espera++;
						if (which_player == 1)
							pinta_tabuleiro(tabuleiro_p2, tabuleiro, tab_size, 0);
						else
							pinta_tabuleiro(tabuleiro_p1, tabuleiro, tab_size, 0);
						al_flip_display();
						break;
					}

					if (espera == 30) {
						if (which_player == 1)
							which_player = 2;
						else
							which_player = 1;
						espera = 100;
						break;
					}

					if (player[0].acertos == 17) {
						if (game_mode != SINGLE_PLAYER)
							al_draw_text(font1, yellow, SCREEN_WIDTH / 2 - al_get_text_width(font1, "Player 1 venceu!") / 2, SCREEN_HEIGHT / 2 - HOVER_FSIZE / 2, 0, "Player 1 venceu!");
						else
							al_draw_text(font1, yellow, SCREEN_WIDTH / 2 - al_get_text_width(font1, "Você venceu!") / 2, SCREEN_HEIGHT / 2 - HOVER_FSIZE / 2, 0, "Você venceu!");

						al_flip_display();
						al_rest(3);
						sair = 1;
						game_over = 1;
						winner = 0;
						break;
					}

					if (player[1].acertos == 17) {
						if (game_mode != SINGLE_PLAYER)
							al_draw_text(font1, yellow, SCREEN_WIDTH / 2 - al_get_text_width(font1, "Player 2 venceu!") / 2, SCREEN_HEIGHT / 2 - HOVER_FSIZE / 2, 0, "Player 2 venceu!");
						else
							al_draw_text(font1, yellow, SCREEN_WIDTH / 2 - al_get_text_width(font1, "Você perdeu!") / 2, SCREEN_HEIGHT / 2 - HOVER_FSIZE, 0, "Você perdeu!");

						al_flip_display();
						al_rest(3);
						sair = 1;
						game_over = 1;
						winner = 1;
						break;
					}

					if (which_player == 1 && !hora_do_pau) {
						desenha_barquinhos(barquinhos_p1, numero, tab_size);
						pinta_tabuleiro(tabuleiro_p1, tabuleiro, tab_size, 1);
						print_item(is_hovering_atirar, font, emph_font, atirar, 0);
					}
					else if (game_mode != SINGLE_PLAYER && !hora_do_pau) {
						desenha_barquinhos(barquinhos_p2, numero, tab_size);
						pinta_tabuleiro(tabuleiro_p2, tabuleiro, tab_size, 1);
						print_item(is_hovering_atirar, font, emph_font, atirar, 0);
					}

					if (game_mode == SINGLE_PLAYER && which_player == 2) {
						which_player = 1;
						computador_atira(tabuleiro_p1, tab_size);
					}

					if (tec) {
						tec = 0;
						al_play_sample(click, 1.0, 0, 3, ALLEGRO_PLAYMODE_ONCE, NULL);
					}

					al_flip_display();
				}
				else if (event.timer.source == relogio) {

					if (segundos != 99 && minutos != 99)
						segundos++;
					if (segundos == 60 && minutos != 99) {
						segundos = 00;
						minutos++;
					}
					else if (minutos == 99 && segundos != 99) {
						segundos++;
					}
					if (segundos == 99 && minutos > 99)
						segundos = 99;

					que_oracao[0] = minutos / 10 + 48;
					que_oracao[1] = minutos % 10 + 48;

					if (segundos == 0) {
						que_oracao[3] = '0';
						que_oracao[4] = '0';
					}
					else {
						que_oracao[3] = segundos / 10 + 48;
						que_oracao[4] = segundos % 10 + 48;
					}
				}
				break;

			case ALLEGRO_EVENT_MOUSE_AXES:
				if (!hora_do_pau) {
					if (is_hovering(atirar, event, NORMAL_FSIZE)) {
						if (!is_hovering_atirar)
							tec = 1;

						is_hovering_atirar = 1;
					}
					else {
						is_hovering_atirar = 0;
					}
				}

				break;

			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				if (is_hovering_atirar)
					hora_do_pau = 1;

				if (hora_do_pau) {
					qual_quadradinho(event.mouse.x, event.mouse.y, &linha, &coluna, tabuleiro, tab_size);

					if (linha == -1 || coluna == -1) {
						break;
					}

					if (which_player == 1)
						verifica_se_acertou(tabuleiro_p2, barquinhos_p2, player, linha, coluna, &atirou, 0);
					else
						verifica_se_acertou(tabuleiro_p1, barquinhos_p1, player, linha, coluna, &atirou, 1);

					if (atirou) {
						atirou = 0;
						hora_do_pau = 0;

						espera = 0;
					}

				}

				break;

			case ALLEGRO_EVENT_KEY_DOWN:
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					sair = 1;
				break;

			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				sair = 1;
				fechar = 1;
				break;

			default:
				break;
			}
		}
	}

	que_oracao[5] = '\0';

	int contador = 0;

	if (game_over) {
		if (game_mode == SINGLE_PLAYER && winner == 1) {
			strcpy(player[winner].nome, "pcAI");
		}
		desenha_tabuleiro(tab_size);
		nome_quadradinhos(tabuleiro, tiny_little_font, yellow, tab_size);
		al_draw_text(font1, yellow, SCREEN_WIDTH / 2 - al_get_text_width(font1, "Digite seu nome") / 2, SCREEN_HEIGHT / 2 - HOVER_FSIZE / 2, 0, "Digite seu nome");
		al_flip_display();

		al_flush_event_queue(queue);

		while (contador < 4) {
			al_wait_for_event(queue, &event);

			switch (event.type) {
			case ALLEGRO_EVENT_KEY_CHAR:
				player[winner].nome[contador] = event.keyboard.unichar;
				player[winner].nome[contador + 1] = '\0';
				contador++;

				desenha_tabuleiro(tab_size);
				nome_quadradinhos(tabuleiro, tiny_little_font, yellow, tab_size);
				al_draw_text(font1, yellow, SCREEN_WIDTH / 2 - al_get_text_width(font1, player[winner].nome) / 2, SCREEN_HEIGHT / 2 - HOVER_FSIZE / 2, 0, player[winner].nome);
				al_flip_display();
				break;

			default:
				break;
			}
		}

		grava_recordes(player[winner], que_oracao);
	}

	// Desalocação de memória.
	for (i = 0; i < tab_size; i++) {
		free(tabuleiro_p1[i]);
		free(tabuleiro_p2[i]);
		free(tabuleiro[i]);
	}


	free(barquinhos_p1);
	free(barquinhos_p2);
	free(tabuleiro_p1);
	free(tabuleiro_p2);
	free(tabuleiro);

	for (i = 0; i < NUM_BARCOS; i++) {
		al_destroy_bitmap(barquinho[i]);
	}

	al_destroy_timer(relogio);
	al_destroy_font(small_font);
	al_destroy_font(font1);
	al_destroy_font(tiny_little_font);

	free(start);
	free(reroll);
	free(atirar);

	if (!fechar)
		return 1;

	return -1;
}
