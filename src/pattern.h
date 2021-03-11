/***********************************************************************
 *
 * Copyright (C) 2009, 2013, 2014 Graeme Gott <graeme@gottcode.org>
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

#ifndef PATTERN_H
#define PATTERN_H

#include "wordlist.h"
class Word;

#include <QHash>
#include <QList>
#include <QMutex>
#include <QPoint>
#include <QSize>
#include <QStringList>
#include <QThread>

#include <random>

class Pattern : public QThread
{
	Q_OBJECT

public:
	virtual ~Pattern();

	static Pattern* create(WordList* words, int type);

	static int types()
	{
		return 6;
	}

	QSize size() const
	{
		return m_size;
	}

	virtual QList<int> counts() const
	{
		return QList<int>() << 4 << 8 << 12 << 16;
	}

	virtual QString name() const
	{
		return QString();
	}

	virtual int minimumLength() const
	{
		return 5;
	}

	int maximumLength() const
	{
		return m_words->maximumLength();
	}

	QList<Word*> solution() const
	{
		return m_solution;
	}

	unsigned int seed() const
	{
		return m_seed;
	}

	const WordList* words() const
	{
		return m_words;
	}

	int wordCount() const
	{
		return counts().indexOf(m_count);
	}

	int wordLength() const
	{
		return m_length;
	}

	void setCount(int count);
	void setLength(int length);
	void setSeed(unsigned int seed);

signals:
	void generated();

protected:
	Pattern(WordList* words);

	Word* addRandomWord(Qt::Orientation orientation);
	QChar at(const QPoint& pos) const;
	virtual void run();

	unsigned int randomInt(unsigned int max)
	{
		std::uniform_int_distribution<unsigned int> gen(0, max - 1);
		return gen(m_random);
	}

protected:
	QPoint m_current;

private:
	void cleanUp();

	virtual int steps() const
	{
		return 2;
	}

	virtual Word* addWord(int step);

private:
	WordList* m_words;
	int m_count;
	int m_length;
	unsigned int m_seed;
	QSize m_size;
	QList<Word*> m_solution;
	bool m_cancelled;
	QMutex m_cancelled_mutex;
	std::mt19937 m_random;
};

//-----------------------------------------------------------------------------

class ChainPattern : public Pattern
{
	Q_OBJECT

public:
	ChainPattern(WordList* words)
		: Pattern(words)
	{
	}

	QList<int> counts() const
	{
		return QList<int>() << 4 << 9 << 14 << 19;
	}

	QString name() const
	{
		return tr("Chain");
	}

private:
	int steps() const
	{
		return 5;
	}

	Word* addWord(int step);
};

//-----------------------------------------------------------------------------

class FencePattern : public Pattern
{
	Q_OBJECT

public:
	FencePattern(WordList* words)
		: Pattern(words)
	{
	}

	QList<int> counts() const
	{
		return QList<int>() << 4 << 7 << 13 << 16;
	}

	QString name() const
	{
		return tr("Fence");
	}

private:
	int steps() const
	{
		return 6;
	}

	Word* addWord(int step);
};

//-----------------------------------------------------------------------------

class RingsPattern : public Pattern
{
	Q_OBJECT

public:
	RingsPattern(WordList* words)
		: Pattern(words)
	{
	}

	int minimumLength() const
	{
		return 7;
	}

	QString name() const
	{
		return tr("Rings");
	}

private:
	int steps() const
	{
		return 4;
	}

	Word* addWord(int step);
};

//-----------------------------------------------------------------------------

class StairsPattern : public Pattern
{
	Q_OBJECT

public:
	StairsPattern(WordList* words)
		: Pattern(words)
	{
	}

	QString name() const
	{
		return tr("Stairs");
	}

private:
	Word* addWord(int step);
};

//-----------------------------------------------------------------------------

class TwistyPattern : public Pattern
{
	Q_OBJECT

public:
	TwistyPattern(WordList* words)
		: Pattern(words)
	{
	}

	QString name() const
	{
		return tr("Twisty");
	}

private:
	Word* addWord(int step);
	Word* stepOne();
	Word* stepTwo();
};

//-----------------------------------------------------------------------------

class WavePattern : public Pattern
{
	Q_OBJECT

public:
	WavePattern(WordList* words)
		: Pattern(words)
	{
	}

	QList<int> counts() const
	{
		return QList<int>() << 5 << 9 << 13 << 17;
	}

	QString name() const
	{
		return tr("Wave");
	}

private:
	int steps() const
	{
		return 4;
	}

	Word* addWord(int step);
};

#endif
