#include "opcoes_menu.h"
#include "jogo.h"
#include "menu_principal.h"

int main() {

	srand(time(0));

	// Inicializa Allegro e addons.
	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_acodec_addon();
	al_init_primitives_addon();
	al_init_image_addon();

	// Instala periféricos.
	al_install_keyboard();
	al_install_mouse();
	al_install_audio();

	// Reserva os samples. O parametro passado incrementa em um para cada novo sample que será utilizado.
	al_reserve_samples(2);

	// Invoca o menu principal.
	invoke_main_menu();


	// Desinstala periféricos.
	al_uninstall_keyboard();
	al_uninstall_mouse();


	// Retorna 0.
	return 0;
}