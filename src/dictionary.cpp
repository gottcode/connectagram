/***********************************************************************
 *
 * Copyright (C) 2009-2020 Graeme Gott <graeme@gottcode.org>
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

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QTextStream>
#include <QXmlStreamReader>

static const QByteArray USER_AGENT = "Connectagram/" + QByteArray(VERSIONSTR) + " (https://gottcode.org/connectagram/; Qt/" + qVersion() + ")";

static const QDateTime BUILD_DATE = QLocale::c().toDateTime(QString("%1 %2").arg(__DATE__, __TIME__).simplified(), "MMM d yyyy hh:ss:mm");

Dictionary::Dictionary(const WordList* wordlist, QObject* parent)
: QObject(parent), m_wordlist(wordlist) {
	m_url.setScheme("https");
	m_url.setPath("/w/api.php");
	m_query.addQueryItem("format", "xml");
	m_query.addQueryItem("action", "mobileview");
	m_query.addQueryItem("sections", "all");
	m_query.addQueryItem("noimages", "");

	m_manager = new QNetworkAccessManager(this);
	connect(m_manager, &QNetworkAccessManager::finished, this, &Dictionary::lookupFinished);

	connect(m_wordlist, &WordList::languageChanged, this, &Dictionary::setLanguage);
}

//-----------------------------------------------------------------------------

void Dictionary::lookup(const QString& word) {
	// Check if word exists in cache and is recent
	QFileInfo info(m_cache_path + word);
	if (info.exists() && (info.lastModified() >= std::max(BUILD_DATE, QDateTime::currentDateTime().addDays(-14)))) {
		QFile file(info.absoluteFilePath());
		if (file.open(QFile::ReadOnly | QFile::Text)) {
			QTextStream stream(&file);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
			stream.setCodec("UTF-8");
#endif
			QString definition = stream.readAll();
			file.close();
			emit wordDefined(word, definition);
			return;
		}
	}

	// Look up word
	QStringList spellings = m_wordlist->spellings(word);
	for (const QString& spelling : spellings) {
		m_spellings[spelling] = word;

		QNetworkRequest request;
		QUrl url = m_url;
		QUrlQuery query = m_query;
		query.addQueryItem("page", spelling);
		url.setQuery(query);
		request.setUrl(url);
		request.setRawHeader("User-Agent", USER_AGENT);

		QNetworkReply* reply = m_manager->get(request);
		m_reply_details[reply] = spelling;
	}
}

//-----------------------------------------------------------------------------

void Dictionary::wait() {
	QHashIterator<QNetworkReply*, QString> i(m_reply_details);
	while (i.hasNext()) {
		i.key()->abort();
	}
}

//-----------------------------------------------------------------------------

void Dictionary::lookupFinished(QNetworkReply* reply) {
	// Find word
	QString word = m_reply_details.value(reply);
	if (!word.isEmpty()) {
		m_reply_details.remove(reply);
		word = m_spellings.take(word);
	}
	if (word.isEmpty()) {
		qWarning("Unknown lookup");
		reply->deleteLater();
		return;
	}

	// Determine if this is the last spelling of a word
	bool last_definition = m_spellings.key(word).isEmpty();

	// Fetch word definitions
	QString definition = m_definitions.value(word);
	if (reply->error() == QNetworkReply::NoError) {
		QXmlStreamReader xml(reply);
		while (!xml.atEnd()) {
			xml.readNext();
			if (!xml.isStartElement()) {
				continue;
			}

			if (xml.name() == QLatin1String("section")) {
				definition += xml.readElementText();
			} else if (xml.name() == QLatin1String("error")) {
				xml.raiseError();
			}
		}

		if (!xml.hasError()) {
			if (last_definition) {
				definition += "<p align=\"right\">" + tr("Definition from Wiktionary, the free dictionary") + "</p>";
				m_definitions.remove(word);
			} else {
				definition += "<hr>";
				m_definitions[word] = definition;
			}
		} else if (last_definition && definition.isEmpty()) {
			definition += tr("No definition found");
		}
	} else {
		definition = tr("Unable to connect to Wiktionary");
		last_definition = true;
	}

	// Show spelling if definition is done
	if (last_definition) {
		emit wordDefined(word, definition);

		// Save word to cache
		QFile file(m_cache_path + word);
		if (file.open(QFile::WriteOnly | QFile::Text)) {
			QTextStream stream(&file);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
			stream.setCodec("UTF-8");
#endif
			stream << definition;
			file.close();
		}
	}
	reply->deleteLater();
}

//-----------------------------------------------------------------------------

void Dictionary::setLanguage(const QString& langcode) {
	m_url.setHost(langcode + ".wiktionary.org");

	// Find cache path
	m_cache_path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
	m_cache_path += "/" + langcode + "/";

	// Create cache directory
	QDir dir(m_cache_path);
	dir.mkpath(dir.absolutePath());
}
