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

#ifndef BOARD_H
#define BOARD_H

#include <QGraphicsScene>
#include <QList>
class Cell;
class Pattern;
class Word;

class Board : public QGraphicsScene {
	Q_OBJECT

	public:
		Board(QObject* parent = 0);
		~Board();

		void check(const QString& original_word, const QString& current_word);
		void click(const QString& word);

		Cell* cell(int x, int y) const {
			return m_cells.value(x).value(y);
		}

		Pattern* pattern() const {
			return m_pattern;
		}

		bool isFinished() const {
			return m_finished;
		}

		bool isPaused() const {
			return m_paused;
		}

		void setCurrentWord(Word* word);
		void setPaused(bool paused);

	public slots:
		void openGame();
		void saveGame();
		void showHint();
		void togglePaused();

	signals:
		void loading();
		void finished();
		void started();
		void pauseChanged();
		void hintAvailable(bool available);
		void wordAdded(const QString& word);
		void wordSelected(const QString& word);
		void wordSolved(const QString& original_word, const QString& current_word);

	private slots:
		void patternGenerated();

	private:
		void cleanUp();

	private:
		Pattern* m_pattern;
		QList<QList<Cell*> > m_cells;
		QList<Word*> m_words;
		Word* m_current_word;
		QGraphicsItem* m_hint;
		bool m_finished;
		bool m_paused;
};

#endif
