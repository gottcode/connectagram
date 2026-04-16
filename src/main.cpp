/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "locale_dialog.h"
#include "scores_dialog.h"
#include "window.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("Connectagram");
	app.setApplicationVersion(VERSIONSTR);
	app.setApplicationDisplayName(Window::tr("Connectagram"));
	app.setOrganizationDomain("gottcode.org");
	app.setOrganizationName("GottCode");
#if !defined(Q_OS_WIN) && !defined(Q_OS_MAC)
	app.setWindowIcon(QIcon::fromTheme("connectagram", QIcon(":/connectagram.png")));
	app.setDesktopFileName("connectagram");
#endif

	// Find application data
	const QString appdir = app.applicationDirPath();
	const QString datadir = QDir::cleanPath(appdir + "/" + CONNECTAGRAM_DATADIR);

	// Handle portability
#ifdef Q_OS_MAC
	const QFileInfo portable(appdir + "/../../../Data");
#else
	const QFileInfo portable(appdir + "/Data");
#endif
	if (portable.exists() && portable.isWritable()) {
		QSettings::setDefaultFormat(QSettings::IniFormat);
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, portable.absoluteFilePath() + "/Settings");
	}

	// Load application language
	LocaleDialog::loadTranslator("connectagram_", datadir);

	// Handle commandline
	QCommandLineParser parser;
	parser.setApplicationDescription(Window::tr("A word unscrambling game"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(app);

	// Find word lists
	QDir::setSearchPaths("connectagram", { datadir + "/gamedata/" });

	// Convert old scores to new format
	ScoresDialog::migrate();

	// Create main window
	Window window;

	return app.exec();
}
