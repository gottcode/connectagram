/*
	SPDX-FileCopyrightText: 2013-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <QBuffer>
#include <QByteArray>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept>

//-----------------------------------------------------------------------------

std::map<QString, std::vector<QString>> readWords(const QString& filename)
{
	QFile in(filename);
	if (!in.open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw std::runtime_error("Unable to open file '" + filename.toStdString() + "' for reading.");
	}

	QTextStream stream(&in);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
	stream.setCodec("UTF-8");
#endif

	std::map<QString, std::vector<QString>> words;
	while (!stream.atEnd()) {
		const QString word = stream.readLine().trimmed();

		// Discard words that are too short
		if (word.length() < 5) {
			continue;
		}

		// Store unique spellings
		std::vector<QString>& spellings = words[word.toUpper()];
		if (std::find(spellings.cbegin(), spellings.cend(), word) == spellings.cend()) {
			spellings.push_back(word);
		}
	}

	in.close();

	return words;
}

//-----------------------------------------------------------------------------

QByteArray joinWordsIntoLines(const std::map<QString, std::vector<QString>>& words)
{
	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly | QIODevice::Text);

	QTextStream stream(&buffer);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
	stream.setCodec("UTF-8");
#endif

	for (const auto& i : words) {
		const QString& solution = i.first;
		const std::vector<QString>& spellings = i.second;
		if ((spellings.size() == 1) && (spellings.front() == solution.toLower())) {
			// Doesn't need to store proper spellings in this case;
			// Connectagram will fall back to all lowercase of solution.
			stream << solution << '\n';
		} else {
			// Store solution and proper spellings.
			stream << solution;
			for (const QString& spelling : spellings) {
				stream << ' ' << spelling;
			}
			stream << '\n';
		}
	}

	buffer.close();

	return buffer.data();
}

//-----------------------------------------------------------------------------

void writeLines(const QString& filename, const QByteArray& lines)
{
	QFile out(filename);
	if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		throw std::runtime_error("Unable to open file '" + filename.toStdString() + "' for writing.");
	}

	out.write(qCompress(lines, 9));

	out.close();
}

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	try {
		QCommandLineParser parser;
		parser.setApplicationDescription("Create Connectagram word list from a list of words.");
		parser.addHelpOption();
		parser.addOption(QCommandLineOption({"l", "language"}, "Specify <language>.", "language"));
		parser.addOption(QCommandLineOption({"o", "output"}, "Place words in <output> instead of default file.", "output"));
		parser.addPositionalArgument("file", "The <file> containing list of words.");
		parser.process(app);

		QString filename;
		const QStringList files = parser.positionalArguments();
		if (files.isEmpty()) {
			parser.showHelp();
		}
		if (files.size() > 1) {
			throw std::runtime_error("Multiple 'file' arguments specified.");
		}
		filename = files.first();

		QString language = filename.section('/', -1);
		if (parser.isSet("language")) {
			language = parser.value("language");
		}

		QString outfilename = "words";
		if (parser.isSet("output")) {
			outfilename = parser.value("output");
		}

		const std::map<QString, std::vector<QString>> words = readWords(filename);
		const QByteArray lines = joinWordsIntoLines(words);
		writeLines(outfilename, lines);
	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		return -1;
	}
}

//-----------------------------------------------------------------------------
