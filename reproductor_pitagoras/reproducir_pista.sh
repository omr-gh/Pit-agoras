#!/bin/bash

#reproduce la pista seleccionada

while read pista
do
    pista=pista
done < pista

canciones=('Bad Guy' Believer 'Billie Jean' "Ddu-du Ddu-Du Blackpink" "Rolling Sky"
	   Ghostbusters "Free Software Song" "Around The World" "Robot Rock"
	   "8 Bits" "Sandstorm")

while [ 1 ]
do
    play -q musica_8bits/"${canciones[$pista]}".ogg
    ((pista=$pista+1))
    
    if [ $pista -gt 10 ]
    then
		pista=0
	fi
done
