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

#ifndef NEW_GAME_DIALOG_H
#define NEW_GAME_DIALOG_H

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
	NewGameDialog(Board* board, QWidget* parent = 0);
	~NewGameDialog();

	static QString densityString(int size);

protected:
	void keyPressEvent(QKeyEvent* event);

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

#endif
