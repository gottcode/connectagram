/***********************************************************************
 *
 * Copyright (C) 2009 Graeme Gott <graeme@gottcode.org>
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

#include <QDialog>
class QComboBox;
class QSpinBox;
class Board;
class Pattern;

class NewGameDialog : public QDialog {
	Q_OBJECT

	public:
		NewGameDialog(Board* board, QWidget* parent = 0);
		~NewGameDialog();

	public slots:
		void accept();

	private slots:
		void patternSelected(int index);

	private:
		void setPattern(int pattern);
		void setCount(int count);
		void setLength(int length);

	private:
		QComboBox* m_patterns_box;
		QComboBox* m_word_count_box;
		QComboBox* m_word_length_box;
		QSpinBox* m_seed_box;
		QList<Pattern*> m_patterns;
		Board* m_board;
};

#endif
