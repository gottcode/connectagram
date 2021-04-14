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

/**
 * @brief The Window class controls creation of the game board and user interface.
 */
class Window : public QMainWindow
{
	Q_OBJECT

public:
	/**
	 * Constructs a window instance.
	 */
	Window();

	/**
	 * Save the game when the window is destroyed.
	 */
	~Window();

public slots:
	/**
	 * Shows the new game dialog to start a new game.
	 */
	void newGame();

protected:
	/**
	 * Override closeEvent to save the game and window position when window is closed.
	 * @param event details of close event
	 */
	void closeEvent(QCloseEvent* event) override;

	/**
	 * Handle detecting clicks to activate definitions or hint buttons.
	 * @param object what object had the event
	 * @param event which event happened
	 * @return @c true if no more processing of the event should occur
	 */
	bool eventFilter(QObject* object, QEvent* event) override;

private slots:
	/**
	 * Opens a new game.
	 */
	void chooseGame();

	/**
	 * Shows a dialog with version information and copyright notices.
	 */
	void about();

	/**
	 * Shows the menu of words to define.
	 */
	void showDefinitions();

	/**
	 * Shows a small dialog with the current game settings.
	 */
	void showDetails();

	/**
	 * Shows the high scores.
	 */
	void showScores();

	/**
	 * Shows a dialog that allows the player to choose the interface language.
	 */
	void setLocale();

	/**
	 * Shows the game area.
	 */
	void gameStarted();

	/**
	 * Handles game ending and adds to high score board.
	 */
	void gameFinished();

	/**
	 * Toggles the checked state of the pause action and the clock.
	 */
	void gamePauseChanged();

private:
	QAction* m_pause_action; /**< controls if the game is paused */
	QAction* m_definitions_action; /**< menu action to show definitions menu */
	QAction* m_details_action; /**< menu action to show details */
	Board* m_board; /**< the game area */
	Clock* m_clock; /**< shows the current time */
	QLabel* m_overlay; /**< dark area behind clock and buttons */
	QLabel* m_definitions_button; /**< button to show definitions menu */
	QLabel* m_hint_button; /**< button to show hint */
	Definitions* m_definitions; /**< the list of word definitions */
	QLabel* m_success; /**< message to show game over */
};

#endif // CONNECTAGRAM_WINDOW_H
