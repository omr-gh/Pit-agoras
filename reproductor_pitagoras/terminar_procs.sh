#!/bin/bash

ps fax | grep reproducir_pista.sh | awk '{print $1}' > procs

while read linea
do
	kill -9 $linea
done < procs
