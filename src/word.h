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

#ifndef WORD_H
#define WORD_H

#include <QChar>
#include <QList>
#include <QPoint>
#include <QString>
class QGraphicsItem;
class Board;
class Random;
class WordList;

class Word {
	public:
		Word(const QString& word, const QPoint& position, Qt::Orientation orientation, Random& random);

		QChar at(int i) const {
			return m_solutions.at(0).at(i);
		}

		void click();
		void check();
		QGraphicsItem* hint();

		bool isCorrect() const {
			return m_correct;
		}

		Qt::Orientation orientation() const {
			return m_orientation;
		}

		QList<QPoint> positions() const {
			return m_positions;
		}

		QList<QString> solutions() const {
			return m_solutions;
		}

		void moveBy(const QPoint& delta);

		void fromString(const QString& shuffled);
		QString toString() const;

		void setBoard(Board* board) {
			m_board = board;
		}

		void setHighlight(bool highlight);
		void shuffle(const WordList& words);

	private:
		Board* m_board;
		bool m_correct;
		QList<QPoint> m_positions;
		QList<QString> m_solutions;
		Qt::Orientation m_orientation;
		Random& m_random;
};

#endif
