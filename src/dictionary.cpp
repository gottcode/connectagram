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

#include "dictionary.h"

#include <QRegExp>
#include <QStringList>
#include <QTcpSocket>

namespace {
	enum Type {
		Status,
		Name,
		Definition,
		Done
	};
}

//-----------------------------------------------------------------------------

Dictionary::Dictionary(QObject* parent)
: QThread(parent) {
}

//-----------------------------------------------------------------------------

void Dictionary::lookup(const QString& word) {
	m_word_mutex.lock();
	m_words.append(word);
	m_word_mutex.unlock();
	if (!isRunning()) {
		start();
	}
}

//-----------------------------------------------------------------------------

void Dictionary::run() {
	m_word_mutex.lock();
	while (!m_words.isEmpty()) {
		m_word = m_words.takeFirst().toUpper();
		m_word_mutex.unlock();

		downloadDefinition();
		parseDefinition();
		emit wordDefined(m_word, m_definition);

		m_word_mutex.lock();
	}
	m_word_mutex.unlock();
}

//-----------------------------------------------------------------------------

void Dictionary::downloadDefinition() {
	QTcpSocket socket;
	socket.connectToHost("www.dict.org", 2628);
	if (!socket.waitForConnected(-1)) {
		m_definition = tr("Unable to connect to www.dict.org");
		return;
	}

	socket.waitForReadyRead(-1);
	socket.readAll();

	socket.write(QString("DEFINE wn \"%1\"\n").arg(m_word).toUtf8());
	socket.flush();

	m_data.clear();
	do {
		socket.waitForReadyRead(-1);
		m_data += socket.readAll();
	} while (!m_data.contains("250") && !m_data.contains("552"));

	socket.disconnectFromHost();
}

//-----------------------------------------------------------------------------

void Dictionary::parseDefinition() {
	if (m_data.isEmpty()) {
		return;
	}

	Type type = Status;
	QRegExp part_of_speech("(n|v|adj|adv) 1?: .*");
	QRegExp number("(\\d*:) .*");
	int definitions = 0;
	bool first_line = false;

	m_definition.clear();
	QStringList lines = m_data.split('\n', QString::SkipEmptyParts);
	for (int i = 0; i < lines.count(); ++i) {
		QString line = lines.at(i).simplified();

		// Parse status
		if (type == Status) {

			QString status = line.left(3);
			if (status == "150") {
			} else if (status == "151") {
				type = Name;
			} else if (status == "250") {
				type = Done;
			} else if (status == "550") {
				type = Done;
			} else if (status == "552") {
				m_definition = tr("No definition found");
				type = Done;
			}

		// Parse name
		} else if (type == Name) {

			m_definition += "<p class=\"title\">" + line;
			type = Definition;
			first_line = true;

		} else if (type == Definition) {

			// Parse part of speech
			if (part_of_speech.exactMatch(line)) {
				QString cap = part_of_speech.cap(1);
				line.remove(0, cap.length() + 1);
				if (cap == "n") {
					cap = "noun";
				} else if (cap == "v") {
					cap = "verb";
				} else if (cap == "adj") {
					cap = "adjective";
				} else if (cap == "adv") {
					cap = "adverb";
				}
				definitions = 0;
				m_definition += "</p>\n<p class=\"part\"><b><i>" + cap + "</i></b></p>\n<p>";
				first_line = false;
			} else if (first_line) {
				m_definition += "</p>\n<p>";
				first_line = false;
			}

			// Parse numbered definition
			if (number.exactMatch(line)) {
				QString cap = number.cap(1);
				line.remove(0, cap.length() + 1);
				if (cap.left(cap.length() - 1).toInt() > 0) {
					if (definitions > 0) {
						m_definition += "</p>\n";
					}
					definitions++;
					line.prepend("<p><b>" + cap + "</b> ");
				}
			}

			// Parse end of defintion
			if (line == ".") {
				m_definition += "</p>\n";
				type = Status;
				continue;
			}

			// Add definition text
			m_definition += line + " ";

		}

	}

	// Parse word links
	m_definition.replace(QRegExp("\\{([^\\}]+)\\}"), "<i>\\1</i>");

	// Remove extra spaces
	m_definition.replace(" </p>", "</p>");
}
