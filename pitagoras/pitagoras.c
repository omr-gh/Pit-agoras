/* COLOR_BLACK   0
 * COLOR_RED     1
 * COLOR_GREEN   2
 * COLOR_YELLOW  3
 * COLOR_BLUE    4
 * COLOR_MAGENTA 5
 * COLOR_CYAN    6
 * COLOR_WHITE   7
 * */

#include <ncurses.h>
//#include <panel.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <wait.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/mman.h>
#define SONIDO_FLECHAS system("play -q sonido_teclas/flechas.ogg&");
#define SONIDO_NUMEROS system("play -q sonido_teclas/numero.ogg&");
#define SONIDO_ENTER system("play -q sonido_teclas/enter.ogg&");
#define SONIDO_CORRECTO system("play -q sonido_teclas/correcto.ogg&");
#define SONIDO_INCORRECTO system("play -q sonido_teclas/fail.ogg&");
#define SONIDO_CIERRE system("play -q sonido_teclas/cerrar.ogg");
#define SONIDO_ventana_ya_ingresado system("play -q sonido_teclas/yaIng.ogg&");
void __fpurge(FILE *stream);
void dibujar_X();
void ventana_valores();
int ing_resp();
void pintar_casilla(WINDOW *casilleros[10][10], int, int, int, int);
int *init_matriz_control(int (*matriz_control)[10][10]);
int menu(int*, int*, int*, WINDOW *win_inferior);
//void cronometro();
void sonido_si_no();
//void musica_si_no();
char presiona_numeros(char);
void set_snd(char caracter);
void ventana_estadisticas(WINDOW *, int, int, int, int);
int control_cas_rep(int (*matriz_control)[10][10], int x, int y);
void comenzar_de_nuevo(WINDOW *win_inferior);
void ventana_ya_ingresado(WINDOW *);
void ventana_instrucciones();
void ventana_bienv();
void reglas();
void ini_video(char *);
void init(char *);
void salir();

WINDOW *ventana_inf(WINDOW *win_inferior), *panel(WINDOW *casilleros_panel[10][10]);
char *cancion[] = {
					"Bad Guy", "Believer","Billie Jean","Ddu-du Ddu-Du Blackpink","Rolling Sky",
					"Ghostbusters","Free software song","Harder, Better, Faster, Stronger","Robot Rock",
					"8 bits","Sandstorm"
			    	};
			    	
int filas, columnas;
int sound = 0;
int music = 0;
WINDOW *win_vals, *menur, *info;
short n_pista;

int main()
{
	WINDOW *win_estadisticas, *win_inferior;
	FILE *fichero;
	WINDOW *casilleros_panel[10][10];
	int producto, intentos, aciertos, fallidos, porcentaje;
	int matriz_control[10][10];
	int res, repetido, enter;
	int v1, v2, valx, valy;
	char name[10], *pname;

	pname = name;
	producto = intentos = aciertos = fallidos = 0;
	valx = valy = enter = porcentaje = 0;
	
	
	//ejecuta figlet y guarda el nombre de jugador
	system("bash bienvenida.sh");
	fichero = fopen("name", "r");
	fscanf(fichero, "%s", name);
	fclose(fichero);
	
	//comienzo del juego
	init(pname);
	panel(casilleros_panel);
	init_matriz_control(&matriz_control);
	win_estadisticas = newwin(5, 82, 2, (columnas * 47.5 ) / 100); //alto, ancho, comy, comx;
	ventana_estadisticas(win_estadisticas, intentos, aciertos, fallidos, porcentaje);
	
	do{
		v1 = menu(&enter, &valx, &valy, win_inferior);
		v2 = menu(&enter, &valx, &valy, win_inferior);
		producto = v1 * v2;
		res = ing_resp();
		intentos++;
		if(res == producto){
			aciertos++;
			if(sound)
				set_snd('c');
		}
		else{
			fallidos++;
			if(sound)
				set_snd('i');
		}
		v1 -= 1;
		v2 -= 1;
		porcentaje = aciertos * 100 / intentos;
		if((v1+1) == valx)
			pintar_casilla(casilleros_panel, v2, v1, producto, res);
		else
			pintar_casilla(casilleros_panel, v1, v2, producto, res);
		repetido = control_cas_rep(&matriz_control, v1, v2);
		if(repetido){
			aciertos -= 1;
			win_inferior = ventana_inf(win_inferior);
			ventana_ya_ingresado(win_inferior);
			delwin(win_inferior);
			reglas();
		}
		ventana_estadisticas(win_estadisticas, intentos, aciertos, fallidos, porcentaje);
		v1++; v2++;
	}
	while(intentos < 101);
	return 0;
}

