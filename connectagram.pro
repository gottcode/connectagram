TEMPLATE = app
QT += network
CONFIG += warn_on release
macx {
	# Uncomment the following line to compile on PowerPC Macs
	# QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
	CONFIG += x86 ppc
}

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

VERSION = $$system(git rev-parse --short HEAD)
isEmpty(VERSION) {
	VERSION = 0
}
DEFINES += VERSIONSTR=\\\"git.$${VERSION}\\\"

unix: !macx {
	DESTDIR = bin
	TARGET = connectagram
} else {
	TARGET = Connectagram
}

HEADERS = src/board.h \
	src/cell.h \
	src/clock.h \
	src/definitions.h \
	src/dictionary.h \
	src/letter.h \
	src/new_game_dialog.h \
	src/pattern.h \
	src/score_board.h \
	src/view.h \
	src/window.h \
	src/word.h

SOURCES = src/board.cpp \
	src/cell.cpp \
	src/clock.cpp \
	src/definitions.cpp \
	src/dictionary.cpp \
	src/letter.cpp \
	src/main.cpp \
	src/new_game_dialog.cpp \
	src/pattern.cpp \
	src/score_board.cpp \
	src/view.cpp \
	src/window.cpp \
	src/word.cpp

RESOURCES = icons/icons.qrc
macx {
	ICON = icons/connectagram.icns
}
win32 {
	RC_FILE = icons/icon.rc
}

unix: !macx {
	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}

	target.path = $$PREFIX/bin/

	definitions.files = share/connectagram/words
	definitions.path = $$PREFIX/share/connectagram/

	icon.files = icons/connectagram.png
	icon.path = $$PREFIX/share/icons/hicolor/48x48/apps

	desktop.files = icons/connectagram.desktop
	desktop.path = $$PREFIX/share/applications/

	INSTALLS += target definitions icon desktop
}
