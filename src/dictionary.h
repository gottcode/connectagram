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

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QMutex>
#include <QThread>

class Dictionary : public QThread {
	Q_OBJECT

	public:
		Dictionary(QObject* parent = 0);

		void lookup(const QString& word);

	signals:
		void wordDefined(const QString& word, const QString& definition);

	protected:
		virtual void run();

	private:
		void downloadDefinition();
		void parseDefinition();

	private:
		QList<QString> m_words;
		QMutex m_word_mutex;
		QString m_word;
		QString m_definition;
		QString m_data;
};

#endif
