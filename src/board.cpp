/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "board.h"

#include "cell.h"
#include "letter.h"
#include "pattern.h"
#include "word.h"

#include <QLocale>
#include <QSettings>
#include <QTimer>

//-----------------------------------------------------------------------------

constexpr int CONNECTAGRAM_GAME_VERSION = 5;

//-----------------------------------------------------------------------------

Board::Board(QObject* parent)
	: QGraphicsScene(parent)
	, m_pattern(nullptr)
	, m_current_word(nullptr)
	, m_hint(nullptr)
	, m_finished(true)
	, m_paused(false)
{
	QTimer* auto_save = new QTimer(this);
	auto_save->setInterval(30000);
	connect(auto_save, &QTimer::timeout, this, &Board::saveGame);
	auto_save->start();

	m_wordlist = new WordList(this);

	setBackgroundBrush(Qt::white);
}

//-----------------------------------------------------------------------------

Board::~Board()
{
	cleanUp();
}

//-----------------------------------------------------------------------------

void Board::check(const QString& original_word, const QString& current_word)
{
	Q_EMIT wordSolved(original_word, current_word);
	m_finished = true;
	for (Word* word : std::as_const(m_words)) {
		m_finished &= word->isCorrect();
	}
	if (m_finished) {
		QSettings().remove("Current/Words");
		Q_EMIT finished();
	}
}

//-----------------------------------------------------------------------------

void Board::click(const QString& word)
{
	setPaused(true);
	Q_EMIT wordSelected(word);
}

//-----------------------------------------------------------------------------

void Board::setCurrentWord(Word* word)
{
	if (m_current_word) {
		m_current_word->setHighlight(false);
	}
	m_current_word = !m_paused ? word : nullptr;
	if (m_current_word) {
		m_current_word->setHighlight(true);
	}
	delete m_hint;
	m_hint = nullptr;
	Q_EMIT hintAvailable(m_current_word);
}

//-----------------------------------------------------------------------------

QString Board::gameNumber() const
{
	return QString("%1%2%3%4%5%6")
			.arg(CONNECTAGRAM_GAME_VERSION)
			.arg(m_wordlist->language())
			.arg(m_pattern->type())
			.arg(m_pattern->wordCount())
			.arg(m_pattern->wordLength() - 4, 2, 16, QChar('0'))
			.arg(m_pattern->seed(), 0, 16);
}

//-----------------------------------------------------------------------------

bool Board::continueGame()
{
	QSettings settings;

	if (settings.value("Current/Version").toInt() != CONNECTAGRAM_GAME_VERSION) {
		settings.remove("Current");
		return false;
	}

	settings.beginGroup("Current");
	const QString language = settings.value("Language", WordList::defaultLanguage()).toString();
	const int pattern = settings.value("Pattern").toInt();
	const int count = settings.value("Count").toInt();
	const int length = settings.value("Length").toInt();
	const unsigned int seed = settings.value("Seed").toUInt();

	generate(language, pattern, count, length, seed);

	return true;
}

//-----------------------------------------------------------------------------

void Board::newGame(const QString& language, int pattern, int count, int length, unsigned int seed)
{
	QSettings settings;
	settings.remove("Current");
	settings.beginGroup("Current");
	settings.setValue("Version", CONNECTAGRAM_GAME_VERSION);
	settings.setValue("Language", language);
	settings.setValue("Pattern", pattern);
	settings.setValue("Count", count);
	settings.setValue("Length", length);
	settings.setValue("Seed", seed);
	settings.setValue("Time", 0);

	generate(language, pattern, count, length, seed);
}

//-----------------------------------------------------------------------------

bool Board::openGame(const QString& number)
{
	if (!number.startsWith(QString::number(CONNECTAGRAM_GAME_VERSION))) {
		return false;
	}

	// Parse language
	int index = 1;
	for (int i = 1; i < number.length(); ++i) {
		if (!number.at(i).isDigit()) {
			index = i;
		} else {
			break;
		}
	}
	QString language = WordList::defaultLanguage();
	if (index > 1) {
		language = number.mid(1, index);
		index += 1;
	}

	// Parse type
	if (index == number.length()) {
		return false;
	}
	int pattern = number.at(index).digitValue();

	// Parse count
	if (++index == number.length()) {
		return false;
	}
	int count = number.at(index).digitValue();

	// Parse length
	if (++index == number.length()) {
		return false;
	}
	bool ok = false;
	int length = number.mid(index, 2).toInt(&ok, 16) + 5;
	if (!ok) {
		return false;
	}

	// Parse seed
	if ((index += 2) == number.length()) {
		return false;
	}
	unsigned int seed = number.mid(index).toUInt(&ok, 16);
	if (!ok) {
		return false;
	}

	// Start game
	newGame(language, pattern, count, length, seed);

	return true;
}

