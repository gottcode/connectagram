/*
	SPDX-FileCopyrightText: 2009-2014 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_WORD_H
#define CONNECTAGRAM_WORD_H

#include <QChar>
#include <QList>
#include <QPoint>
#include <QString>
class QGraphicsItem;
class Board;
class Random;
class WordList;

#include <random>

class Word
{
public:
	Word(const QString& word, const QPoint& position, Qt::Orientation orientation, std::mt19937& random);

	QChar at(int i) const
	{
		return m_solutions.at(0).at(i);
	}

	void click();
	void check();
	QGraphicsItem* hint();

	bool isCorrect() const
	{
		return m_correct;
	}

	Qt::Orientation orientation() const
	{
		return m_orientation;
	}

	QList<QPoint> positions() const
	{
		return m_positions;
	}

	QList<QString> solutions() const
	{
		return m_solutions;
	}

	void moveBy(const QPoint& delta);

	void fromString(const QString& shuffled);
	QString toString() const;

	void setBoard(Board* board)
	{
		m_board = board;
	}

	void setHighlight(bool highlight);
	void shuffle(const WordList* words);

private:
	Board* m_board;
	bool m_correct;
	QList<QPoint> m_positions;
	QList<QString> m_solutions;
	Qt::Orientation m_orientation;
	std::mt19937& m_random;
};

#endif // CONNECTAGRAM_WORD_H
