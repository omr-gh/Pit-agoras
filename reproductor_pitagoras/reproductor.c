// reproductor para pitagoras

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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#define SIZE 5
void __fpurge(FILE *stream);
void init();
void time_info0();
void time_info();
void selec_song(short*, short*, short*);
void informa_pista(short*);
void muestra_pista(short*);
void win_info();
void cronometro();
void sig_handler(int sig);
void salir();
WINDOW *menu, *info;

char *cancion[] = {
    "Bad Guy", "Believer","Billie Jean","Ddu-du Ddu-Du Blackpink","Rolling Sky",
    "Ghostbusters","Free Software Song","Around The World","Robot Rock",
    "8 Bits","Sandstorm"
};

int main()
{
    pid_t pid;
    int i, readbytes, p[2];
    char buffer[SIZE];
    short pista_rep, pista, enter;
    pista_rep = pista = enter = 0;
    signal(SIGINT, sig_handler);
    init();
    win_info();
    time_info0();
	
    if(pipe(p) == -1) exit(-1);
    pid = fork();

    if(pid > 0){
		close(p[0]);
		while(1){
			selec_song(&pista_rep, &pista, &enter);
			informa_pista(&pista_rep);
			muestra_pista(&pista_rep);
			time_info();
			sprintf(buffer, "%d", pista_rep);
			system("bash terminar_procs.sh 2> /dev/null");
			system("killall play 2>/dev/null");
			write(p[1], buffer, strlen(buffer));
		}
		close(p[1]);
    }
    else{
    	close(p[1]);
		while((readbytes = read(p[0], buffer, SIZE)) > 0){
			pista_rep = atoi(buffer);
			for(i = 0; i < strlen(buffer); i++)
				buffer[i] = '\0';
			if(pista_rep != 11)
				system("bash reproducir_pista.sh");
		}
		close(p[0]);
    }
    waitpid(pid, NULL, 0);
    exit(0);
    return 0;
}

void sig_handler(int sig)
{
    salir();
}

void init()
{
    initscr();
    start_color();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr,TRUE);
    
    //nro. pares - primer plano - segundo plano
    init_pair(1, 3, 1);
    init_pair(2, 2, 0);
    init_pair(3, 3, 0);
    init_pair(4, 4, 0);
    init_pair(5, 6, 0);
    init_pair(6, 5, 0);
    init_pair(7, 6, 1);
    refresh();
}

// menu principal donde se selecciona la cancion
void selec_song(short *pista_rep,  short *pista, short *enter)
{
    int y, x, yt;
    int i, op;

    y = 13;
    x = 40 ;
    yt = 1;

    //ventana principal reproductor
    menu = newwin(y, x, 0,0);
    curs_set(0);
    keypad(menu, TRUE);
    box(menu,0,0);

    //ventanas playlist
    while(1){
      for(i = 0; i < 11; i++){
		  if(i == *pista){
			  wattron(menu, A_BOLD|COLOR_PAIR(1));
			  if(i < 9)
			  mvwprintw(menu, yt, 1, "%d.  %s", i+1, *(cancion + i));
			  else
			  mvwprintw(menu, yt, 1, "%d. %s", i+1, *(cancion + i));
			  wattroff(menu, A_BOLD|COLOR_PAIR(1));
		  }
		  else{
			  if(i < 9)
			  mvwprintw(menu, yt, 1, "%d.  %s", i+1,  *(cancion + i));
			  else
			  mvwprintw(menu, yt, 1, "%d. %s", i+1,  *(cancion + i));
		  }
		  yt ++;
      }
      yt = 1;

      //resalta la pista en reproducción
      if(*pista_rep >= 0 && *enter == 1){
		  wattron(menu, A_BLINK|A_BOLD|COLOR_PAIR(5));
		  if(*pista_rep < 9)
			  mvwprintw(menu, (*pista_rep)+1, 1, "%d.  %s", (*pista_rep)+1, *(cancion + (*pista_rep)));
		  else
			  mvwprintw(menu, (*pista_rep)+1, 1, "%d. %s", (*pista_rep)+1, *(cancion + (*pista_rep)));
	  wattroff(menu, A_BLINK|A_BOLD|COLOR_PAIR(5));
	  wrefresh(menu);
      }
      if(*pista_rep >= 0 && *pista_rep == *pista && *enter == 1){
	      wattron(menu, A_BLINK|A_BOLD|COLOR_PAIR(7));
		  if(*pista_rep < 9)
		  	  mvwprintw(menu, (*pista_rep)+1, 1, "%d.  %s", (*pista_rep)+1, *(cancion + (*pista_rep)));
		   else
			  mvwprintw(menu, (*pista_rep)+1, 1, "%d. %s", (*pista_rep)+1, *(cancion + (*pista_rep)));
	  wattroff(menu, A_BLINK|A_BOLD|COLOR_PAIR(7));
	  wrefresh(menu);
      }

      op = wgetch(menu);
      switch(op){
		  case KEY_UP:
			  (*pista)--;
			  if(*pista < 0)
			  	*pista = 10;
			  break;
		  case KEY_DOWN:
			  (*pista)++;
			  if(*pista > 10)
			  	*pista = 0;
			  break;
		  case KEY_LEFT:
			  system("amixer -q sset Master 5%-");
			  break;
		  case KEY_RIGHT:
			  system("amixer -q sset Master 5%+");
			  break;
		  default: break;
      }
      if(op == 10){
      	*pista_rep = *pista;
	  	*enter = 1;
	  	break;
      }
   }
}

