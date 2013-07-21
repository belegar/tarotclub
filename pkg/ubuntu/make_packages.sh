#!/bin/bash
# This is the main TarotClub install script
# It will generate two files:
#   - A tarball file containing the current TarotClub binaries architecture
#   - A Debian package (i386 or amd64)

# The script needs at least one argument, $# is the number of parameters
if [ $# -ne 1 ]; then
  echo "Please specify an architecture (i386 or amd64), eg: make_packages.sh amd64"
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

# Edit TarotClub version

TAROT_ROOT=$(pwd)/../..

if [ ${ARCH} == "i386" ]; then
  RELEASE_ROOT=build-TarotClub-Desktop_Qt_5_0_2_GCC_32bit-Release
else
  RELEASE_ROOT=build-TarotClub-Desktop_Qt_5_0_2_GCC_64bit-Release
fi

TEMP_DIR=tarotclub

# version is in defines.h file in the following format:
# #define TAROT_VERSION   "2.1.0-alpha.2"
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
cp ${TAROT_ROOT}/lib/icon256x256.png .
cp ${TAROT_ROOT}/lib/tarotclub.xsl .
cp ${TAROT_ROOT}/prj/tarotclub_en.qm .
cp ${TAROT_ROOT}/prj/tarotclub_fr.qm .

# copy install script
cp ${TAROT_ROOT}/pkg/ubuntu/install.sh .

echo "Tarball tree created."
cd ..
tar -cjvf tarotclub-${VERSION}.tar.bz2 ${TEMP_DIR}
echo "Tarball archive created."

####################  DEBIAN PACKAGE ###########################

PACKAGE_ROOT="./deb_temp"
INSTALL_DIR="${PACKAGE_ROOT}/usr/share/tarotclub"
FONTS_DIR="${PACKAGE_ROOT}/usr/share/fonts"
DESKTOP_ENTRY="${PACKAGE_ROOT}/usr/share/applications"
MENU_ENTRY="${PACKAGE_ROOT}/usr/share/menu"
DEBIAN_DIR="${PACKAGE_ROOT}/DEBIAN"

echo "Starting DEBIAN package..."

# clean previous install
rm -rf ${PACKAGE_ROOT}
rm *.deb

# creation of directories
mkdir -p ${INSTALL_DIR}
mkdir -p ${FONTS_DIR}
mkdir -p ${DEBIAN_DIR}
mkdir -p ${DESKTOP_ENTRY}
mkdir -p ${MENU_ENTRY}

# move TarotClub where it must be installed
mv ${TEMP_DIR}/kanzlei.ttf ${FONTS_DIR} 
mv ${TEMP_DIR}/* ${INSTALL_DIR}  

# Debian package files
cp ./control_${ARCH}.txt ${DEBIAN_DIR}/control
cp ./postinst ${DEBIAN_DIR}
cp ./tarotclub.desktop ${DESKTOP_ENTRY}
cp ./tarotclub.menu ${MENU_ENTRY}

echo "Package tree created."

dpkg-deb --build ${PACKAGE_ROOT}
echo "Ubuntu package created."
mv "${PACKAGE_ROOT}.deb" "tarotclub-${VERSION}_${ARCH}.deb"

