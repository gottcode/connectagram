TEMPLATE = app
QT += network
CONFIG += warn_on
macx {
	CONFIG += x86_64
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
	src/locale_dialog.h \
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
	src/locale_dialog.cpp \
	src/main.cpp \
	src/new_game_dialog.cpp \
	src/pattern.cpp \
	src/score_board.cpp \
	src/view.cpp \
	src/window.cpp \
	src/word.cpp

TRANSLATIONS = translations/connectagram_en.ts

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
	isEmpty(BINDIR) {
		BINDIR = bin
	}

	target.path = $$PREFIX/$$BINDIR/

	data.files = share/connectagram/data
	data.path = $$PREFIX/share/connectagram/

	icon.files = icons/connectagram.png
	icon.path = $$PREFIX/share/icons/hicolor/48x48/apps

	desktop.files = icons/connectagram.desktop
	desktop.path = $$PREFIX/share/applications/

	qm.files = translations/*.qm
	qm.path = $$PREFIX/share/focuswriter/translations

	INSTALLS += target icon desktop data qm
}
