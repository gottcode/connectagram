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

#include "dictionary.h"

#include "wordlist.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlStreamReader>

static const QByteArray USER_AGENT = "Connectagram/" + QByteArray(VERSIONSTR) + " (http://gottcode.org/connectagram/; Qt/" + qVersion() + ")";

Dictionary::Dictionary(const WordList& wordlist, QObject* parent)
: QObject(parent), m_wordlist(wordlist) {
	m_url.setScheme("http");
	m_url.setPath("/w/api.php");
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
	m_query.addQueryItem("format", "xml");
	m_query.addQueryItem("action", "mobileview");
	m_query.addQueryItem("sections", "all");
	m_query.addQueryItem("noimages", "");
#else
	m_url.addQueryItem("format", "xml");
	m_url.addQueryItem("action", "mobileview");
	m_url.addQueryItem("sections", "all");
	m_url.addQueryItem("noimages", "");
#endif
	setLanguage("en");

	m_manager = new QNetworkAccessManager(this);
	connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(lookupFinished(QNetworkReply*)));
}

//-----------------------------------------------------------------------------

void Dictionary::setLanguage(const QString& langcode) {
	m_url.setHost(langcode + ".wiktionary.org");
}

//-----------------------------------------------------------------------------

void Dictionary::lookup(const QString& word) {
	QNetworkRequest request;
	QUrl url = m_url;
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
	QUrlQuery query = m_query;
	query.addQueryItem("page", word.toLower());
	url.setQuery(query);
#else
	url.addQueryItem("page", word.toLower());
#endif
	request.setUrl(url);
	request.setRawHeader("User-Agent", USER_AGENT);

	QNetworkReply* reply = m_manager->get(request);
	m_words[reply] = word;
}

//-----------------------------------------------------------------------------

void Dictionary::wait() {
	QHashIterator<QNetworkReply*, QString> i(m_words);
	while (i.hasNext()) {
		i.key()->abort();
	}
}

//-----------------------------------------------------------------------------

void Dictionary::lookupFinished(QNetworkReply* reply) {
	// Find word
	QString word = m_words.value(reply);
	if (word.isEmpty()) {
		qWarning("Unknown lookup");
		reply->deleteLater();
		return;
	}
	m_words.remove(reply);

	// Fetch word definitions
	QString definition;
	if (reply->error() == QNetworkReply::NoError) {
		bool started = false;
		QXmlStreamReader xml(reply);
		while (!xml.atEnd()) {
			xml.readNext();
			if (!xml.isStartElement()) {
				continue;
			}

			if (xml.name() == "section") {
				int toclevel = xml.attributes().value("toclevel").toString().toInt();
				if (toclevel == 1) {
					if (!started) {
						started = true;
					} else {
						break;
					}
				} else if (toclevel == 2) {
					definition += xml.readElementText();
				}
			} else if (xml.name() == "error") {
				xml.raiseError();
			}
		}

		if (!xml.hasError()) {
			definition += "<p align=\"right\">" + tr("Definition from Wiktionary, the free dictionary") + "</p>";
		} else {
			definition = tr("No definition found");
		}
	} else {
		definition = tr("Unable to connect to Wiktionary");
	}
	emit wordDefined(word, definition);

	reply->deleteLater();
}
