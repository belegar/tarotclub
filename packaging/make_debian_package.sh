#!/bin/bash

# This is the TarotClub debian package generation script
# 
# It will generate two files:
#   - A tarball file containing the current TarotClub binaries architecture
#   - A Debian package (i386 or amd64)

# ------------------------------------------------------------------------------
# Usage of this script
# ------------------------------------------------------------------------------
# 1. Increase the package version to add a changelog:
# 
# At the same level than the dedian/ directory, run: 
# 
# dch -i
# 
# 2. Change the UNRELEASED to the suitable Ubuntu version
# 
# change: tarotclub (2.3.6-0ubuntu1) UNRELEASED; urgency=medium
# into: tarotclub (2.3.6-0ubuntu1) trusty; urgency=medium

# The script needs at least one argument, $# is the number of parameters
if [ $# -ne 2 ]; then
  echo "Please specify an application and package versions. Eg: make_debian_package.sh 2.4.3 2"
  exit 1
fi

# ------------------------------------------------------------------------------
# Get the version package from the Bitbucket depot
# ------------------------------------------------------------------------------

VERSION=$1
PACKAGE_VER=$2
HG_TAG=VERSION_${VERSION//./_}
ARCHIVE_NAME=tarotclub_${VERSION}.orig.tar.gz

if [ ! -f "./${ARCHIVE_NAME}" ]; then
	echo "Archive ${ARCHIVE_NAME} not found, get it."
	wget https://bitbucket.org/tarotclub/tarotclub/get/${HG_TAG}.tar.gz
	mv *.tar.gz ${ARCHIVE_NAME}
fi

# ------------------------------------------------------------------------------
# Change current directory to the debian root dir
# ------------------------------------------------------------------------------
cd ./debian

# Build the desktop file
DESKTOP_FILE="[Desktop Entry]\n"
DESKTOP_FILE+="Name=TarotClub\n"
DESKTOP_FILE+="Comment=Le jeu de Tarot libre!\n"
DESKTOP_FILE+="Exec=/usr/share/tarotclub/TarotClub\n"
DESKTOP_FILE+="Icon=tarotclub\n"
DESKTOP_FILE+="Path=/usr/share/tarotclub\n"
DESKTOP_FILE+="Categories=Game;CardGame;\n"
DESKTOP_FILE+="Terminal=false\n"
DESKTOP_FILE+="Type=Application\n"

# generate desktop file, -e option allow to interpret escape characters
echo -e ${DESKTOP_FILE} > ./debian/tarotclub.desktop

# ------------------------------------------------------------------------------
# Generate the debian script files to create the bin and src packages
# ------------------------------------------------------------------------------
debuild -S -sa

cd ..

# ------------------------------------------------------------------------------
# Try the package result (in /var/cache/pbuilder/result)
# ------------------------------------------------------------------------------
# Make sure you have created a chroot environment:
# sudo pbuilder create --distribution trusty
sudo pbuilder build tarotclub_${VERSION}-0ubuntu${PACKAGE_VER}.dsc

echo "Do you wish to upload on the PPA?"
select yn in "Yes" "No"; do
    case $yn in
        Yes ) dput ppa:tarotclub/ppa tarotclub_${VERSION}-0ubuntu${PACKAGE_VER}_source.changes;;
        No ) exit;;
    esac
done


