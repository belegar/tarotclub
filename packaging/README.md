# Packaging scripts for TarotClub

## Introduction

This repository is the place holder for scripts to generate packages for various operating systems.

## Organization

Each operating system has a dedicated directory to store specific scripts.
Shared scripts or information between operating system is located in the shared directory.

# Ubuntu packaging

## Prerequisites

Tag the source code main line with the format VERSION_X_Y_Z

## Process 


  0. Install the packages

'''
sudo apt-get install devscripts
sudo apt-get install debhelper
sudo apt-get install pbuilder
'''

  1. Increase the package version to add a changelog:

At the same level than the dedian/ directory, run: 

'''
dch -i
'''

2. Change the UNRELEASED to the suitable Ubuntu version

change: tarotclub (2.3.6-0ubuntu1) UNRELEASED; urgency=medium
into: tarotclub (2.3.6-0ubuntu1) trusty; urgency=medium

# Arch package

## Install development tools

pacman -S qt5-base
pacman -S vim
pacman -S icu
pacman -S qt5-tools
 
## TODO

  * Share the desktop file with the debian package
  * Share the same installation directory tree than the debian package

# To verify a PKGBUILD

pacman -S namcap
namcap -i PKGBUILD

## Process

in package/arch directory, type:

makepkg -s

To make the AUR package: mkaurball

This command is available by installing the right package:

'''
pacman -S pkgbuild-introspection
'''

To install the package: 
pacman -U tarotclub-hg-1430.45896f4bd03b-1-any.pkg.tar.xz 

# Windows package

1. Edit the batch script to update the Mercurial TAG related to the version you want to package
2. Run the batch

# Fedora package

## Library install 

Make sure you have installed the RPM developer tools and the compiler.

'''
yum install qt5-qtbase-devel qt5-qtsvg-devel gcc-c++
yum install rpmdevtools yum-utils mercurial wget
yum groupinstall "Fedora Packager"
'''

## Environment setup

Get the Mercurial "packaging" depot under the HOME/Documents directory.

You have to create a fiel named .rpmmacros located at the root of your HOME directory. Copy the provided one:

cp Documents/rpmmacros ~/.rpmmacros

## Build the packages

In the SOURCES directory, create an archive of the sources to build by execting the provided script.
One argument is required: the version to get.

'''
get_sources.sh 2.5.1
'''

In the SPECS directory, build the packages:

'''
rpmbuild -ba tarotclub.spec
'''

## Install the package

rpm -i tarotclub-2.5.1-0.1.fc21.x86_64.rpm

