/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_WINDOW_H
#define CONNECTAGRAM_WINDOW_H

class Board;
class Clock;
class Definitions;

#include <QMainWindow>
class QAction;
class QLabel;

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
	QAction* m_definitions_action;
	QAction* m_details_action;
	Board* m_board;
	Clock* m_clock;
	QLabel* m_definitions_button;
	QLabel* m_hint_button;
	Definitions* m_definitions;
	QLabel* m_success;
};

#endif // CONNECTAGRAM_WINDOW_H
