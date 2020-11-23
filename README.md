# Pit-agoras v1.0
Juego interactivo en consola escrito en C con la librería ncurses. Está pensado para alumnos de primaria.
Simula una tabla pitagórica en la que el participante deberá probar su
conocimiento en las tablas de multiplicar.
Lo complementa un pequeño y simple reproductor de audio, el cual puede usarse
en forma independiente. El proyecto completo está en desarrollo.

Pre-requisitos:
Se necesita instalar los siquientes paquetes:
compilador gcc sox libsox-fmt-all espeak mbrola mbrola-es1 mbrola-es2 -
libncurses5-dev libncursesw5-dev figlet

en debian ejecutar:
sudo apt install gcc sox espeak ...

El código se debe compilar linkando con la librería ncurses:
gcc pitagoras.c -o pitagoras -lncurses
