TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}
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
	src/random.h \
	src/score_board.h \
	src/view.h \
	src/window.h \
	src/word.h \
	src/wordlist.h

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
	src/random.cpp \
	src/score_board.cpp \
	src/view.cpp \
	src/window.cpp \
	src/word.cpp \
	src/wordlist.cpp

TRANSLATIONS = translations/connectagram_en.ts \
	translations/connectagram_fr.ts \
	translations/connectagram_ro.ts

RESOURCES = icons/icons.qrc
macx {
	ICON = icons/connectagram.icns
}
win32 {
	RC_FILE = icons/icon.rc
}

macx {
	GAME_DATA.files = data
	GAME_DATA.path = Contents/Resources

	QMAKE_BUNDLE_DATA += GAME_DATA
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

	pixmap.files = icons/connectagram.xpm
	pixmap.path = $$PREFIX/share/pixmaps/

	icon.files = icons/icons/hicolor/*
	icon.path = $$PREFIX/share/icons/hicolor/

	desktop.files = icons/connectagram.desktop
	desktop.path = $$PREFIX/share/applications/

	qm.files = translations/*.qm
	qm.path = $$PREFIX/share/connectagram/translations

	INSTALLS += target pixmap icon desktop data qm
}
