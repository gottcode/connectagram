/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CLOCK_H
#define CLOCK_H

#include <QLabel>
#include <QTime>
class QTimeLine;

class Clock : public QLabel
{
	Q_OBJECT

public:
	Clock(QWidget* parent);

public slots:
	void start();
	void stop();
	void setLoading();
	void setPaused(bool paused);

signals:
	void togglePaused();

protected:
	void mousePressEvent(QMouseEvent* event) override;

private slots:
	void tick();

private:
	void updateTime();
	void updateText();

private:
	QTime m_clock;
	QTimeLine* m_clock_timer;
	bool m_paused;
	bool m_stopped;
};

#endif
