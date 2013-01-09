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

class Pattern : public QThread {
	Q_OBJECT

	public:
		Pattern();
		virtual ~Pattern();

		static Pattern* create(int type);

		static int types() {
			return 6;
		}

		QSize size() const {
			return m_size;
		}

		virtual int minimumLength() const {
			return 5;
		}

		virtual int minimumCount() const {
			return 4;
		}

		virtual int expandCount() const {
			return 2;
		}

		virtual QString name() const {
			return QString();
		}

		QList<Word*> solution() const {
			return m_solution;
		}

		QStringList words() const {
			return m_words.words();
		}

		int maximumLength() const {
			return m_words.maximumLength();
		}

		int seed() const {
			return m_seed;
		}

		int wordCount() const {
			return m_count;
		}

		int wordLength() const {
			return m_length;
		}

		void setCount(int count);
		void setLength(int length);
		void setSeed(int seed);

	signals:
		void generated();

	protected:
		Word* addRandomWord(Qt::Orientation orientation);
		QChar at(const QPoint& pos) const;
		virtual void run();

	protected:
		QPoint m_current;

	private:
		void cleanUp();

		virtual int steps() const {
			return expandCount();
		}

		virtual Word* addWord(int step);

	private:
		static WordList m_words;
		int m_count;
		int m_length;
		int m_seed;
		QSize m_size;
		QList<Word*> m_solution;
		bool m_cancelled;
		QMutex m_cancelled_mutex;
};

//-----------------------------------------------------------------------------

class ChainPattern : public Pattern {
	public:
		int expandCount() const {
			return 5;
		}

		QString name() const {
			return tr("Chain");
		}

	private:
		Word* addWord(int step);
};

//-----------------------------------------------------------------------------

class FencePattern : public Pattern {
	public:
		int expandCount() const {
			return 3;
		}

		QString name() const {
			return tr("Fence");
		}

	private:
		int steps() const {
			return 6;
		}

		Word* addWord(int step);
};

//-----------------------------------------------------------------------------

class RingsPattern : public Pattern {
	public:
		int minimumLength() const {
			return 7;
		}

		int expandCount() const {
			return 4;
		}

		QString name() const {
			return tr("Rings");
		}

	private:
		Word* addWord(int step);
};

//-----------------------------------------------------------------------------

class StairsPattern : public Pattern {
	public:
		QString name() const {
			return tr("Stairs");
		}

	private:
		Word* addWord(int step);
};

//-----------------------------------------------------------------------------

class TwistyPattern : public Pattern {
	public:
		QString name() const {
			return tr("Twisty");
		}

	private:
		Word* addWord(int step);
		Word* stepOne();
		Word* stepTwo();
};

//-----------------------------------------------------------------------------

class WavePattern : public Pattern {
	public:
		int minimumCount() const {
			return 5;
		}

		int expandCount() const {
			return 4;
		}

		QString name() const {
			return tr("Wave");
		}

	private:
		Word* addWord(int step);
};

#endif
