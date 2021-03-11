/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "cell.h"

#include "letter.h"

//-----------------------------------------------------------------------------

Cell::Cell(const QPoint& position, Letter* letter)
	: m_position(position)
	, m_letter(nullptr)
{
	setLetter(letter);
}

//-----------------------------------------------------------------------------

void Cell::setLetter(Letter* letter)
{
	m_letter = letter;
	letter->setCell(this);
}

//-----------------------------------------------------------------------------
