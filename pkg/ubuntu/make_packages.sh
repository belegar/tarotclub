#!/bin/bash

# Edit TarotClub version

TAROT_ROOT=$(pwd)/../..
RELEASE_ROOT=build-TarotClub-Desktop_Qt_5_0_2_GCC_32bit-Release
TEMP_DIR=tarotclub

# version is in defines.h file in the following format:
# #define TAROT_VERSION   "2.1.0a1"
VERSION=$(grep -Po '([-0-9]*\.[0-9]*\.[0-9]*-[A-Za-z]*\.[0-9]*)' ${TAROT_ROOT}/src/defines.h)

####################  TARBALL PACKAGE ###########################

echo "Creating TarotClub ${VERSION} tarball..."

# clean previous install
rm -rf ${TEMP_DIR}
rm *.bz2

# create directory tree
mkdir ${TEMP_DIR}
mkdir ${TEMP_DIR}/ai
cd ${TEMP_DIR}

# copy files
cp -r ${TAROT_ROOT}/src/data/cards/default/ .
cp ${TAROT_ROOT}/${RELEASE_ROOT}/bin/TarotClub .
cp ${TAROT_ROOT}/src/data/fonts/kanzlei.ttf .
cp ${TAROT_ROOT}/BUILD .
cp ${TAROT_ROOT}/COPYING .
cp ${TAROT_ROOT}/COPYING-FR .
cp ${TAROT_ROOT}/HISTORY .
cp ${TAROT_ROOT}/INSTALL .
cp ${TAROT_ROOT}/README .
cp ${TAROT_ROOT}/doc/tarotclub.qch .
cp ${TAROT_ROOT}/doc/tarotclub.qhc .
cp ${TAROT_ROOT}/ai/beginner.js ./ai

# copy install script
cp ${TAROT_ROOT}/pkg/ubuntu/install.sh .

echo "Tarball tree created."
cd ..
tar -cjvf tarotclub-${VERSION}.tar.bz2 ${TEMP_DIR}
echo "Tarball archive created."

####################  DEBIAN PACKAGE ###########################

PACKAGE_ROOT="./deb_temp"
INSTALL_DIR="${PACKAGE_ROOT}/opt/tarotclub-${VERSION}"
FONTS_DIR="${PACKAGE_ROOT}/usr/local/share/fonts"
DEBIAN_DIR="${PACKAGE_ROOT}/DEBIAN"

echo "Starting DEBIAN package..."

# clean previous install
rm -rf ${PACKAGE_ROOT}
rm *.deb

# creation of directories
mkdir -p ${INSTALL_DIR}
mkdir -p ${FONTS_DIR}
mkdir -p ${DEBIAN_DIR}

# move TarotClub where it must be installed
mv ${TEMP_DIR}/kanzlei.ttf ${FONTS_DIR} 
mv ${TEMP_DIR}/* ${INSTALL_DIR}  

# Debian package files
cp ./control_i386.txt ${DEBIAN_DIR}/control
cp ./postinst ${DEBIAN_DIR}

echo "Package tree created."

dpkg-deb --build ${PACKAGE_ROOT}
echo "Ubuntu package created."
mv "${PACKAGE_ROOT}.deb" "tarotclub-${VERSION}_i386.deb"