//-----------------------------------------------------------------------------

void Board::saveGame()
{
	if (!m_finished && !m_words.isEmpty()) {
		QStringList words;
		for (Word* word : std::as_const(m_words)) {
			words.append(word->toString());
		}
		QSettings().setValue("Current/Words", words);
	}
}

//-----------------------------------------------------------------------------

void Board::showHint()
{
	if (m_current_word && !mouseGrabberItem()) {
		delete m_hint;
		m_hint = m_current_word->hint();
	}
}

//-----------------------------------------------------------------------------

void Board::setDarkMode(bool enabled)
{
	setBackgroundBrush(enabled ? QColor(0x26, 0x26, 0x26) : Qt::white);
	QSettings().setValue("DarkMode", enabled);
}

//-----------------------------------------------------------------------------

void Board::setPaused(bool paused)
{
	if (m_finished || (paused == m_paused)) {
		return;
	}

	m_paused = paused;

	for (const QList<Cell*>& cells : std::as_const(m_cells)) {
		for (const Cell* cell : cells) {
			if (cell) {
				cell->letter()->setPaused(m_paused);
			}
		}
	}

	if (m_paused) {
		setCurrentWord(nullptr);
	}

	Q_EMIT pauseChanged();
}

//-----------------------------------------------------------------------------

void Board::togglePaused()
{
	setPaused(!m_paused);
}

//-----------------------------------------------------------------------------

void Board::patternGenerated()
{
	m_words = m_pattern->solution();
	QSize size = m_pattern->size();
	setSceneRect(0, 0, size.width() * 34 + 2, size.height() * 34 + 34);

	for (int x = 0; x < size.width(); ++x) {
		QList<Cell*> cells;
		for (int y = 0; y < size.height(); ++y) {
			cells.append(nullptr);
		}
		m_cells.append(cells);
	}

	for (Word* word : std::as_const(m_words)) {
		word->setBoard(this);
		QList<QPoint> positions = word->positions();
		for (int i = 0; i < positions.count(); ++i) {
			const QPoint& pos = positions.at(i);
			Cell* cell = m_cells.at(pos.x()).at(pos.y());
			if (!cell) {
				Letter* letter = new Letter(word->at(i), this);
				addItem(letter);

				cell = new Cell(pos, letter);
				cell->setWord(word);
				m_cells[pos.x()][pos.y()] = cell;
			} else {
				cell->letter()->setJoin();
				cell->setWord(nullptr);
			}
		}
	}

	for (Word* word : std::as_const(m_words)) {
		Q_EMIT wordAdded(word->toString());
		word->shuffle(m_pattern->words());
	}

	QStringList previous = QSettings().value("Current/Words").toStringList();
	if (previous.count() == m_words.count()) {
		for (int i = 0; i < m_words.count(); ++i) {
			m_words[i]->fromString(previous.at(i));
		}
	}
	saveGame();

	Q_EMIT started();
}

//-----------------------------------------------------------------------------

void Board::generate(const QString& language, int pattern, int count, int length, unsigned int seed)
{
	cleanUp();
	Q_EMIT loading();

	m_wordlist->setLanguage(language);
	m_pattern = Pattern::create(m_wordlist, pattern);
	m_pattern->setCount(count);
	m_pattern->setLength(length);
	m_pattern->setSeed(seed);

	connect(m_pattern, &Pattern::generated, this, &Board::patternGenerated);
	m_pattern->start();
}

//-----------------------------------------------------------------------------

void Board::cleanUp()
{
	delete m_pattern;
	m_pattern = nullptr;
	clear();
	for (int i = 0; i < m_cells.count(); ++i) {
		qDeleteAll(m_cells[i]);
	}
	m_cells.clear();
	m_words.clear();
	m_current_word = nullptr;
	m_hint = nullptr;
	m_finished = false;
	m_paused = false;
}

//-----------------------------------------------------------------------------
