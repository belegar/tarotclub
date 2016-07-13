Name:           tarotclub
Version:	2.6.2        
Release:        0.2%{?dist}
Summary:        A local/network French Tarot card game
Group:		Amusements/Games
License:        GPLv3
URL:            http://www.tarotclub.fr
Source0:        %{name}_%{version}.tar.gz
Source1:	tarotclub.desktop
Source2:	tarotclub.sh
BuildRequires:  qt5-qtsvg-devel, qt5-qtbase-devel
Requires:	qt5-qtbase, qt5-qtsvg

%description
TarotClub is a Tarot card game (not the divination one) playable
on any computer with Mac OS X, Windows or Linux. The Tarot card game
is mostly played in french-spoken countries. It uses a specific deck
of cards including standard card figures plus the Knight and 22 trumps.
The complete rules of the Tarot card game is available in Wikipedia.
.
This project is a tentative to create a nice open source version of this game,
playable in various operating system and with a modular design.

%prep
%setup -q -n tarotclub-tarotclub-e3ca5ac0a85f

%build
qmake-qt5 ./prj/desktop.pro
make
qmake-qt5 ./prj/botclient.pro
make

%clean
rm -rf $RPM_BUILD_ROOT

%install

## EXECUTABLE AND RELATED FILES ##

install -Dm644 ./COPYING ${RPM_BUILD_ROOT}/usr/share/tarotclub/COPYING
install -Dm644 ./HISTORY.md ${RPM_BUILD_ROOT}/usr/share/tarotclub/HISTORY.md
install -Dm644 ./README.md ${RPM_BUILD_ROOT}/usr/share/tarotclub/README.md
install -Dm755 ./build-desktop/release/TarotClub ${RPM_BUILD_ROOT}/usr/share/tarotclub
install -Dm755 ./build-botclient/release/botclient ${RPM_BUILD_ROOT}/usr/share/tarotclub
install -Dm644 ./prj/desktop/tarotclub_fr.qm ${RPM_BUILD_ROOT}/usr/share/tarotclub/tarotclub_fr.qm

## CARDS ##
cp -r ./assets/cards/default/ ${RPM_BUILD_ROOT}/usr/share/tarotclub/
chmod 644 ${RPM_BUILD_ROOT}/usr/share/tarotclub/default/*

## AI JAVASCRIPT FILES ##

install -d ${RPM_BUILD_ROOT}/usr/share/tarotclub/ai/tarotlib/
cp -a ./assets/ai/tarotlib/{util,card,deck,player,bot,game}.js ${RPM_BUILD_ROOT}/usr/share/tarotclub/ai/tarotlib/
chmod 644 ${RPM_BUILD_ROOT}/usr/share/tarotclub/ai/tarotlib/*
install -Dm644 ./assets/ai/main.js ${RPM_BUILD_ROOT}/usr/share/tarotclub/ai/main.js
install -Dm644 ./assets/ai/package.json ${RPM_BUILD_ROOT}/usr/share/tarotclub/ai/package.json

## ICONS ##
install -Dm644 ./assets/icons/icon256x256.png ${RPM_BUILD_ROOT}/usr/share/pixmaps/tarotclub.png
install -Dm644 ./assets/icons/club.svg ${RPM_BUILD_ROOT}/usr/share/icons/hicolor/scalable/apps/tarotclub.svg
for res in 16 32 48 128 256 512; do
install -Dm644 "./assets/icons/icon${res}x${res}.png" \
             "${RPM_BUILD_ROOT}/usr/share/icons/hicolor/${res}x${res}/apps/tarotclub.png"
done

## DOCUMENTATION ##

install -Dm644 ./assets/doc/index.html           ${RPM_BUILD_ROOT}/usr/share/tarotclub/doc/index.html
install -Dm644 ./assets/doc/rules_en.html        ${RPM_BUILD_ROOT}/usr/share/tarotclub/doc/rules_en.html
install -Dm644 ./assets/doc/rules_fr.html        ${RPM_BUILD_ROOT}/usr/share/tarotclub/doc/rules_fr.html
cp -r ./assets/doc/images/                       ${RPM_BUILD_ROOT}/usr/share/tarotclub/doc
chmod 644 ${RPM_BUILD_ROOT}/usr/share/tarotclub/doc/images/*

## MISC FILES LOCATED AT THE PACKAGE ROOT ##

install -Dm644 %{SOURCE1} ${RPM_BUILD_ROOT}/usr/share/applications/tarotclub.desktop  
install -Dm755 %{SOURCE2} ${RPM_BUILD_ROOT}/usr/bin/tarotclub.sh

%files
%defattr(-,root,root)

%{_bindir}/tarotclub.sh
%{_datadir}/applications/tarotclub.desktop
%{_datadir}/tarotclub/*
%{_datadir}/icons/hicolor/128x128/apps/tarotclub.png
%{_datadir}/icons/hicolor/16x16/apps/tarotclub.png
%{_datadir}/icons/hicolor/256x256/apps/tarotclub.png
%{_datadir}/icons/hicolor/32x32/apps/tarotclub.png
%{_datadir}/icons/hicolor/48x48/apps/tarotclub.png
%{_datadir}/icons/hicolor/512x512/apps/tarotclub.png
%{_datadir}/icons/hicolor/scalable/apps/tarotclub.svg
%{_datadir}/pixmaps/tarotclub.png

%doc


%changelog
* Sat Jan  3 2015 Anthony Rabine
- First SPEC file version for FC21

