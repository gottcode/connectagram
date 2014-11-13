lessThan(QT_VERSION, 4.6) {
	error("Connectagram requires Qt 4.6 or greater")
}

TEMPLATE = app
QT += network
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}
CONFIG += warn_on

VERSION = 1.1.2
DEFINES += VERSIONSTR=\\\"$${VERSION}\\\"

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

TRANSLATIONS = $$files(translations/connectagram_*.ts)

RESOURCES = icons/icons.qrc
macx {
	ICON = icons/connectagram.icns

	GAME_DATA.files = data
	GAME_DATA.path = Contents/Resources

	QMAKE_BUNDLE_DATA += GAME_DATA
} else:win32 {
	RC_FILE = icons/icon.rc
} else:unix {
	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}
	isEmpty(BINDIR) {
		BINDIR = bin
	}

	target.path = $$PREFIX/$$BINDIR/

	data.files = data
	data.path = $$PREFIX/share/connectagram/

	pixmap.files = icons/connectagram.xpm
	pixmap.path = $$PREFIX/share/pixmaps/

	icon.files = icons/hicolor/*
	icon.path = $$PREFIX/share/icons/hicolor/

	desktop.files = icons/connectagram.desktop
	desktop.path = $$PREFIX/share/applications/

	appdata.files = icons/po/connectagram.appdata.xml
	appdata.path = $$PREFIX/share/appdata/

	qm.files = translations/*.qm
	qm.path = $$PREFIX/share/connectagram/translations

	man.files = doc/connectagram.6
	man.path = $$PREFIX/share/man/man6

	INSTALLS += target pixmap icon desktop appdata data qm man
}
