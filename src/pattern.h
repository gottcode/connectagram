/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_PATTERN_H
#define CONNECTAGRAM_PATTERN_H

#include "wordlist.h"
class Word;

#include <QHash>
#include <QList>
#include <QPoint>
#include <QSize>
#include <QStringList>
#include <QThread>

#include <atomic>
#include <random>

/**
 * @brief The Pattern class defines how the words are laid out on the board.
 */
class Pattern : public QThread
{
	Q_OBJECT

public:
	/**
	 * Cancels running layout and waits for it to finish before cleaning up.
	 */
	virtual ~Pattern();

	/**
	 * Creates a new pattern instance.
	 * @param words the list of words to use
	 * @param type the pattern to create
	 * @return the pattern instance or @c nullptr if @p type is not valid
	 */
	static Pattern* create(WordList* words, int type);

	/**
	 * @return the total number of types available
	 */
	static int types()
	{
		return 6;
	}

	/**
	 * @return the width and height in cells of the generated board
	 */
	QSize size() const
	{
		return m_size;
	}

	/**
	 * @return the amount of words for each word density of the current pattern
	 */
	virtual QList<int> counts() const
	{
		return QList<int>() << 4 << 8 << 12 << 16;
	}

	/**
	 * @return the translated named of the pattern
	 */
	virtual QString name() const
	{
		return QString();
	}

	/**
	 * @return the shortest allowed word length
	 */
	virtual int minimumLength() const
	{
		return 5;
	}

	/**
	 * @return the longest word length available
	 */
	int maximumLength() const
	{
		return m_words->maximumLength();
	}

	/**
	 * @return the words of the layout
	 */
	QList<Word*> solution() const
	{
		return m_solution;
	}

	/**
	 * @return the seed used to create the layout
	 */
	unsigned int seed() const
	{
		return m_seed;
	}

	/**
	 * @return the pattern type, to be used with #create()
	 */
	virtual int type() const = 0;

	/**
	 * @return the word list used
	 */
	const WordList* words() const
	{
		return m_words;
	}

	/**
	 * @return the amount of words used
	 */
	int wordCount() const
	{
		return counts().indexOf(m_count);
	}

	/**
	 * @return how long the words are
	 */
	int wordLength() const
	{
		return m_length;
	}

	/**
	 * Sets the word density to use.
	 * @param count the word density used to pick the amount of words
	 */
	void setCount(int count);

	/**
	 * Sets how long the words are.
	 * @param length how long the words are
	 */
	void setLength(int length);

	/**
	 * Sets the seed for the random number generator.
	 * @param seed the seed for the random number generator
	 */
	void setSeed(unsigned int seed);

Q_SIGNALS:
	/**
	 * Emitted when the pattern has finished laying out the words.
	 */
	void generated();

protected:
	/**
	 * Constructs a new pattern instance.
	 * @param words the word list to use
	 */
	explicit Pattern(WordList* words);

	/**
	 * Adds a random word to the layout at #m_current, constrained by what words it touches.
	 * @param orientation the direction of the word
	 * @return the word placed in the layout
	 */
	Word* addRandomWord(Qt::Orientation orientation);

	/**
	 * Fetch the character at the requested position.
	 * @param pos the location of the character
	 * @return the character or a null QChar if nothing is there
	 */
	QChar at(const QPoint& pos) const;

	/**
	 * Fetches a random number.
	 * @param max the maximum value of the random number
	 * @return a random number
	 */
	unsigned int randomInt(unsigned int max)
	{
		std::uniform_int_distribution<unsigned int> gen(0, max - 1);
		return gen(m_random);
	}

protected:
	QPoint m_current; /**< the location used to place words in #addRandomWord() */

private:
	/**
	 * Generates the pattern of words in a new thread.
	 */
	void run() final;

	/**
	 * Resets all values and removes the solution.
	 */
	void cleanUp();

	/**
	 * @return how many steps in #addWord()
	 */
	virtual int steps() const
	{
		return 2;
	}

	/**
	 * Adds a word to the layout.
	 * @param step which step the pattern is on
	 * @return word added to the layout
	 */
	virtual Word* addWord(int step);

private:
	WordList* m_words; /**< the list of words */
	int m_count; /**< the amount of words */
	int m_length; /**< the length of each word */
	unsigned int m_seed; /**< seed for random number generator */
	QSize m_size; /**< the size of the layout */
	QList<Word*> m_solution; /**< the words in the layout */
	std::atomic_bool m_cancelled; /**< is the thread cancelled */
	std::mt19937 m_random; /**< the random number generator */
};

//-----------------------------------------------------------------------------

class ChainPattern : public Pattern
{
	Q_OBJECT

public:
	explicit ChainPattern(WordList* words)
		: Pattern(words)
	{
	}

	QList<int> counts() const override
	{
		return QList<int>() << 4 << 9 << 14 << 19;
	}

	QString name() const override
	{
		return tr("Chain");
	}

	static constexpr int Type = 0;
	int type() const override
	{
		return Type;
	}

private:
	int steps() const override
	{
		return 5;
	}

	Word* addWord(int step) override;
};

//-----------------------------------------------------------------------------

class FencePattern : public Pattern
{
	Q_OBJECT

public:
	explicit FencePattern(WordList* words)
		: Pattern(words)
	{
	}

	QList<int> counts() const override
	{
		return QList<int>() << 4 << 7 << 13 << 16;
	}

	QString name() const override
	{
		return tr("Fence");
	}

	static constexpr int Type = 1;
	int type() const override
	{
		return Type;
	}

private:
	int steps() const override
	{
		return 6;
	}

	Word* addWord(int step) override;
};

//-----------------------------------------------------------------------------

class RingsPattern : public Pattern
{
	Q_OBJECT

public:
	explicit RingsPattern(WordList* words)
		: Pattern(words)
	{
	}

	int minimumLength() const override
	{
		return 7;
	}

	QString name() const override
	{
		return tr("Rings");
	}

	static constexpr int Type = 2;
	int type() const override
	{
		return Type;
	}

private:
	int steps() const override
	{
		return 4;
	}

	Word* addWord(int step) override;
};

//-----------------------------------------------------------------------------

class StairsPattern : public Pattern
{
	Q_OBJECT

public:
	explicit StairsPattern(WordList* words)
		: Pattern(words)
	{
	}

	QString name() const override
	{
		return tr("Stairs");
	}

	static constexpr int Type = 3;
	int type() const override
	{
		return Type;
	}

private:
	Word* addWord(int step) override;
};

//-----------------------------------------------------------------------------

class TwistyPattern : public Pattern
{
	Q_OBJECT

public:
	explicit TwistyPattern(WordList* words)
		: Pattern(words)
	{
	}

	QString name() const override
	{
		return tr("Twisty");
	}

	static constexpr int Type = 4;
	int type() const override
	{
		return Type;
	}

private:
	Word* addWord(int step) override;
	Word* addHorizontalWord();
	Word* addVerticalWord();
};

//-----------------------------------------------------------------------------

class WavePattern : public Pattern
{
	Q_OBJECT

public:
	explicit WavePattern(WordList* words)
		: Pattern(words)
	{
	}

	QList<int> counts() const override
	{
		return QList<int>() << 5 << 9 << 13 << 17;
	}

	QString name() const override
	{
		return tr("Wave");
	}

	static constexpr int Type = 5;
	int type() const override
	{
		return Type;
	}

private:
	int steps() const override
	{
		return 4;
	}

	Word* addWord(int step) override;
};

#endif // CONNECTAGRAM_PATTERN_H
