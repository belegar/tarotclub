#!/bin/bash

# Edit TarotClub version
tarotclub_title=$(cat ./version.txt)
install_dir="./opt/${tarotclub_title}"
fonts_dir="./usr/local/share/fonts"

# clean previous install
rm -rf $tarotclub_title
mkdir $tarotclub_title
cd $tarotclub_title
mkdir -p $install_dir
mkdir -p $fonts_dir

# copy files
cp -r ../../src/data/cards/default/ $install_dir
cp ../../prj/bin/TarotClub $install_dir
cp ../../src/data/fonts/kanzlei.ttf $fonts_dir
cp ../../BUILD $install_dir
cp ../../COPYING $install_dir
cp ../../COPYING-FR $install_dir
cp ../../HISTORY $install_dir
cp ../../INSTALL $install_dir
cp ../../README $install_dir

# Ubuntu package files
mkdir "DEBIAN"
cp ../control_i386.txt ./DEBIAN/control
cp ../postinst ./DEBIAN
cd ..

echo "Package tree created."

dpkg-deb --build $tarotclub_title
echo "Ubuntu package created."
mv "${tarotclub_title}.deb" "${tarotclub_title}_i386.deb"


