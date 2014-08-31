#!/bin/bash
# This is the TarotClub Linux package generation script
# 
# It will generate two files:
#   - A tarball file containing the current TarotClub binaries architecture
#   - A Debian package (i386 or amd64)

# The script needs at least one argument, $# is the number of parameters
if [ $# -ne 1 ]; then
  echo "Please specify an architecture (i386 or amd64). Eg: make_packages.sh amd64"
  exit 1
fi

# Argument is  or amd64
if [ $1 == "i386" ] || [ $1 == "amd64" ]; then
  ARCH=$1
  echo "Using arch: ${ARCH}"
else
  echo "Invalid architecture, supported options are: i386 or amd64"
  exit 1
fi

# Various directory paths
TAROT_ROOT=$(pwd)/../..
RELEASE_ROOT=build-desktop/release
SERVER_RELEASE_ROOT=build-server/release
TEMP_DIR=tarotclub

# version is in defines.h file in the following format:
# #define TAROT_VERSION   "2.1.0-alpha.2" or "2.1.0"
VERSION=$(grep -Po '([-0-9]*\.[0-9]*\.[0-9]*(-[A-Za-z]*\.[0-9]*)?)' ${TAROT_ROOT}/src/Defines.h)

####################  TARBALL PACKAGE ###########################

echo "Creating TarotClub ${VERSION} tarball..."

# clean previous install
rm -rf ${TEMP_DIR}
rm *.bz2

# create directory tree
mkdir -p ${TEMP_DIR}/ai/tarotlib
mkdir -p ${TEMP_DIR}/doc
cd ${TEMP_DIR}

# copy files
cp ${TAROT_ROOT}/COPYING .
cp ${TAROT_ROOT}/HISTORY .
cp ${TAROT_ROOT}/README.md .
cp ${TAROT_ROOT}/${RELEASE_ROOT}/TarotClub .
cp ${TAROT_ROOT}/${SERVER_RELEASE_ROOT}/tcds .
cp -r ${TAROT_ROOT}/assets/cards/default/ .
cp ${TAROT_ROOT}/assets/fonts/kanzlei.ttf .
cp ${TAROT_ROOT}/assets/ai/tarotlib/system.js ./ai/tarotlib
cp ${TAROT_ROOT}/assets/ai/tarotlib/util.js ./ai/tarotlib
cp ${TAROT_ROOT}/assets/ai/tarotlib/card.js ./ai/tarotlib
cp ${TAROT_ROOT}/assets/ai/tarotlib/deck.js ./ai/tarotlib
cp ${TAROT_ROOT}/assets/ai/tarotlib/player.js ./ai/tarotlib
cp ${TAROT_ROOT}/assets/ai/tarotlib/bot.js ./ai/tarotlib
cp ${TAROT_ROOT}/assets/ai/tarotlib/game.js ./ai/tarotlib
cp ${TAROT_ROOT}/assets/ai/beginner.js ./ai
cp ${TAROT_ROOT}/assets/ai/conf.json ./ai
cp ${TAROT_ROOT}/assets/icons/icon256x256.png .
cp ${TAROT_ROOT}/prj/desktop/tarotclub_fr.qm .
cp ${TAROT_ROOT}/doc/index.html ./doc
cp ${TAROT_ROOT}/doc/rules_en.html ./doc
cp ${TAROT_ROOT}/doc/rules_fr.html ./doc
cp -r ${TAROT_ROOT}/doc/images/ ./doc

# copy install script
cp ${TAROT_ROOT}/pkg/ubuntu/install.sh .

echo "Tarball tree created."
cd ..
tar -cjvf tarotclub-${VERSION}.tar.bz2 ${TEMP_DIR}
echo "Tarball archive created."

####################  DEBIAN PACKAGE ###########################

# ---------------------------------
# Some directory definitions
# ---------------------------------
PACKAGE_ROOT="./deb_temp"
INSTALL_DIR="${PACKAGE_ROOT}/usr/share/tarotclub"
FONTS_DIR="${PACKAGE_ROOT}/usr/share/fonts"
DESKTOP_ENTRY="${PACKAGE_ROOT}/usr/share/applications"
DEBIAN_DIR="${PACKAGE_ROOT}/DEBIAN"

# ---------------------------------
# Control file contents
# ---------------------------------
CONTROL_FILE="Package: tarotclub\n"
CONTROL_FILE+="Version: ${VERSION}\n"
CONTROL_FILE+="Section: games\n"
CONTROL_FILE+="Priority: extra\n"
CONTROL_FILE+="Architecture: ${ARCH}\n"
CONTROL_FILE+="Depends: qt5-default (>= 5.2.0), libqt5svg5 (>= 5.2.0)\n"
CONTROL_FILE+="Installed-Size: 8312\n"
CONTROL_FILE+="Maintainer: Anthony Rabine <anthony.rabine@tarotclub.fr>\n"
CONTROL_FILE+="Homepage: http://www.tarotclub.fr\n"
CONTROL_FILE+="Description: TarotClub\n"
CONTROL_FILE+=" TarotClub is a french Tarot card game. Network and local games \n"
CONTROL_FILE+=" are available, full customisable SVG deck, deal editor.\n"
CONTROL_FILE+=" .\n"
CONTROL_FILE+=" Artificial intelligence is scriptable using JavaScript language.\n"


echo "Starting DEBIAN package..."

# clean previous install
rm -rf ${PACKAGE_ROOT}
rm *.deb

# generate control file, -e option allow to interpret escape characters
echo -e ${CONTROL_FILE} > control.txt

# creation of directories
mkdir -p ${INSTALL_DIR}
mkdir -p ${FONTS_DIR}
mkdir -p ${DEBIAN_DIR}
mkdir -p ${DESKTOP_ENTRY}

# move TarotClub where it must be installed
mv ${TEMP_DIR}/kanzlei.ttf ${FONTS_DIR} 
mv ${TEMP_DIR}/* ${INSTALL_DIR} 

# Debian package files
cp ./control.txt ${DEBIAN_DIR}/control
cp ./postinst ${DEBIAN_DIR}
cp ./tarotclub.desktop ${DESKTOP_ENTRY}

echo "Package tree created."
echo "Making all files root owner..."
sudo chown -R root:root ${TEMP_DIR}

dpkg-deb --build ${PACKAGE_ROOT}
echo "Ubuntu package created."
mv "${PACKAGE_ROOT}.deb" "tarotclub-${VERSION}_${ARCH}.deb"

