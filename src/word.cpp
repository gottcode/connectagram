/*
	SPDX-FileCopyrightText: 2009-2014 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "word.h"

#include "board.h"
#include "cell.h"
#include "letter.h"
#include "wordlist.h"

#include <QGraphicsPathItem>
#include <QPainterPath>

#include <algorithm>

//-----------------------------------------------------------------------------

static int factorial(int num)
{
	return (num > 1) ? (factorial(num - 1) * num) : 1;
}

static int countPermutations(const QString& word)
{
	Q_ASSERT(!word.isEmpty());

	int n = factorial(word.length());
	int d = 1;

	QString sorted = word;
	std::sort(sorted.begin(), sorted.end());
	QChar c;
	int count = 0;
	for (const QChar& c2 : sorted) {
		if (c2 != c) {
			c = c2;
			d *= factorial(count);
			count = 1;
		} else {
			count++;
		}
	}
	d *= factorial(count);

	return n / d;
}

//-----------------------------------------------------------------------------

Word::Word(const QString& word, const QPoint& position, Qt::Orientation orientation, std::mt19937& random)
	: m_board(nullptr)
	, m_correct(false)
	, m_orientation(orientation)
	, m_random(random)
{
	if (word.isEmpty()) {
		return;
	}
	m_solutions.append(word);

	QPoint delta = (orientation == Qt::Horizontal) ? QPoint(1, 0) : QPoint(0, 1);
	QPoint pos = position;
	for (int i = 0; i < word.length(); ++i) {
		m_positions.append(pos);
		pos += delta;
	}
}

//-----------------------------------------------------------------------------

void Word::click()
{
	m_board->click(toString());
}

//-----------------------------------------------------------------------------

void Word::check()
{
	QString word;
	for (const QPoint& pos : m_positions) {
		word += m_board->cell(pos.x(), pos.y())->letter()->character();
	}
	if (m_solutions.contains(word)) {
		m_correct = true;
		for (const QPoint& pos : m_positions) {
			Cell* cell = m_board->cell(pos.x(), pos.y());
			cell->setWord(this);
			cell->letter()->setCorrect();
		}
		m_board->setCurrentWord(nullptr);
		m_board->check(m_solutions.at(0), word);
	}
}

//-----------------------------------------------------------------------------

QGraphicsItem* Word::hint()
{
	if (isCorrect()) {
		return nullptr;
	}

	// Find position of first incorrect character
	int count = m_positions.count();
	int pos = -1;
	QString solution;
	for (const QString& word : m_solutions) {
		for (int i = 0; i < count; ++i) {
			const QPoint& point = m_positions.at(i);
			Letter* letter = m_board->cell(point.x(), point.y())->letter();
			if (letter->character() != word.at(i)) {
				if (i > pos) {
					pos = i;
					solution = word;
				}
				break;
			}
		}
	}

	// Find letter from end of string which goes in that position
	QChar c = solution.at(pos);
	int pos2;
	QPointF position;
	for (pos2 = count - 1; pos2 > pos; --pos2) {
		const QPoint& point = m_positions.at(pos2);
		Letter* letter = m_board->cell(point.x(), point.y())->letter();
		if (letter->isMovable() && letter->character() == c) {
			position = letter->scenePos();
			letter->setBrush(QColor(0xff, 0xbf, 0x0));
			break;
		}
	}

	// Create hint graphicsitem
	int edge = ((pos2 - pos - 1) * 34) + 16;
	QList<QPointF> positions;
	positions += QPointF(0, 8);
	positions += QPointF(12, 0);
	positions += QPointF(12, 5);
	positions += QPointF(edge, 5);
	positions += QPointF(edge, 11);
	positions += QPointF(12, 11);
	positions += QPointF(12, 16);
	positions += QPointF(0, 8);
	QPointF delta = QPointF(-edge, 8);
	if (m_orientation == Qt::Vertical) {
		delta = QPointF(8, -edge);
		for (int i = 0; i < 8; ++i) {
			QPointF& pos = positions[i];
			qSwap(pos.rx(), pos.ry());
		}
	}
	QPainterPath path;
	path.moveTo(positions.at(0));
	for (int i = 1; i < 8; ++i) {
		path.lineTo(positions.at(i));
	}

	QGraphicsPathItem* item = m_board->addPath(path, Qt::NoPen, QColor(0xff, 0xbf, 0x0));
	item->setPos(position + delta);
	item->setZValue(20);

	return item;
}

//-----------------------------------------------------------------------------

void Word::moveBy(const QPoint& delta)
{
	int count = m_positions.count();
	for (int i = 0; i < count; ++i) {
		m_positions[i] += delta;
	}
}

//-----------------------------------------------------------------------------

void Word::fromString(const QString& shuffled)
{
	// Find movable letters
	QString sorted1;
	QList<Letter*> letters;
	for (int i = 0; i < m_positions.count(); ++i) {
		const QPoint& pos = m_positions.at(i);
		Letter* letter = m_board->cell(pos.x(), pos.y())->letter();
		QChar c = letter->character();
		sorted1.append(c);
		if (letter->isMovable()) {
			letters.append(letter);
		} else if (c != shuffled.at(i)) {
			return;
		}
	}

	// Check if shuffled has the right characters
	QString sorted2 = shuffled;
	std::sort(sorted1.begin(), sorted1.end());
	std::sort(sorted2.begin(), sorted2.end());
	if (sorted1 != sorted2) {
		return;
	}

	// Move letters to match shuffled
	for (int i = 0; i < m_positions.count(); ++i) {
		const QPoint& pos = m_positions.at(i);
		Cell* cell = m_board->cell(pos.x(), pos.y());
		if (!cell->letter()->isMovable()) {
			continue;
		}

		QChar c = shuffled.at(i);
		for (int j = 0; j < letters.count(); ++j) {
			if (letters.at(j)->character() == c) {
				cell->setLetter(letters.takeAt(j));
				break;
			}
		}
	}

	check();
}

//-----------------------------------------------------------------------------

QString Word::toString() const
{
	QString result;
	for (const QPoint& pos : m_positions) {
		result.append(m_board->cell(pos.x(), pos.y())->letter()->character());
	}
	return result;
}

//-----------------------------------------------------------------------------

void Word::setHighlight(bool highlight)
{
	for (const QPoint& pos : m_positions) {
		m_board->cell(pos.x(), pos.y())->letter()->setHighlight(highlight);
	}
}

//-----------------------------------------------------------------------------

void Word::shuffle(const WordList* words)
{
	// Create list of characters and filter the list of words
	QString chars;
	QString movable;
	QString filter;
	for (int i = 0; i < m_positions.count(); ++i) {
		const QPoint& pos = m_positions.at(i);
		Letter* letter = m_board->cell(pos.x(), pos.y())->letter();
		QChar c = letter->character();
		chars.append(c);
		if (letter->isMovable()) {
			movable.append(c);
			filter.append('.');
		} else {
			filter.append(c);
		}
	}
	std::sort(chars.begin(), chars.end());
	std::sort(movable.begin(), movable.end());
	QStringList filtered = words->filter(filter);

	// Find valid solutions
	for (const QString& valid : filtered) {
		QString sorted = valid;
		std::sort(sorted.begin(), sorted.end());
		if (sorted == chars && !m_solutions.contains(valid)) {
			m_solutions.append(valid);
		}
	}

	// Find permutation
	QString permuted;
	if (m_solutions.count() < countPermutations(movable)) {
		do {
			std::shuffle(movable.begin(), movable.end(), m_random);
			permuted = movable;
			for (int i = 0, end = filter.length(); i < end; ++i) {
				if (filter.at(i) == '.') {
					continue;
				}
				permuted.insert(i, filter.at(i));
			}
		} while (m_solutions.contains(permuted));
	} else {
		permuted = m_solutions.at(0);
	}
	fromString(permuted);
}

//-----------------------------------------------------------------------------
