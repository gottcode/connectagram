/***********************************************************************
 *
 * Copyright (C) 2009, 2013 Graeme Gott <graeme@gottcode.org>
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

class WordList;

#include <QHash>
#include <QObject>
#include <QUrl>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QUrlQuery>
#endif
class QNetworkAccessManager;
class QNetworkReply;

class Dictionary : public QObject {
	Q_OBJECT

public:
	Dictionary(const WordList& wordlist, QObject* parent = 0);

	QUrl url() const {
		return m_url;
	}

	void setLanguage(const QString& langcode);

signals:
	void wordDefined(const QString& word, const QString& definition);

public slots:
	void lookup(const QString& word);
	void wait();

private slots:
	void lookupFinished(QNetworkReply* reply);

private:
	const WordList& m_wordlist;
	QUrl m_url;
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
	QUrlQuery m_query;
#endif
	QNetworkAccessManager* m_manager;
	QHash<QNetworkReply*, QString> m_words;
};

#endif
