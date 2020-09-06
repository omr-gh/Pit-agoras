#!/bin/bash
#lee la informacion del archivo solicitado

while read pista
do
	$pista = pista
	
done < pista

canciones=('Bad Guy' Believer 'Billie Jean' "Ddu-du Ddu-Du Blackpink" "Rolling Sky"
	   Ghostbusters "Free Software Song" "Around The World" "Robot Rock"
	   "8 Bits" "Sandstorm")

ffmpeg -i musica_8bits/"${canciones[$pista]}".ogg -hide_banner > data 2>&1

#lee la linea duracion de data.txt y copia la info en time_info
awk 'NR==2' data | awk '{print $2}' > time_info
