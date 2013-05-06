#!/bin/sh

# copy font into user's fonts directory
mkdir -p ~/.fonts
cp ./kanzlei.ttf ~/.fonts/

# make the new font available for all
fc-cache -f -v ~/.fonts
