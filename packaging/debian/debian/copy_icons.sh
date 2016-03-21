#!/bin/bash

mkdir -p ./icons/hicolor/scalable/apps
mkdir -p ./pixmaps
cp ./assets/icons/icon256x256.png ./pixmaps/tarotclub.png
cp ./assets/icons/club.svg ./icons/hicolor/scalable/apps/tarotclub.svg
for res in 16 32 48 128 256 512; do
	mkdir -p "./icons/hicolor/${res}x${res}/apps"
	cp "./assets/icons/icon${res}x${res}.png" "./icons/hicolor/${res}x${res}/apps/tarotclub.png"
done

