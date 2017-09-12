#!/bin/bash
directory='cat_cdms'
url='http://www.astro.uni-koeln.de/cdms/entries'
root='http://www.astro.uni-koeln.de/site/vorhersagen/catalog/'
prefix='c'
suffix='.cat'
xml='@(<.+?>)'
lines=($(wget -q -O - $url | grep -o -P "[\d]{6}<TD>.+?<TD"))
echo "${#lines[@]} links found"
if [ ! -d "$directory" ]
then
	mkdir "$directory"
fi
shopt -s extglob
i=9
while [ $i -lt 15 ] # ${#lines[@]} ]
do
	echo ${lines[i]}
	num="${lines[i]:0:6}"
	substance="${lines[i]:10}"
	substance="${substance%<TD}"
	substance="${substance//$xml}"
	substance="${substance//&#150;/-}"
	echo "$substance	$prefix$num$suffix"
	#wget -O "$directory/$prefix$num$suffix" "$root$prefix$num$suffix"
	i=$[$i+1]
done # > links
shopt -u extglob
echo "${#lines[@]} catalog files saved"

