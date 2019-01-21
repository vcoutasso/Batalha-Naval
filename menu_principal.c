#include "menu_principal.h"
#include "opcoes_menu.h"

// Verifica se algum dos ponteiros alocados está apontado para NULL.
int check_pointers(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, ALLEGRO_FONT *tut_font,
	ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_SAMPLE *click, ALLEGRO_TIMER *display_timer) {

	if (display == NULL || font == NULL || emph_font == NULL || tut_font == NULL || queue == NULL ||
		click == NULL || display_timer == NULL) {
		return 0;
	}

	return 1;
}

// Verifica se houve erro ao alocar memoria para algum dos items do menu.
int check_items(struct item_menu *item1, struct item_menu *item2, struct item_menu *item3, struct item_menu *item4) {
	if (item1 == NULL || item2 == NULL || item3 == NULL || item4 == NULL)
		return 0;

	return 1;
}

// Imprime o item na tela de maneira apropriada.
void print_item(int is_hovering, ALLEGRO_FONT *font, ALLEGRO_FONT *emph_font, struct item_menu *item, int marcado) {

	ALLEGRO_COLOR yellow = al_map_rgb(255, 180, 0), orange = al_map_rgb(255, 180, 90), green = al_map_rgb(60, 255, 60);

	// Verifica se o item é selecionável e se o mouse está sobre ele. Imprime na tela de acordo.
	if (!is_hovering && !marcado)
		al_draw_text(font, yellow, item->x, item->y, 0, item->str);
	else if (marcado)
		al_draw_text(font, green, item->x, item->y, 0, item->str);
	else
		al_draw_text(emph_font, orange, item->e_x, item->y, 0, item->str);

}