/* ejecuta las funciones void */
void init(char *name)
{
	ini_video(name);
	curs_set(0);
	sonido_si_no();
//	musica_si_no();
	ventana_bienv();
	ventana_valores();
	ventana_instrucciones();
	dibujar_X();
	reglas();
}

//*** inicializacion de ncurses y colores ***//
void ini_video(char *name)
{
	initscr();
	getmaxyx(stdscr, filas, columnas);
	start_color();
	char *participante = "Participante: ";
	int lenp = strlen(participante);
	int lenn = strlen(name);
	
	// pares de colores primer plano, segundo plano.
	init_pair(1, 0, 7);
	init_pair(2, 7, 3);
	init_pair(3, 7, 6);
	init_pair(4, 3, 1);
	init_pair(6, 7, 2);
	init_pair(7, 3, 2);
	init_pair(8, 1, 2);
	init_pair(9, 6, 2);
	init_pair(10, 1, 2);
	init_pair(11, 0, 2);
	init_pair(12, 7, 0);
	init_pair(13, 0, 6);
	init_pair(14, 5, 7);
	init_pair(15, 7, 4);
	init_pair(16, 6, 2);
	init_pair(17, 0, 6);
	init_pair(18, 6, 0);
	init_pair(19, 4, 2);
	init_pair(20, 2, 7);
	init_pair(21, 4, 7);
	init_pair(22, 7, 5);

	cbreak();
	noecho();
	keypad(stdscr, true);
	bkgd(COLOR_PAIR(11));
	refresh();
	attron(A_BOLD|COLOR_PAIR(6));
	mvprintw(1, (columnas-3)-(lenp+lenn), "%s", participante);
	attroff(A_BOLD|COLOR_PAIR(6));
	refresh();
	mvprintw(1, (columnas-3)-(lenp+lenn)+lenp, "%s", name);
	refresh();
}

//dibujo X
void dibujar_X()
{
	WINDOW *casilleroX;
	float y, x, porcy, porcx;

	porcy = 24;
	porcx = 30.5;
	y = (filas*porcy)/100;
	x = (columnas*porcx)/100;
	
	//casillero x
	casilleroX = newwin(3, 6, y, x);
	keypad(casilleroX, true);
	box(casilleroX, 0,0);
	mvwprintw(casilleroX, 1, 2, "%c",'X');
	wbkgd(casilleroX, A_BOLD|COLOR_PAIR(17));
	wrefresh(casilleroX);
}

