# SPDX-FileCopyrightText: 2008-2025 Thorsten Roth
# SPDX-License-Identifier: GPL-3.0-or-later

TEMPLATE      = app

unix: !macx {
       TARGET = hangman
} else {
       TARGET = Hangman
}

VERSION  = 4.2.0

QMAKE_TARGET_PRODUCT     = "Hangman"
QMAKE_TARGET_DESCRIPTION = "Hangman game framework written in C++ and Qt 6."
QMAKE_TARGET_COPYRIGHT   = "(C) 2008-present Thorsten Roth"

DEFINES      += APP_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\" \
                APP_VERSION=\"\\\"$$VERSION\\\"\" \
                APP_DESC=\"\\\"$$QMAKE_TARGET_DESCRIPTION\\\"\" \
                APP_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += core gui widgets
CONFIG       += c++17
DEFINES      += QT_NO_FOREACH

CONFIG(debug, debug|release) {
  CONFIG     += warn_on
  DEFINES    += QT_DISABLE_DEPRECATED_BEFORE=0x061000
}

SOURCES      += main.cpp\
                hangman.cpp \
                game.cpp

HEADERS      += hangman.h \
                game.h

FORMS        += hangman.ui

RESOURCES     = data/hangman_resources.qrc

TRANSLATIONS += lang/hangman_de.ts \
                lang/hangman_en.ts \
                lang/hangman_it.ts \
                lang/hangman_nl.ts

win32:RC_FILE = data/win/hangman_win.rc

macx {
  ICON               = data/mac/icon.icns
  QMAKE_INFO_PLIST   = data/mac/Info.plist
}

unix: !macx {
  isEmpty(PREFIX) {
    PREFIX = /usr/local
  }
  isEmpty(BINDIR) {
    BINDIR = bin
  }

  target.path    = $$PREFIX/$$BINDIR/

  desktop.path   = $$PREFIX/share/applications
  desktop.files += data/unix/com.github.elth0r0.hangman.desktop

  pixmap.path    = $$PREFIX/share/pixmaps
  pixmap.files  += data/icon.png \
                   data/unix/hangman.xpm

  meta.path      = $$PREFIX/share/metainfo
  meta.files    += data/unix/com.github.elth0r0.hangman.metainfo.xml

  INSTALLS      += target \
                   desktop \
                   pixmap \
                   meta
}
