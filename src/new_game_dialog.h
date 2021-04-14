/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_NEW_GAME_DIALOG_H
#define CONNECTAGRAM_NEW_GAME_DIALOG_H

class Board;
class Pattern;
class WordList;

#include <QDialog>
class QComboBox;
class QSpinBox;
class QToolButton;

/**
 * @brief The NewGameDialog class allows the player to choose the new game settings.
 */
class NewGameDialog : public QDialog
{
	Q_OBJECT

public:
	/**
	 * Constructs a new game dialog.
	 * @param board the board to manage the settings for
	 * @param parent the QWidget that manages the dialog
	 */
	explicit NewGameDialog(Board* board, QWidget* parent = nullptr);

	/**
	 * Destroys the dialog.
	 */
	~NewGameDialog();

	/**
	 * Fetch the description of an amount of words.
	 * @param size the word range
	 * @return the translated description of a word density range
	 */
	static QString densityString(int size);

protected:
	/**
	 * Override keyPressEvent to handle starting a game when the player presses enter.
	 * @param event the details of the key press event
	 */
	void keyPressEvent(QKeyEvent* event) override;

private slots:
	/**
	 * Updates the maximum word length and the current word list based on the language selected.
	 * @param index the index of the languages combobox
	 */
	void languageSelected(int index);

	/**
	 * Updates which patterns are available for the selected length.
	 * @param index the index of the lengths combobox
	 */
	void lengthSelected(int index);

	/**
	 * Stores all of the settings and starts a new game.
	 */
	void patternSelected();

	/**
	 * Resets the settings back to their default values.
	 */
	void restoreDefaults();

private:
	/**
	 * Selects a language in the languages combobox.
	 * @param language the language to find in the combobox
	 */
	void setLanguage(const QString& language);

	/**
	 * Selects a word count in the counts combobox.
	 * @param count the count to find in the combobox
	 */
	void setCount(int count);

	/**
	 * Selects a length in the lengths combobox.
	 * @param length the length to find in the combobox
	 */
	void setLength(int length);

private:
	QComboBox* m_languages_box; /**< the available languages */
	QComboBox* m_word_count_box; /**< the available word counts */
	QComboBox* m_word_length_box; /**< the available word lengths */
	QList<QToolButton*> m_pattern_buttons; /**< buttons to select a pattern */
	QList<Pattern*> m_patterns; /**< the patterns to lay out words */
	Board* m_board; /**< the board to manage */
	WordList* m_wordlist; /**< the current word list */
};

#endif // CONNECTAGRAM_NEW_GAME_DIALOG_H