//menu
int menu(int *enter, int *valx, int *valy, WINDOW *win_inferior)
{
	WINDOW *casillas[10];
	float  y, x, yv, xv, porcy, porcx, porcyv, porcxv;
	int i, op, valor;
	
	valor = 0;
	porcy = 24;
	porcx = 34.5;
	porcyv = 30.5;
	porcxv = 30.5;
	y = (filas*porcy)/100;
	x = (columnas*porcx)/100;
	yv = (filas*porcyv)/100;
	xv = (columnas*porcxv)/100;
	
	while(1){
		for(i = 1; i < 11; i++){
			//casillas horihontales
			casillas[i] = newwin(3, 6, y, x);
			keypad(casillas[i], true);
			box(casillas[i], 0,0);
			mvwprintw(casillas[i], 1, 2, "%i", i);
			wrefresh(casillas[i]);
			if(i == *valx)
				wbkgd(casillas[i], A_BOLD|COLOR_PAIR(22));
			else
				wbkgd(casillas[i], A_BOLD|A_REVERSE);//COLOR_PAIR(1));
			wrefresh(casillas[i]);

			//casillas verticales
			casillas[i] = newwin(3, 6, yv, xv);
			keypad(casillas[i], true);
			box(casillas[i], 0,0);
			mvwprintw(casillas[i], 1, 2, "%i", i);
			wrefresh(casillas[i]);
			if(i == *valy)
				wbkgd(casillas[i], A_BOLD|COLOR_PAIR(22));
			else
				wbkgd(casillas[i], A_BOLD|A_REVERSE);//COLOR_PAIR(1));
			wrefresh(casillas[i]);
			x += 6;
			yv += 3;
		}
		//reseteo los valores de la ubicacion de los casilleros
		y = (filas*porcy)/100;
		x = (columnas*porcx)/100;
		yv = (filas*porcyv)/100;
		xv = (columnas*porcxv)/100;
		op = wgetch(casillas[1]);

		if(*enter == 2){
			ventana_valores();
			*enter = 0;
		}
		//si el usuario presiona un numero
		if(isdigit(op)){
			if(op == '0' && *valx == 1){
				*valx = 10;
				valor = *valx;
			}
			else if(op == '0' && *valy == 1){
				*valy = 10;
				valor = *valy;
			}
			else{
				if(*enter == 0){
					*valx = presiona_numeros(op);
					valor = *valx;
				}
				else{
					*valy = presiona_numeros(op);
					valor = *valy;
				}
			}
			if(sound)
				set_snd('n');
		}
		else{
			switch(op){ //si presiona flechas
				case KEY_LEFT:
					(*valx)--;
					if(*valx < 1)
						*valx = 10;
					valor = *valx;
					break;
				case KEY_RIGHT:
					(*valx)++;
					if(*valx == 11)
						*valx = 1;
					valor = *valx;
					break;
				case KEY_UP:
					(*valy)--;
					if(*valy < 1)
						*valy = 10;
					valor = *valy;
					break;
				case KEY_DOWN:
					(*valy)++;
					if(*valy == 11)
						*valy = 1;
					valor = *valy;
					break;
					
				case 's': case 'S': //activa o desactiva el sonido
					if(!sound)
						sound = 1;
					else
						sound = 0;
					sonido_si_no();
					break;
				case 'm': case 'M': //activa o desactiva la musica de fondo
					if(!music)
						music = 1;
					else
						music = 0;
//					musica_si_no();
					break;
				case 27: salir(); break;
				case 'r': case 'R':
					win_inferior = ventana_inf(win_inferior);
					comenzar_de_nuevo(win_inferior);
					delwin(win_inferior);
					break;
				default: break;
			}
			if(sound)
				set_snd('f');
		}
		if(op == 10){
			if(sound)
				set_snd('e');
			if(*valx == 0 && *valy == 0)
				continue;
			else
				break;
		}
	}
	if(*enter == 0){
		wattron(win_vals, A_BOLD);
		mvwprintw(win_vals, 1,2, "%i x ", valor);
		(*enter)++;
	}
	else{
		wprintw(win_vals, "%i = ", valor);
		wattroff(win_vals, A_BOLD);
		wattron(win_vals, A_BLINK);
		waddch(win_vals, '?');
		wattroff(win_vals, A_BLINK);
		(*enter)++;
	}
	wrefresh(win_vals);
	return valor;
}



//*** Aquí se muestran los datos ingresados ***//
void ventana_valores()
{
	float y = 18;
	float x = 60;
	win_vals = newwin(3, 19, (filas * y)/100, (columnas * x)/100); //alto, ancho, comy, comx;
	keypad(win_vals, true);
	box(win_vals, 0,0);
	wbkgd(win_vals, COLOR_PAIR(15));
	wrefresh(win_vals);
}

// * * * ingreso de la respuesta * * *
int ing_resp()
{
	int i, c, res;
	char rta[4];
	i = 0;
	wattron(win_vals, A_BOLD);
	curs_set(1);
	echo();
	
	while((c = wgetch(win_vals)) != '\n' && i <= 4){
		if(c == 263){
			waddch(win_vals,' ');
			i--;
			rta[i]=0;
			waddch(win_vals,'\b');
			rta[i] = c;
			i--;
			wrefresh(win_vals);
		}
		else
			rta[i] = c;
		__fpurge(stdin);
		i++;
	}
	curs_set(0);
	noecho();
	wrefresh(win_vals);
	wattroff(win_vals, A_BOLD);
	res = atoi(rta);
	return res;
}

//*** panel de resultados ***//
WINDOW *panel(WINDOW *casilleros_panel[10][10])
{
	int k, i;
	float y, x, porcx, porcy;

	porcx = 34.5;
	porcy = 30.5;
	y = (filas * porcy)/100;
	x = (columnas * porcx)/100;

	//casillas
	for(i = 0; i < 10; i++){
		for(k = 0; k < 10; k++){
			casilleros_panel[i][k] = newwin(3, 6, y, x);
			keypad(casilleros_panel[i][k], true);
			box(casilleros_panel[i][k], 0,0);
			wbkgd(casilleros_panel[i][k], A_BOLD|COLOR_PAIR(2));
			wrefresh(casilleros_panel[i][k]);
			x+=6;
		}
		y += 3;
		x = (columnas * porcx)/100;
	}
	return casilleros_panel[10][10];
}

