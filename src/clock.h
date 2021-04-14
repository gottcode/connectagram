/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_CLOCK_H
#define CONNECTAGRAM_CLOCK_H

#include <QLabel>
#include <QTime>
class QTimeLine;

/**
 * @brief The Clock class manages the value and display of the current game time.
 */
class Clock : public QLabel
{
	Q_OBJECT

public:
	/**
	 * Constructs a clock instance.
	 * @param parent the QWidget that manages the clock
	 */
	explicit Clock(QWidget* parent);

public slots:
	/**
	 * Starts the clock.
	 */
	void start();

	/**
	 * Stops the clock.
	 */
	void stop();

	/**
	 * Shows loading text.
	 */
	void setLoading();

	/**
	 * Shows paused text or current time.
	 * @param paused if it should show the pause text
	 */
	void setPaused(bool paused);

signals:
	/**
	 * Emitted when the player clicks on the clock to toggle pause state.
	 */
	void togglePaused();

protected:
	/**
	 * Override mousePressEvent to process player clicks.
	 * @param event details of mouse press event
	 */
	void mousePressEvent(QMouseEvent* event) override;

private slots:
	/**
	 * Update the current game time.
	 */
	void tick();

private:
	/**
	 * Update value of the current game time.
	 */
	void updateTime();

	/**
	 * Update display of the current game time.
	 */
	void updateText();

private:
	QTime m_clock; /**< value of the current game time */
	QTimeLine* m_clock_timer; /**< updates the current time */
	bool m_paused; /**< is the game paused */
	bool m_stopped; /**< is the game over */
};

#endif // CONNECTAGRAM_CLOCK_H
