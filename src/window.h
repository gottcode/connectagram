/***********************************************************************
 *
 * Copyright (C) 2009, 2014 Graeme Gott <graeme@gottcode.org>
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

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
class QAction;
class QLabel;
class Board;
class Clock;
class Definitions;
class ScoreBoard;

class Window : public QMainWindow {
	Q_OBJECT

	public:
		Window();
		~Window();

	public slots:
		void newGame();

	protected:
		virtual void closeEvent(QCloseEvent* event);
		virtual bool event(QEvent* event);
		virtual bool eventFilter(QObject* object, QEvent* event);

	private slots:
		void chooseGame();
		void about();
		void showDetails();
		void setLocale();
		void gameStarted();
		void gameFinished();
		void gamePauseChanged();

	private:
		QAction* m_pause_action;
		Board* m_board;
		Clock* m_clock;
		QLabel* m_definitions_button;
		QLabel* m_hint_button;
		ScoreBoard* m_scores;
		Definitions* m_definitions;
		QLabel* m_success;
};

#endif
