/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "locale_dialog.h"
#include "scores_dialog.h"
#include "window.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>

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

	QString path = app.applicationDirPath();
	QStringList paths;
	paths.append(path + "/data/");
	paths.append(path + "/../share/connectagram/data/");
	paths.append(path + "/../Resources/data/");
	QDir::setSearchPaths("connectagram", paths);

	LocaleDialog::loadTranslator("connectagram_");

	ScoresDialog::migrate();

	QCommandLineParser parser;
	parser.setApplicationDescription(Window::tr("A word unscrambling game"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(app);

	Window window;

	return app.exec();
}
