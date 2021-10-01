TEMPLATE = app
QT += widgets
CONFIG += c++17

CONFIG(debug, debug|release) {
	CONFIG += warn_on
	DEFINES += QT_DEPRECATED_WARNINGS
	DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060200
	DEFINES += QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
}

# Allow in-tree builds
MOC_DIR = ../build
OBJECTS_DIR = ../build
RCC_DIR = ../build

# Set program version
VERSION = 1.3.1
DEFINES += VERSIONSTR=\\\"$${VERSION}\\\"

# Set program name
unix: !macx {
	TARGET = connectagram
} else {
	TARGET = Connectagram
}
DESTDIR = ..

# Specify program sources
HEADERS = board.h \
	cell.h \
	clock.h \
	definitions.h \
	letter.h \
	locale_dialog.h \
	new_game_dialog.h \
	pattern.h \
	scores_dialog.h \
	view.h \
	window.h \
	word.h \
	wordlist.h

SOURCES = board.cpp \
	cell.cpp \
	clock.cpp \
	definitions.cpp \
	letter.cpp \
	locale_dialog.cpp \
	main.cpp \
	new_game_dialog.cpp \
	pattern.cpp \
	scores_dialog.cpp \
	view.cpp \
	window.cpp \
	word.cpp \
	wordlist.cpp

# Generate translations
TRANSLATIONS = $$files(../translations/connectagram_*.ts)
qtPrepareTool(LRELEASE, lrelease)
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$LRELEASE -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm

# Install program data
RESOURCES = ../icons/icons.qrc
macx {
	ICON = ../icons/connectagram.icns

	GAME_DATA.files = ../data
	GAME_DATA.path = Contents/Resources

	QMAKE_BUNDLE_DATA += GAME_DATA
} else:win32 {
	RC_ICONS = ../icons/connectagram.ico
	QMAKE_TARGET_DESCRIPTION = "Anagram game"
	QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2021 Graeme Gott"
} else:unix {
	RESOURCES += ../icons/icon.qrc

	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}
	isEmpty(BINDIR) {
		BINDIR = bin
	}

	target.path = $$PREFIX/$$BINDIR/

	data.files = ../data
	data.path = $$PREFIX/share/connectagram/

	icon.files = ../icons/hicolor/*
	icon.path = $$PREFIX/share/icons/hicolor/

	desktop.files = ../icons/connectagram.desktop
	desktop.path = $$PREFIX/share/applications/

	appdata.files = ../icons/connectagram.appdata.xml
	appdata.path = $$PREFIX/share/metainfo/

	qm.files = $$replace(TRANSLATIONS, .ts, .qm)
	qm.path = $$PREFIX/share/connectagram/translations
	qm.CONFIG += no_check_exist

	man.files = ../doc/connectagram.6
	man.path = $$PREFIX/share/man/man6

	INSTALLS += target icon desktop appdata data qm man
}
