#!/bin/bash

# Edit TarotClub version
tarotclub_title=$(cat ./version.txt)

# clean previous install
rm -rf $tarotclub_title
mkdir $tarotclub_title
cd $tarotclub_title

# copy files
cp -r ../../src/data/cards/default/ .
cp ../../prj/bin/TarotClub .
cp ../../src/data/fonts/kanzlei.ttf .
cp ../../BUILD .
cp ../../COPYING .
cp ../../COPYING-FR .
cp ../../HISTORY .
cp ../../INSTALL .
cp ../../README .

echo "Package tree created."
cd ..
tar -cjvf ${tarotclub_title}.tar.bz2 $tarotclub_title
echo "Tarball archive created."