// Função do menu principal.
void invoke_main_menu() {
	int is_running = 1;
	const int normal_font_size = NORMAL_FSIZE;
	const int emph_font_size = HOVER_FSIZE;
	const int tut_font_size = TUT_FSIZE;
	const int grow_font_size = GROW_FSIZE;
	const int fps = 60;
	int is_hovering_quit = 0;
	int is_hovering_howto = 0;
	int is_hovering_play = 0;
	int is_hovering_scores = 0;
	int hover_sound = 0;

	// Retira o frame da tela e deixa em modo tela cheia para o windows e sem frame para o linux.
#ifdef LINUX
	al_set_new_display_flags(ALLEGRO_NOFRAME);
#endif

#ifdef WINDOWS
	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
#endif

	// Declaração de membros das structs da biblioteca allegro.
	ALLEGRO_DISPLAY *display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
	ALLEGRO_FONT *font = al_load_ttf_font("Resources/Chunkfive.otf", normal_font_size, 0);
	ALLEGRO_FONT *emph_font = al_load_ttf_font("Resources/Chunkfive.otf", emph_font_size, 0);
	ALLEGRO_FONT *grow_font = al_load_ttf_font("Resources/Chunkfive.otf", grow_font_size, 0);
	ALLEGRO_FONT *tut_font = al_load_ttf_font("Resources/Chunkfive.otf", tut_font_size, 0);
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
	ALLEGRO_SAMPLE *click = al_load_sample("Resources/click.wav");
	ALLEGRO_SAMPLE *blop = al_load_sample("Resources/blop.wav");
	ALLEGRO_TIMER *display_timer = al_create_timer((float)1 / fps);
	ALLEGRO_EVENT event;
	ALLEGRO_BITMAP *icon = al_load_bitmap("Resources/icon.png");
	const ALLEGRO_COLOR light_blue = al_map_rgb(0, 180, 255), yellow = al_map_rgb(255, 180, 0), orange = al_map_rgb(255, 180, 90);

	// Caso algum dos ponteiros esteja apontando para NULL mesmo após a inicialização, uma mensagem de erro é exibida e o programa é encerrado. 
	if (!check_pointers(display, font, emph_font, tut_font, queue, click, display_timer)) {
		al_show_native_message_box(NULL, "Batalha Naval", "Error", "Memory allocation error!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		exit(1);
	}

	if (icon == NULL) {
		al_show_native_message_box(NULL, "Batalha Naval", "Error", "Memory allocation error!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		exit(1);
	}

	al_set_window_title(display, "Batalha Naval");
	al_set_display_icon(display, icon);

	// Declaração de membros da struct item_menu
	struct item_menu *play = malloc(sizeof(*play));
	struct item_menu *howto = malloc(sizeof(*howto));
	struct item_menu *scores = malloc(sizeof(*scores));
	struct item_menu *quit = malloc(sizeof(*quit));

	// Verifica se tudo correu bem na alocação.
	if (!check_items(play, howto, scores, quit)) {
		al_show_native_message_box(NULL, "Batalha Naval", "Error", "Memory allocation error!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		exit(1);
	}

	// Inicializa os itens do menu.
	init_item(play, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 2 * normal_font_size, "Novo Jogo", font, emph_font);
	init_item(howto, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, "Como Jogar", font, emph_font);
	init_item(scores, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 2 * normal_font_size, "Recordes", font, emph_font);
	init_item(quit, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 4 * normal_font_size, "Sair", font, emph_font);

	// Source de eventos. Se uma source de evento não for registrada, os eventos dessa source nunca serão contabilizados e colocados na queue.
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_timer_event_source(display_timer));

	// Inicializa contagem do timer.
	al_start_timer(display_timer);

	// Aguarda um evento e inicializa o dummy_event pro resharper parar de encher o saco.
	al_wait_for_event(queue, &event);
	ALLEGRO_EVENT dummy_event = event;

	// Loop principal enquanto a variavel is_running = 1.
	while (is_running) {

		// Aguarda o próximo evento antes de fazer qualquer ação. Essa espera evita o loop de rodar continuamente mesmo quando não há nada a ser feito,
		// resultando em um alto consumo de recursos como processamento da CPU sem necessidade.
		al_wait_for_event(queue, &event);


		switch (event.type) {
			// Verifica se o mouse esté sobre alguma opção. Caso positivo, essa opção deve ser enfatizada com um tamanho de fonte maior e cor diferente.
		case ALLEGRO_EVENT_MOUSE_AXES:
			// Se o mouse estiver sobre a opção de novo jogo.
			if (is_hovering(play, event, normal_font_size)) {

				// Flag para tocar o sample de hover.
				if (!is_hovering_play)
					hover_sound = 1;

				// Flag para indicar que o mouse está sobre a opção em questão.
				is_hovering_play = 1;
			}
			// Se o mouse estiver sobre a opção de como jogar.
			else if (is_hovering(howto, event, normal_font_size)) {

				if (!is_hovering_howto)
					hover_sound = 1;

				is_hovering_howto = 1;
			}
			// Se o mouse estiver sobre a opção recordes.
			else if (is_hovering(scores, event, normal_font_size)) {

				if (!is_hovering_scores)
					hover_sound = 1;

				is_hovering_scores = 1;
			}
			// Se o mouse estiver sobre a opção sair.
			else if (is_hovering(quit, event, normal_font_size)) {

				if (!is_hovering_quit)
					hover_sound = 1;

				is_hovering_quit = 1;
			}
			// Se o mouse não estiver sobre nenhuma opção.
			else {
				is_hovering_quit = 0;
				is_hovering_howto = 0;
				is_hovering_play = 0;
				is_hovering_scores = 0;
			}
			break;

			// 'dummy_event' e 'event' são uma variaveis do tipo ALLEGRO_EVENT. A variavel 'event' recebe os eventos da 'queue' e
			// armazena informações como a posição do mouse na tela. dummy_event está interessada especificamente nesses dados
			// e utiliza eles para saber a posição do mouse ao pressionar o botao do mouse. 'event' terá as posições do mouse
			// ao soltar o botão. Dessa forma é possível comparar as posições do mouse ao pressionar e soltar o botão.
			// Isso impede de clicar em uma area vazia da tela, arrastar para uma opção valida e, mesmo assim, ter acesso à ela.
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			if (event.mouse.button == 1) {
				dummy_event = event;
			}
			break;

			// Se o tipo de evento for ALLEGRO_EVENT_MOUSE_BUTTON_UP, quer dizer que algum dos botões foi pressionado e solto.
			// Ao ser solto, o evento é colocado na fila. Essa seção do switch case é responsável por gerenciar os cliques do mouse.
			// Se o clique for sobre uma opção, alguma ação deve ser tomada.
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			if (event.mouse.button == 1) {
				if (is_hovering_quit && is_hovering(quit, dummy_event, normal_font_size)) {
					is_hovering_quit = 0;
					is_running = 0;
				}
				else if (is_hovering_howto && is_hovering(howto, dummy_event, normal_font_size)) {
					is_hovering_howto = 0;
					if (how_to(font, emph_font, tut_font, queue, click, display_timer, event, light_blue, yellow, orange) == -1)
						is_running = 0;
				}
				else if (is_hovering_play && is_hovering(play, dummy_event, normal_font_size)) {
					is_hovering_play = 0;
					if (new_game(font, emph_font, queue, click, blop, display_timer, event, light_blue, yellow, orange) == -1)
						is_running = 0;
				}
				else if (is_hovering_scores && is_hovering(scores, dummy_event, normal_font_size)) {
					is_hovering_scores = 0;
					if (records(font, emph_font, queue, click, display_timer, event, light_blue, yellow, orange) == -1)
						is_running = 0;
				}
			}
			break;
			// O programa é fechado ao clicar no X. Apesar de não aparecer o X no modo fullscreen, funciona na miniatura.
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			is_running = 0;
			break;
			// Atualiza a tela a cada tick do timer. A frequencia de atualização se dá pelo inverso do valor de fps.
		case ALLEGRO_EVENT_TIMER:
			al_clear_to_color(light_blue);

			if (hover_sound) {
				hover_sound = 0;
				al_play_sample(click, 1.0, 0, 3, ALLEGRO_PLAYMODE_ONCE, NULL);
			}


			// Desenha as opções do menu na tela. Se o mouse estiver sobre alguma opção, essa opção deverá ser apresenta de outra forma.
			print_item(is_hovering_play, font, emph_font, play, 0);
			print_item(is_hovering_howto, font, emph_font, howto, 0);
			print_item(is_hovering_scores, font, emph_font, scores, 0);
			print_item(is_hovering_quit, font, emph_font, quit, 0);
			al_draw_text(grow_font, yellow, SCREEN_WIDTH / 6, SCREEN_HEIGHT / 8, 0, "BATALHA NAVAL");

			al_flip_display();
			break;

		default:
			break;
		}
	}

	// Libera todos os ponteiros que estavam sendo utilizados. Equivalente a um free().
	al_destroy_timer(display_timer);
	al_destroy_sample(click);
	al_destroy_sample(blop);
	al_destroy_event_queue(queue);
	al_destroy_font(font);
	al_destroy_font(emph_font);
	al_destroy_font(tut_font);
	al_destroy_display(display);

	free(play);
	free(howto);
	free(scores);
	free(quit);
}