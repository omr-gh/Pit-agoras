#!/bin/bash

/usr/bin/clear
tput cup 5 0 # colocal el cursor en x y
tput setaf 2
figlet -c Bienvendio! gracias por
figlet -c usar Pitagoras
espeak -v mb-es2 -s140 bienvenido 2>/dev/null
espeak -v mb-es2 -s130 graciasporusarpitágoras 2>/dev/null
tput cup 25 7
echo "Escribe tu nombre"
tput cup 25 25
tput setaf 7
read name
tput cup 26 7
tput setaf 2
tput cup 27 7
printf "Suerte con la tabla "
tput setaf 7
printf $name!
tput setaf 2
printf " Presiona una tecla..."
#read -n 1 -p 
espeak -v mb-es2 -s110 suerte$name 2>/dev/null
espeak -v mb-es2 -s140 presionaunatecla 2>/dev/null
read -n 1
echo $name > name

