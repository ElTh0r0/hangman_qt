#  This file is part of Hangman.
#  Copyright (C) 2008-2019 Thorsten Roth
#
#  Hangman is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Hangman is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Hangman.  If not, see <http://www.gnu.org/licenses/>.

lessThan(QT_MAJOR_VERSION, 5) {
  error("Hangman requires Qt 5.0 or greater")
}

TEMPLATE      = app

unix: !macx {
       TARGET = hangman
} else {
       TARGET = Hangman
}

VERSION       = 4.0.3
QMAKE_TARGET_PRODUCT     = "Hangman"
QMAKE_TARGET_DESCRIPTION = "Hangman game framework written in C++ and Qt 5."
QMAKE_TARGET_COPYRIGHT   = "(C) 2008-2019 Thorsten Roth"

DEFINES      += APP_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\" \
                APP_VERSION=\"\\\"$$VERSION\\\"\" \
                APP_DESC=\"\\\"$$QMAKE_TARGET_DESCRIPTION\\\"\" \
                APP_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += core gui widgets
CONFIG       += c++11

CONFIG(debug, debug|release) {
  CONFIG     += warn_on
  DEFINES    += QT_DEPRECATED_WARNINGS
  DEFINES    += QT_DISABLE_DEPRECATED_BEFORE=0x051300
}

SOURCES      += main.cpp\
                hangman.cpp \
                game.cpp

HEADERS      += hangman.h \
                game.h

FORMS        += hangman.ui

RESOURCES     = res/hangman_resources.qrc
win32:RC_FILE = res/hangman_win.rc

TRANSLATIONS += lang/hangman_de.ts \
                lang/hangman_nl.ts

macx {
  ICON               = res/icon.icns
  QMAKE_INFO_PLIST   = res/Info.plist
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
    desktop.files += data/hangman.desktop

    pixmap.path    = $$PREFIX/share/pixmaps
    pixmap.files  += res/images/hangman.png \
                     res/images/hangman.xpm

    INSTALLS      += target \
                     desktop \
                     pixmap
}