//*** pinta las casillas elegidas ***//
void pintar_casilla(WINDOW *casilleros_panel[10][10], int x, int y, int producto, int res)
{

	if(res == producto){ //si es correcto COLOR_PAIR(3)
		mvwprintw(casilleros_panel[x][y], 1, 2, "%i", producto);
		wbkgd(casilleros_panel[x][y], A_BOLD|COLOR_PAIR(3));
		wrefresh(casilleros_panel[x][y]);
		wattroff(casilleros_panel[x][y], A_BOLD|COLOR_PAIR(3));
	}
	else{ //si es incorrecto COLOR_PAIR(4))
		mvwprintw(casilleros_panel[x][y], 1, 2, "%i", producto);
		wbkgd(casilleros_panel[x][y], A_BOLD|COLOR_PAIR(4));
		wrefresh(casilleros_panel[x][y]);
		wattroff(casilleros_panel[x][y], A_BOLD|COLOR_PAIR(4));
	}
}

//*** mensaje de que ya se usó esa casilla ***//
void ventana_ya_ingresado(WINDOW *win_inferior)
{
	wattron(win_inferior, A_BOLD|A_BLINK);
	mvwprintw(win_inferior, 1, 1, "%s", "¡Casillero ya usado!! Presione una tecla.");
	set_snd('y');
	wrefresh(win_inferior);
	wattroff(win_inferior, A_BOLD|A_BLINK);
	getch();
	werase(win_inferior);
	wrefresh(win_inferior);
}

/* inicializa la matriz de control de los casilleros visitados*/
int *init_matriz_control(int (*matriz_control)[10][10])
{
	int i, k;
	for(i = 0; i < 10; i++){
		for(k = 0; k < 10; k++)	{
			(*matriz_control)[i][k] = 0;
		}
	}
	return matriz_control[10][10];
}


