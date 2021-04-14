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

/**
 * @brief The Board class controls the word list and the play area of the game.
 */
class Board : public QGraphicsScene
{
	Q_OBJECT

public:
	/**
	 * Constructs a board instance.
	 * @param parent the QObject that manages the board
	 */
	explicit Board(QObject* parent = nullptr);

	/**
	 * Destroys the board.
	 */
	~Board();

	/**
	 * Handles a solved word and checks if game is finished.
	 * @param original_word the word when the board was generated
	 * @param current_word the anagram of @p original_word found by the player
	 */
	void check(const QString& original_word, const QString& current_word);

	/**
	 * Handles the player clicking on a word.
	 * @param word the word the player clicked on
	 */
	void click(const QString& word);

	/**
	 * Fetch a cell.
	 * @param x the horizontal position
	 * @param y the vertical position
	 * @return the requested cell
	 */
	Cell* cell(int x, int y) const
	{
		return m_cells.value(x).value(y);
	}

	/**
	 * @return the pattern used to generate the board
	 */
	Pattern* pattern() const
	{
		return m_pattern;
	}

	/**
	 * @return whether the current game has finished
	 */
	bool isFinished() const
	{
		return m_finished;
	}

	/**
	 * @return whether the current game is paused
	 */
	bool isPaused() const
	{
		return m_paused;
	}

	/**
	 * @return the current word list
	 */
	WordList* words() const
	{
		return m_wordlist;
	}

	/**
	 * Highlights and tracks the word the player is interacting with.
	 * @param word the word player is interacting with
	 */
	void setCurrentWord(Word* word);

	/**
	 * @return the value to pass to #openGame() to play the same game
	 */
	QString gameNumber() const;

public slots:
	/**
	 * Continues a game from the previous run.
	 * @return @c true if the game could be started
	 */
	bool continueGame();

	/**
	 * Starts a new game.
	 * @param language the language of the board
	 * @param pattern which #Pattern to use to place the words
	 * @param count how many words to place
	 * @param length how long the words are
	 * @param seed the value to start the random number generator
	 */
	void newGame(const QString& language, int pattern, int count, int length, unsigned int seed);

	/**
	 * Starts a specified game.
	 * @param number the game to start
	 * @return @c true if the game could be started
	 */
	bool openGame(const QString& number);

	/**
	 * Stores the current spellings of the words.
	 */
	void saveGame();

	/**
	 * Shows a marker for which letter to move next.
	 */
	void showHint();

	/**
	 * Makes the play area background light or dark.
	 * @param enabled makes it dark if @c true
	 */
	void setDarkMode(bool enabled);

	/**
	 * Pauses or resumes the game.
	 * @param paused pauses the game if @c true
	 */
	void setPaused(bool paused);

	/**
	 * Pauses or resumes the game depending on if it is currently paused.
	 */
	void togglePaused();

signals:
	/**
	 * Emitted while the game is loading.
	 */
	void loading();

	/**
	 * Emitted when the game is over.
	 */
	void finished();

	/**
	 * Emitted when the game has finished loading and is ready to play.
	 */
	void started();

	/**
	 * Emitted when the game is paused or resumed.
	 */
	void pauseChanged();

	/**
	 * Emitted when the player can get a hint.
	 * @param available if the player can get a hint
	 */
	void hintAvailable(bool available);

	/**
	 * Emitted when a word is added to the board during game generation.
	 * @param word the original spelling of a word
	 */
	void wordAdded(const QString& word);

	/**
	 * Emitted when the player clicks on a word.
	 * @param word the current spelling of a word
	 */
	void wordSelected(const QString& word);

	/**
	 * Emitted when the player solves a word.
	 * @param original_word the original spelling of a word
	 * @param current_word the anagram of @p original_word found by the player
	 */
	void wordSolved(const QString& original_word, const QString& current_word);

private slots:
	/**
	 * Creates the cells when the pattern of words has been created.
	 */
	void patternGenerated();

private:
	/**
	 * Creates a new board layout.
	 * @param language the language of the board
	 * @param pattern which #Pattern to use to place the words
	 * @param count how many words to place
	 * @param length how long the words are
	 * @param seed the value to start the random number generator
	 */
	void generate(const QString& language, int pattern, int count, int length, unsigned int seed);

	/**
	 * Cleans up the board, removes all words, and resets all variables to the starting values.
	 */
	void cleanUp();

private:
	WordList* m_wordlist; /**< list of words for current language */
	Pattern* m_pattern; /**< how the words are arranged */
	QList<QList<Cell*>> m_cells; /**< the letter locations on the board */
	QList<Word*> m_words; /**< the words on the board */
	Word* m_current_word; /**< the word the player is interacting with */
	QGraphicsItem* m_hint; /**< currently displayed hint on the board */
	bool m_finished; /**< is the game over */
	bool m_paused; /**< is the game paused */
};

#endif // CONNECTAGRAM_BOARD_H
