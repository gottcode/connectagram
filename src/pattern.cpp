/***********************************************************************
 *
 * Copyright (C) 2009, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
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

#include "pattern.h"

#include "word.h"

#include <algorithm>

//-----------------------------------------------------------------------------

Pattern::Pattern(WordList* words)
	: m_current(0,0)
	, m_words(words)
	, m_count(0)
	, m_length(0)
	, m_seed(0)
	, m_cancelled(false)
{
	Q_ASSERT(words);
}

//-----------------------------------------------------------------------------

Pattern::~Pattern()
{
	if (isRunning()) {
		m_cancelled_mutex.lock();
		m_cancelled = true;
		m_cancelled_mutex.unlock();
		wait();
	}
	cleanUp();
}

//-----------------------------------------------------------------------------

void Pattern::setCount(int count)
{
	m_count = counts().value(count, 4);
}

//-----------------------------------------------------------------------------

void Pattern::setLength(int length)
{
	m_length = qBound(minimumLength(), length, maximumLength()) - 1;
	m_words->setLength(m_length);
}

//-----------------------------------------------------------------------------

void Pattern::setSeed(unsigned int seed)
{
	m_seed = seed;
}

//-----------------------------------------------------------------------------

Pattern* Pattern::create(WordList* words, int type)
{
	Pattern* pattern = nullptr;
	switch (type) {
	case 0:
		pattern = new ChainPattern(words);
		break;
	case 1:
		pattern = new FencePattern(words);
		break;
	case 2:
		pattern = new RingsPattern(words);
		break;
	case 3:
		pattern = new StairsPattern(words);
		break;
	case 4:
		pattern = new TwistyPattern(words);
		break;
	case 5:
		pattern = new WavePattern(words);
		break;
	default:
		break;
	}
	return pattern;
}

//-----------------------------------------------------------------------------

Word* Pattern::addRandomWord(Qt::Orientation orientation)
{
	// Filter words on what characters are on the board
	QString known_letters;
	QPoint pos = m_current;
	QPoint delta = (orientation == Qt::Horizontal) ? QPoint(1, 0) : QPoint(0, 1);
	for (int i = 0; i <= wordLength(); ++i) {
		QChar c = at(pos);
		known_letters.append(c.isNull() ? QChar('.') : c);
		pos += delta;
	}
	QStringList words = m_words->filter(known_letters);

	// Find word
	QString result = !words.isEmpty() ? words.at(randomInt(words.count())) : QString();
	if (result.isEmpty()) {
		return nullptr;
	}

	// Remove anagrams of word
	m_words->addAnagramFilter(result);

	return new Word(result, m_current, orientation, m_random);
}

//-----------------------------------------------------------------------------

QChar Pattern::at(const QPoint& pos) const
{
	for (Word* word : m_solution) {
		QList<QPoint> positions = word->positions();
		for (int i = 0; i < positions.count(); ++i) {
			if (positions.at(i) == pos) {
				return word->at(i);
			}
		}
	}
	return QChar();
}

//-----------------------------------------------------------------------------

void Pattern::run()
{
	if (m_words->isEmpty()) {
		return;
	}

	m_random.seed(m_seed);

	// Add words
	cleanUp();
	int s = steps();
	int count = counts().value(wordCount());
	for (int i = 0; i < count; ++i) {
		Word* word = addWord(i % s);
		if (word) {
			m_solution.append(word);
		} else {
			cleanUp();
			i = -1;
		}

		QMutexLocker locker(&m_cancelled_mutex);
		if (m_cancelled) {
			return;
		}
	}

	// Move words so that no positions are negative
	int x1 = 0x7FFFFFFF;
	int x2 = -x1;
	int y1 = x1;
	int y2 = x2;
	for (Word* word : m_solution) {
		QList<QPoint> positions = word->positions();
		for (const QPoint& pos : positions) {
			x1 = std::min(x1, pos.x());
			y1 = std::min(y1, pos.y());
			x2 = std::max(x2, pos.x());
			y2 = std::max(y2, pos.y());
		}
	}
	m_size = QSize(x2 - x1 + 1, y2 - y1 + 1);

	QPoint delta = QPoint(-x1, -y1);
	for (Word* word : m_solution) {
		word->moveBy(delta);
	}

	m_words->resetAnagramFilters();

	emit generated();
}

//-----------------------------------------------------------------------------

void Pattern::cleanUp()
{
	qDeleteAll(m_solution);
	m_solution.clear();
	m_words->resetAnagramFilters();
	m_current = QPoint(0,0);
}

//-----------------------------------------------------------------------------

Word* Pattern::addWord(int)
{
	return nullptr;
}

//-----------------------------------------------------------------------------

Word* ChainPattern::addWord(int step)
{
	Word* result = nullptr;
	switch (step) {
	case 0:
		result = addRandomWord(Qt::Vertical);
		break;
	case 1:
		result = addRandomWord(Qt::Horizontal);
		m_current += QPoint(wordLength(), 0);
		break;
	case 2:
		result = addRandomWord(Qt::Vertical);
		m_current += QPoint(-wordLength(), wordLength());
		break;
	case 3:
		result = addRandomWord(Qt::Horizontal);
		m_current += QPoint(wordLength() - 1, wordLength() / -2);
		break;
	case 4:
		result = addRandomWord(Qt::Horizontal);
		m_current = QPoint(m_current.x() + wordLength() - 1, 0);
		break;
	default:
		break;
	}
	return result;
}

//-----------------------------------------------------------------------------

Word* FencePattern::addWord(int step)
{
	Word* result = nullptr;
	switch (step) {
	case 0:
		result = addRandomWord(Qt::Vertical);
		m_current += QPoint(0, 1);
		break;
	case 1:
		result = addRandomWord(Qt::Horizontal);
		m_current += QPoint(0, 2);
		break;
	case 2:
		result = addRandomWord(Qt::Horizontal);
		m_current += QPoint(wordLength(), -3);
		break;
	case 3:
		result = addRandomWord(Qt::Vertical);
		break;
	case 4:
		result = addRandomWord(Qt::Horizontal);
		m_current += QPoint(0, 2);
		break;
	case 5:
		result = addRandomWord(Qt::Horizontal);
		m_current += QPoint(wordLength(),- 2);
		break;
	default:
		break;
	}
	return result;
}

//-----------------------------------------------------------------------------

Word* RingsPattern::addWord(int step)
{
	Word* result = nullptr;
	switch (step) {
	case 0:
		result = addRandomWord(Qt::Horizontal);
		break;
	case 1:
		result = addRandomWord(Qt::Vertical);
		m_current += QPoint(0, wordLength());
		break;
	case 2:
		result = addRandomWord(Qt::Horizontal);
		m_current += QPoint(wordLength(), -wordLength());
		break;
	case 3:
		result = addRandomWord(Qt::Vertical);
		m_current = QPoint(m_current.x() - 2, m_current.y() ? 0 : (wordLength() - 2));
		break;
	default:
		break;
	}
	return result;
}

//-----------------------------------------------------------------------------

Word* StairsPattern::addWord(int step)
{
	Word* result = nullptr;
	switch (step) {
	case 0:
		result = addRandomWord(Qt::Horizontal);
		m_current += QPoint(wordLength() - 1, 0);
		break;
	case 1:
		result = addRandomWord(Qt::Vertical);
		m_current += QPoint(0, wordLength());
		break;
	default:
		break;
	}
	return result;
}

//-----------------------------------------------------------------------------

Word* TwistyPattern::addWord(int step)
{
	return step ? stepTwo() : stepOne();
}

//-----------------------------------------------------------------------------

Word* TwistyPattern::stepOne()
{
	QList<QPoint> positions;
	for (int i = 0; i <= wordLength(); ++i) {
		positions.append(m_current + QPoint(0, i));
	}

	while (!positions.isEmpty()) {
		QPoint pos = positions.takeAt(randomInt(positions.count()));

		// Define possible range for word
		int right = wordLength() + pos.x() - 1;
		int left = -wordLength() + pos.x() + 1;

		// Check for conflicting words on the right side of range for word
		for (int x = 1; x <= wordLength(); ++x) {
			for (int y = -1; y < 2; ++y) {
				QPoint test(pos.x() + x, pos.y() + y);
				if (!at(test).isNull()) {
					right = x + pos.x() - 2;
					x = wordLength();
					break;
				}
			}
		}

		// Check for conflicting words on the left side of range for word
		for (int x = -1; x >= -wordLength(); --x) {
			for (int y = -1; y < 2; ++y) {
				QPoint test(pos.x() + x, pos.y() + y);
				if (!at(test).isNull()) {
					left = x + pos.x() + 2;
					x = -wordLength();
					break;
				}
			}
		}

		// Randomly place word in range if it can fit
		int length = right - left;
		int offset = length - wordLength();
		if (offset >= 0) {
			left += offset ? randomInt(offset) : 0;
			right = left + wordLength();
			m_current = QPoint(left, pos.y());
			return addRandomWord(Qt::Horizontal);
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------

Word* TwistyPattern::stepTwo()
{
	QList<QPoint> positions;
	for (int i = 0; i <= wordLength(); ++i) {
		positions.append(m_current + QPoint(i, 0));
	}

	while (!positions.isEmpty()) {
		QPoint pos = positions.takeAt(randomInt(positions.count()));

		// Define possible range for word
		int bottom = wordLength() + pos.y() - 1;
		int top = -wordLength() + pos.y() + 1;

		// Check for conflicting words on the bottom of range for word
		for (int y = 1; y <= wordLength(); ++y) {
			for (int x = -1; x < 2; ++x) {
				QPoint test(pos.x() + x, pos.y() + y);
				if (!at(test).isNull()) {
					bottom = y + pos.y() - 2;
					y = wordLength();
					break;
				}
			}
		}

		// Check for conflicting words on the top of range for word
		for (int y = -1; y >= -wordLength(); --y) {
			for (int x = -1; x < 2; ++x) {
				QPoint test(pos.x() + x, pos.y() + y);
				if (!at(test).isNull()) {
					top = y + pos.y() + 2;
					y = -wordLength();
					break;
				}
			}
		}

		// Randomly place word in range if it can fit
		int length = bottom - top;
		int offset = length - wordLength();
		if (offset >= 0) {
			top += offset ? randomInt(offset) : 0;
			bottom = top + wordLength();
			m_current = QPoint(pos.x(), top);
			return addRandomWord(Qt::Vertical);
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------

Word* WavePattern::addWord(int step)
{
	Word* result = nullptr;
	switch (step) {
	case 0:
		result = addRandomWord(Qt::Horizontal);
		m_current += QPoint(wordLength(), 0);
		break;
	case 1:
		result = addRandomWord(Qt::Vertical);
		m_current += QPoint(0, wordLength());
		break;
	case 2:
		result = addRandomWord(Qt::Horizontal);
		m_current += QPoint(wordLength(), -wordLength());
		break;
	case 3:
		result = addRandomWord(Qt::Vertical);
		break;
	default:
		break;
	}
	return result;
}

//-----------------------------------------------------------------------------