/* si el usuario presiona números */
char presiona_numeros(char op)
{
	char numeros[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	int i; char n;

	i = 0;
	while(i < 11){
		if(op == numeros[i])
			break;
		else
			i++;
	}
	return n = i;
}

/*boton sonido*/
void sonido_si_no()
{
	WINDOW *win_sonido;
	int y = filas/filas;
	int x = 1; //(columnas * 10) / 100;

	win_sonido = newwin(4, 10, y, x);
	if(sound == 0){
		wbkgd(win_sonido, A_BOLD|COLOR_PAIR(12)); // letras amarillas, fondo rojo
		mvwprintw(win_sonido, 1, 2, "Sonido");
		mvwprintw(win_sonido, 2, 4, "No");
		wattroff(win_sonido, A_BOLD|COLOR_PAIR(12));
	}else{
		wbkgd(win_sonido, A_BOLD|COLOR_PAIR(18)); // letras amarillas, fondo rojo
		mvwprintw(win_sonido, 1, 2, "Sonido");
		mvwprintw(win_sonido, 2, 4, "Si");
		wattroff(win_sonido, A_BOLD|COLOR_PAIR(18));
	}
	wattron(win_sonido, COLOR_PAIR(1)); //letras negras, fondo blanco
	box(win_sonido, ':', ':');
	wattroff(win_sonido, COLOR_PAIR(1));
	wrefresh(win_sonido);
}

/* determina que sonidos se usan  */
void set_snd(char caracter)
{

	if(caracter == 'f')
		SONIDO_FLECHAS;
	if(caracter == 'n')
		SONIDO_NUMEROS;
	if(caracter == 'e')
		SONIDO_ENTER;
	if(caracter == 'c')
		SONIDO_CORRECTO;
	if(caracter == 'i')
		SONIDO_INCORRECTO;
	if(caracter == 'o')
		SONIDO_CIERRE;
	if(caracter == 'y')
		SONIDO_ventana_ya_ingresado;
}

//muetra las estadísticas del juego
void ventana_estadisticas(WINDOW *win_estadisticas, int intentos, int aciertos, int fallidos, int porcentaje)
{
	int y = 2;
	int x = 3;
	
	//marco ventana
	wbkgd(win_estadisticas, COLOR_PAIR(16));
	wattron(win_estadisticas, A_BOLD);
	box(win_estadisticas, '*','*');
	wattroff(win_estadisticas, A_BOLD);
	wrefresh(win_estadisticas);
	wattroff(win_estadisticas, COLOR_PAIR(16));
	
	// score
	wattron(win_estadisticas, COLOR_PAIR(11));
	mvwprintw(win_estadisticas, y, x, "%s", "Estadísticas || ");
	wrefresh(win_estadisticas);
	wattroff(win_estadisticas, COLOR_PAIR(11));

	//intentos
	wattron(win_estadisticas, A_BOLD|COLOR_PAIR(6));
	mvwprintw(win_estadisticas, y, x+18, "%s %i", "intentos:", intentos);
	wrefresh(win_estadisticas);
	wattroff(win_estadisticas, COLOR_PAIR(6));

	//aciertos
	wattron(win_estadisticas, COLOR_PAIR(6));
	mvwprintw(win_estadisticas, y, x+32, "%s %i", "Aciertos:", aciertos);
	wrefresh(win_estadisticas);
	wattroff(win_estadisticas, COLOR_PAIR(6));

	//fallidos
	wattron(win_estadisticas, COLOR_PAIR(8));
	mvwprintw(win_estadisticas, y, x+46, "%s %i", "Fallidos:", fallidos);
	wrefresh(win_estadisticas);
	wattroff(win_estadisticas, COLOR_PAIR(6));

	//porcentaje
	wattron(win_estadisticas, COLOR_PAIR(6));
	mvwprintw(win_estadisticas, y, x+60, "%s %s", "% Aciertos:", "   ");
	mvwprintw(win_estadisticas, y, x+60, "%s %i", "% Aciertos:", porcentaje);
	wrefresh(win_estadisticas);
	wattroff(win_estadisticas, A_BOLD|COLOR_PAIR(6));
}


//controla si el casillero fue visitado
int control_cas_rep(int (*matriz_control)[10][10], int x, int y)
{
	int ingresado = 0;

	if((*matriz_control)[x][y] >= 0){
		(*matriz_control)[x][y]++;
	}
	if((*matriz_control)[x][y] >= 2){
		ingresado = 1;
	}

	return ingresado;
}

//*** comienza el juego de nuevo. Resetea todo ***//
void comenzar_de_nuevo(WINDOW *win_inferior)
{
	int c;
	char *comenzar = "¿Comenzar de nuevo? S/n ";
	char *abortado = "Abortado.";

	mvwprintw(win_inferior, 1, 1, "%s", comenzar);
	echo();
	curs_set(1);
	wrefresh(win_inferior);
	c = wgetch(win_inferior);
	if(c == 's' || c == 'S'){
		c = wgetch(win_inferior);
		main();
	}
	else if(c == 'n' || c == 'N'){
		c = wgetch(win_inferior);
		salir();
	}
	else{
		c = wgetch(win_inferior);
		werase(win_inferior);
		mvwprintw(win_inferior, 1, 1, "%s", abortado);
		wrefresh(win_inferior);
		napms(1500);
		salir();
	}
}

// ventana inferior para mostrar mensjes
WINDOW *ventana_inf(WINDOW *win_inferior)
{
	win_inferior = newwin(3, 43, filas-5, 1);
	wbkgd(win_inferior, COLOR_PAIR(6));
	wrefresh(win_inferior);
	return win_inferior;
}

//salir del juego
void salir()
{
	set_snd('o');
	system("killall play");
	endwin();
	system("/usr/bin/clear");
	system("figlet -c Gracias por usar Pitagoras !");
	exit(0);
}

/* ventana uso del teclado */
void ventana_instrucciones()
{
	int y = (filas * 18)/100;
	int x = (columnas * 74) / 100;

	WINDOW *instrucciones;
	instrucciones = newwin(36, 40, y, x);
	keypad(instrucciones, true);
	box(instrucciones, 'x', 'x');
	wbkgd(instrucciones, COLOR_PAIR(12));
	wrefresh(instrucciones);

	/* instrucciones */
	wattron(instrucciones, COLOR_PAIR(18));
	wmove(instrucciones, 2,2);
	wprintw(instrucciones, "%s", "¿ C O M O   J U G A R ?");
	wmove(instrucciones, 4,2);
	wprintw(instrucciones, "%c %s", 62, "Presione s para activar o");
	wrefresh(instrucciones);
	wmove(instrucciones, 5,4);
	wprintw(instrucciones, "%s", "desactivar el sonido.");
	wmove(instrucciones, 7,2);
	wprintw(instrucciones, "%c %s", 62, "Use las flechas para desplazarse");
	wmove(instrucciones, 8,4);
	wprintw(instrucciones, "%s", "por las barras.");
	wmove(instrucciones, 10, 2);
	wprintw(instrucciones, "%c %s", 62, "Luego de seleccionar un operando");
	wmove(instrucciones, 11, 4);
	wprintw(instrucciones, "%s", "presione la tecla Enter");
	wmove(instrucciones, 13,2);
	wprintw(instrucciones, "%c %s", 62, "Puede optar por usar los números");
	wmove(instrucciones, 14,4);
	wprintw(instrucciones, "%s", "en lugar de las flechas.");
	wmove(instrucciones, 16, 2);
	wprintw(instrucciones, "%c %s", 62, "Introduzca el resultado, luego");
	wmove(instrucciones, 17, 4);
	wprintw(instrucciones, "%s", "presione la tecla Enter");
	wmove(instrucciones, 18, 4);
	wprintw(instrucciones, "%s", "nuevamente.");
	wattroff(instrucciones, COLOR_PAIR(18));
	wrefresh(instrucciones);
}

//*** leyenda en la izquierda de la tabla ***//
void ventana_bienv()
{
	float y,x;
	y = 24;
	x = 1;

	WINDOW *ventana_bienv;
	ventana_bienv = newwin(13, 43, (filas * y)/100, x);
	wbkgd(ventana_bienv, COLOR_PAIR(16));
	wattron(ventana_bienv, COLOR_PAIR(15));
	box(ventana_bienv, 58, 58);
	wattroff(ventana_bienv, COLOR_PAIR(15));
	wrefresh(ventana_bienv);
	wattron(ventana_bienv, A_BOLD);
	mvwprintw(ventana_bienv, 2,2, "%s", "¡ ¡   B I E N V E N I D O   A   L A");
	mvwprintw(ventana_bienv, 4,2, "%s", "T A B L A   D E   P I T A G O R A S ! !");
	mvwprintw(ventana_bienv, 6,2, "%s", "PRUEBA TU HABILIDAD PARA LAS TABLAS DE");
	mvwprintw(ventana_bienv, 8,2, "%s", "MULTIPLICAR. TIENES 100 INTENTOS PARA");
	mvwprintw(ventana_bienv, 10,2, "%s", "COMPLETARLA. * * ¡ S U E R T E ! * *");
	wrefresh(ventana_bienv);
	wattroff(ventana_bienv, A_BOLD);
}

// *** muestra las regals *** //
void reglas()
{
	WINDOW *win_reglas;
	float y;
	
	y = 62;
	win_reglas = newwin(15, 43, (filas * y) / 100, 1);
	box(win_reglas, 0, 0);
	wbkgd(win_reglas, COLOR_PAIR(13));

	wmove(win_reglas, 4,1);
	waddch(win_reglas, '*');
	wmove(win_reglas, 8,1);
	waddch(win_reglas, '*');
	wmove(win_reglas, 11,1);
	waddch(win_reglas, '*');
	mvwaddstr(win_reglas, 2, 10, "***  A T E N C I O N  ***");
	mvwaddstr(win_reglas, 4, 2, "Si se usa una misma casilla dos o");
	wrefresh(win_reglas);
	mvwaddstr(win_reglas, 5, 2, "más veces se sumarán los intentos");
	mvwaddstr(win_reglas, 6, 2, "pero no los aciertos.");
	wrefresh(win_reglas);
	mvwaddstr(win_reglas, 8, 2, "Los fallidos se sumarán de cualquier");
	mvwaddstr(win_reglas, 9, 2, "manera.");
	wrefresh(win_reglas);
	mvwaddstr(win_reglas, 11, 2, "Pulsa Esc para salir");
	wrefresh(win_reglas);
	mvwaddstr(win_reglas, 12, 2, "o R para reiniciar.");
	wrefresh(win_reglas);
	wattroff(win_reglas, A_BOLD | COLOR_PAIR(13) | COLOR_PAIR(14));
}



