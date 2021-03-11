/***********************************************************************
 *
 * Copyright (C) 2009-2021 Graeme Gott <graeme@gottcode.org>
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

class Window : public QMainWindow
{
	Q_OBJECT

public:
	Window();
	~Window();

public slots:
	void newGame();

protected:
	void closeEvent(QCloseEvent* event) override;
	bool event(QEvent* event) override;
	bool eventFilter(QObject* object, QEvent* event) override;

private slots:
	void chooseGame();
	void about();
	void showDefinitions();
	void showDetails();
	void showScores();
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
	Definitions* m_definitions;
	QLabel* m_success;
};

#endif
