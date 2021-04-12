/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_BOARD_H
#define CONNECTAGRAM_BOARD_H

class Cell;
class Pattern;
class Word;
class WordList;

#include <QGraphicsScene>
#include <QList>

class Board : public QGraphicsScene
{
	Q_OBJECT

public:
	explicit Board(QObject* parent = nullptr);
	~Board();

	void check(const QString& original_word, const QString& current_word);
	void click(const QString& word);

	Cell* cell(int x, int y) const
	{
		return m_cells.value(x).value(y);
	}

	Pattern* pattern() const
	{
		return m_pattern;
	}

	bool isFinished() const
	{
		return m_finished;
	}

	bool isPaused() const
	{
		return m_paused;
	}

	WordList* words() const
	{
		return m_wordlist;
	}

	void setCurrentWord(Word* word);

	QString gameNumber() const;

public slots:
	void openGame();
	bool openGame(const QString& number);
	void saveGame();
	void showHint();
	void setDarkMode(bool enabled);
	void setPaused(bool paused);
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
	WordList* m_wordlist;
	Pattern* m_pattern;
	QList<QList<Cell*>> m_cells;
	QList<Word*> m_words;
	Word* m_current_word;
	QGraphicsItem* m_hint;
	bool m_finished;
	bool m_paused;
};

#endif // CONNECTAGRAM_BOARD_H