//ventana que muestra la informacion de canciones
void win_info()
{
    info = newwin(4,40, 13,0);
    box(info, 0,0);
    mvwprintw(info, 0, 5, "%s", "|                      |");
    wrefresh(info);
}

//crea el fichero pista y le pasa la pista seleccionada
void informa_pista(short *ppista)
{
    FILE *fichero;
    char n_pista[3];
    short pista = *ppista;
    sprintf(n_pista, "%d", pista);
    fichero = fopen("pista", "w");
    fprintf(fichero, n_pista);
    fclose(fichero);
}

//muestra el nombre de la cancion seleccionada al pie de la ventana
void muestra_pista(short *pista)
{
    int y, x, i, len;
    char titulo[35];
    strcpy(titulo, cancion[*pista]);
    len = strlen(cancion[*pista]);
    getmaxyx(info, y, x);
    wmove(info, y-2, x-36);

    for(i = 0; i < 35; i++)
		wprintw(info, " ");
    wrefresh(info);
    wattron(info, A_BOLD|COLOR_PAIR(6));
    wmove(info, y-2, 5);
	wprintw(info, "-- ");	
	
    if(len >= 25){
		for(i = 0; i < len; i++){
			if(i == 28)
				break;
			wprintw(info, "%c", titulo[i]);
		}
		wprintw(info, "...");
    }
    else
	    wprintw(info, "%s --", titulo);
    wrefresh(info);
    wattroff(info, A_BOLD|COLOR_PAIR(6));
}

//rellena el espacio que ocupará la info de la canción
void time_info0()
{
    wattron(info, A_BOLD|COLOR_PAIR(3));
    mvwprintw(info, 0, 6, "%s", "Time");
    wattroff(info, A_BOLD|COLOR_PAIR(3));
	
    wattron(info, A_BOLD|COLOR_PAIR(2));
    mvwprintw(info, 1, 5, "[%.2d:%.2d:%.2d]", 0,0,0);
    wattroff(info, A_BOLD|COLOR_PAIR(2));
    wattron(info, A_BOLD|COLOR_PAIR(2));
    mvwprintw(info, 1, 5, "[%.2d:%.2d:%.2d]", 0,0,0);
    wattroff(info, A_BOLD|COLOR_PAIR(2));
	
    wattron(info, A_BOLD|COLOR_PAIR(5));
    mvwprintw(info, 0, 20, "%s", "Duration");
    wattroff(info, A_BOLD|COLOR_PAIR(5));
	
    wattron(info, A_BOLD|COLOR_PAIR(4));
    mvwprintw(info,1,19,"[%.2d:%.2d:%.2d]", 0,0,0);
    wrefresh(info);
    wattroff(info, A_BOLD|COLOR_PAIR(4));
}

//obtiene la pista de pista
//guarda la info en time_info y muestra la duracion de la cancion
void time_info()
{
    FILE *fichero;
    int h,m,s;

    system("bash extrae_info_audio.sh");
    fichero = fopen("time_info", "r");
    fscanf(fichero, "%d:%d:%d", &h, &m, &s);
    fclose(fichero);
	
    wattron(info, A_BOLD|COLOR_PAIR(5));
    mvwprintw(info, 0, 20, "%s", "Duration");
    wattroff(info, A_BOLD|COLOR_PAIR(5));
	
    wattron(info, A_BOLD|COLOR_PAIR(4));
    mvwprintw(info,1,19,"[%.2d:%.2d:%.2d]", h, m, s);
    wrefresh(info);
    wattroff(info, A_BOLD|COLOR_PAIR(4));
	
}

void cronometro()
{
    FILE *fichero;
    int h, m, s, time;
    int horas = 0, minutos, segundos;
    fichero = fopen("time_info", "r");
    fscanf(fichero, "%d:%d:%d", &h, &m, &s);
    fclose(fichero);
    time = (h*3600)+(m*60) + s;
	
    for(minutos = 0; minutos <= m; minutos++){
    	for(segundos = 0; segundos < 60; segundos++){
			fflush(stdout);
			wattron(info, A_BOLD|COLOR_PAIR(3));
			mvwprintw(info, 0, 6, "%s", "Playing...");
			wrefresh(info);
			wattroff(info, A_BOLD|COLOR_PAIR(3));
			wattron(info, A_BOLD|COLOR_PAIR(2));
			mvwprintw(info, 1, 5, "[%.2d:%.2d:%.2d]", horas, minutos, segundos);
			sleep(1);
			wrefresh(info);
			wattroff(info, A_BOLD|COLOR_PAIR(2));
			time--;
			if(time == 0)
				break;
		}
    }
}

void erase_win(WINDOW *win)
{
    werase(win);
    wrefresh(win);
    refresh();
}

void salir()
{
    system("bash terminar_procs.sh");
    system("killall play 2>/dev/null");
    endwin();
    exit(0);
}
