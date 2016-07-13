#!/bin/bash
# This is the TarotClub Linux tarball package generation script

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
TAROT_ROOT=$(pwd)/../../tarotclub
RELEASE_ROOT=build-desktop/release
TEMP_DIR=tarotclub

# version is in defines.h file in the following format:
# #define TAROT_VERSION   "2.1-alpha.2" or "2.1"
VERSION=2.4

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
cp ../install.sh .

echo "Tarball tree created."
cd ..
tar -cjvf tarotclub-${VERSION}.tar.bz2 ${TEMP_DIR}
echo "Tarball archive created."


