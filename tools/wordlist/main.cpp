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

#include <functional>
#include <iostream>
#include <stdexcept>

//-----------------------------------------------------------------------------

QString generateSolutionHe(QString string)
{
	string.replace(u'ך', u'כ');
	string.replace(u'ם', u'מ');
	string.replace(u'ן', u'נ');
	string.replace(u'ף', u'פ');
	string.replace(u'ץ', u'צ');
	return string;
}

QString generateSolutionNone(QString string)
{
	return string;
}

std::function<QString(QString)> fetchGenerateSolution(const QString& language)
{
	if (language == "he") {
		return generateSolutionHe;
	} else {
		return generateSolutionNone;
	}
}

//-----------------------------------------------------------------------------

QMap<QString, QStringList> readWords(const QString& filename, const QString& language)
{
	QFile in(filename);
	if (!in.open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw std::runtime_error("Unable to open file '" + filename.toStdString() + "' for reading.");
	}

	const auto generateSolution = fetchGenerateSolution(language);

	QTextStream stream(&in);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
	stream.setCodec("UTF-8");
#endif

	QMap<QString, QStringList> words;
	while (!stream.atEnd()) {
		const QString word = stream.readLine().trimmed();

		// Replace letters
		const QString solution = generateSolution(word.toUpper());

		// Discard words that are too short
		if (solution.length() < 5) {
			continue;
		}

		// Store unique spellings
		QStringList& spellings = words[solution];
		if (!spellings.contains(word)) {
			spellings += word;
		}
	}

	in.close();

	return words;
}

//-----------------------------------------------------------------------------

QByteArray joinWordsIntoLines(const QMap<QString, QStringList>& words)
{
	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly | QIODevice::Text);

	QTextStream stream(&buffer);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
	stream.setCodec("UTF-8");
#endif

	QMapIterator<QString, QStringList> i(words);
	while (i.hasNext()) {
		i.next();
		const QString& solution = i.key();
		const QStringList& spellings = i.value();
		if ((spellings.size() == 1) && (spellings.first() == solution.toLower())) {
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

		const QMap<QString, QStringList> words = readWords(filename, language);
		const QByteArray lines = joinWordsIntoLines(words);
		writeLines(outfilename, lines);
	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		return -1;
	}
}

//-----------------------------------------------------------------------------
