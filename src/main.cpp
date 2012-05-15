/***********************************************************************
 *
 * Copyright (C) 2009 Graeme Gott <graeme@gottcode.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "window.h"

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QTranslator>

int main(int argc, char** argv) {
	QApplication app(argc, argv);
	app.setApplicationName("Connectagram");
	app.setApplicationVersion("1.0.1");
	app.setOrganizationDomain("gottcode.org");
	app.setOrganizationName("GottCode");

	QTranslator qt_translator;
	qt_translator.load("qt_" + QLocale::system().name());
	app.installTranslator(&qt_translator);

	QTranslator translator;
	translator.load(":/connectagram_" + QLocale::system().name());
	app.installTranslator(&translator);

#if defined(Q_OS_MAC)
	QDir::setCurrent(QCoreApplication::applicationDirPath() + "/../Resources/");
#elif defined(Q_OS_UNIX)
	QDir::setCurrent(QCoreApplication::applicationDirPath() + "/../share/connectagram/");
#elif defined(Q_OS_WIN32)
	QDir::setCurrent(QCoreApplication::applicationDirPath());
#endif
	Window window;

	return app.exec();
}
