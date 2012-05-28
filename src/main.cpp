/***********************************************************************
 *
 * Copyright (C) 2009, 2012 Graeme Gott <graeme@gottcode.org>
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

#include "locale_dialog.h"
#include "window.h"

#include <QApplication>
#include <QDir>

int main(int argc, char** argv) {
	QApplication app(argc, argv);
	app.setApplicationName("Connectagram");
	app.setApplicationVersion(VERSIONSTR);
	app.setOrganizationDomain("gottcode.org");
	app.setOrganizationName("GottCode");

	LocaleDialog::loadTranslator("connectagram_");

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
