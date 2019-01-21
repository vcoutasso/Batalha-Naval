all : main

main: main.o menu_principal.o opcoes_menu.o funcoes_jogo.o jogo.o
	gcc -o main main.o menu_principal.o opcoes_menu.o funcoes_jogo.o jogo.o `pkg-config --libs allegro-5 allegro_audio-5 allegro_dialog-5 allegro_image-5 allegro_primitives-5 allegro_acodec-5 allegro_color-5 allegro_font-5 allegro_main-5 allegro_ttf-5`

main.o: main.c opcoes_menu.h jogo.h menu_principal.h
	gcc -o main.o main.c -c -Wall -pedantic 

menu_principal.o: menu_principal.c menu_principal.h opcoes_menu.h
	gcc -o menu_principal.o menu_principal.c -c -Wall -pedantic 


opcoes_menu.o: opcoes_menu.c opcoes_menu.h menu_principal.h jogo.h
	gcc -o opcoes_menu.o opcoes_menu.c -c -Wall -pedantic 


funcoes_jogo.o: funcoes_jogo.c funcoes_jogo.h menu_principal.h jogo.h
	gcc -o funcoes_jogo.o funcoes_jogo.c -c -Wall -pedantic 


jogo.o: jogo.c jogo.h menu_principal.h funcoes_jogo.h
	gcc -o jogo.o jogo.c -c -Wall -pedantic 

clean:
	rm -rf *.o *~ main
