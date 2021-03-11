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

class NewGameDialog : public QDialog
{
	Q_OBJECT

public:
	explicit NewGameDialog(Board* board, QWidget* parent = nullptr);
	~NewGameDialog();

	static QString densityString(int size);

protected:
	void keyPressEvent(QKeyEvent* event) override;

private slots:
	void languageSelected(int index);
	void lengthSelected(int index);
	void patternSelected();

private:
	void setLanguage(const QString& language);
	void setCount(int count);
	void setLength(int length);

private:
	QComboBox* m_languages_box;
	QComboBox* m_word_count_box;
	QComboBox* m_word_length_box;
	QList<QToolButton*> m_pattern_buttons;
	QList<Pattern*> m_patterns;
	Board* m_board;
	WordList* m_wordlist;
};

#endif // CONNECTAGRAM_NEW_GAME_DIALOG_H
