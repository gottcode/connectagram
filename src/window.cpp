/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "window.h"

#include "board.h"
#include "clock.h"
#include "definitions.h"
#include "locale_dialog.h"
#include "new_game_dialog.h"
#include "pattern.h"
#include "scores_dialog.h"
#include "view.h"

#include <QApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

#include <algorithm>
#include <cmath>

//-----------------------------------------------------------------------------

Window::Window()
{
	m_board = new Board(this);
	connect(m_board, &Board::finished, this, &Window::gameFinished);
	connect(m_board, &Board::started, this, &Window::gameStarted);
	connect(m_board, &Board::pauseChanged, this, &Window::gamePauseChanged);

	QWidget* contents = new QWidget(this);
	setCentralWidget(contents);

	View* view = new View(m_board, contents);

	m_definitions = new Definitions(m_board->words(), this);
	connect(m_board, &Board::wordAdded, m_definitions, &Definitions::addWord);
	connect(m_board, &Board::wordSolved, m_definitions, &Definitions::solveWord);
	connect(m_board, &Board::wordSelected, m_definitions, &Definitions::selectWord);
	connect(m_board, &Board::loading, m_definitions, &Definitions::clearWords);

	// Create success message
	m_success = new QLabel(contents);
	m_success->setAttribute(Qt::WA_TransparentForMouseEvents);

	QFont f = font();
	f.setPointSize(24);
	QFontMetrics metrics(f);
	int width = metrics.boundingRect(tr("Success")).width();
	int height = metrics.height();
	int ratio = devicePixelRatio();
	QPixmap pixmap(QSize(width + height, height * 2) * ratio);
	pixmap.setDevicePixelRatio(ratio);
	pixmap.fill(QColor(0, 0, 0, 0));
	{
		QPainter painter(&pixmap);

		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor(0, 0, 0, 200));
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.drawRoundedRect(0, 0, width + height, height * 2, 10, 10);

		painter.setFont(f);
		painter.setPen(Qt::white);
		painter.setRenderHint(QPainter::TextAntialiasing, true);
		painter.drawText(height / 2, height / 2 + metrics.ascent(), tr("Success"));
	}
	m_success->setPixmap(pixmap);
	m_success->hide();
	connect(m_board, &Board::loading, m_success, &QLabel::hide);

	// Create overlay background
	m_overlay = new QLabel(this);

	f = font();
	f.setPixelSize(20);
	metrics = QFontMetrics(f);
	width = std::max(metrics.boundingRect(tr("Loading")).width(), metrics.boundingRect(tr("Paused")).width());
	for (int i = 0; i < 10; ++i) {
		QString test(6, QChar(i + 48));
		test.insert(4, QChar(':'));
		test.insert(2, QChar(':'));
		width = std::max(width, metrics.boundingRect(test).width());
	}
	pixmap = QPixmap(QSize(width + 82, 32) * ratio);
	pixmap.setDevicePixelRatio(ratio);
	pixmap.fill(Qt::transparent);
	{
		QPainter painter(&pixmap);

		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor(0, 0, 0, 200));
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.drawRoundedRect(0, -32, width + 82, 64, 5, 5);
	}
	m_overlay->setPixmap(pixmap);

	m_overlay->hide();

	// Create overlay buttons
	m_definitions_button = new QLabel(m_overlay);
	m_definitions_button->setPixmap(QIcon(":/definitions.png").pixmap(24,24));
	m_definitions_button->setCursor(Qt::PointingHandCursor);
	m_definitions_button->setToolTip(tr("Definitions"));
	m_definitions_button->setDisabled(true);
	m_definitions_button->installEventFilter(this);

	m_hint_button = new QLabel(m_overlay);
	m_hint_button->setPixmap(QIcon(":/hint.png").pixmap(24,24));
	m_hint_button->setCursor(Qt::PointingHandCursor);
	m_hint_button->setToolTip(tr("Hint"));
	m_hint_button->setDisabled(true);
	m_hint_button->installEventFilter(this);
	connect(m_board, &Board::hintAvailable, m_hint_button, &QLabel::setEnabled);

	// Create clock
	m_clock = new Clock(m_overlay);
	m_clock->setDisabled(true);
	connect(m_clock, &Clock::togglePaused, m_board, &Board::togglePaused);
	connect(m_board, &Board::loading, m_clock, &Clock::setLoading);

	QHBoxLayout* overlay_layout = new QHBoxLayout(m_overlay);
	overlay_layout->setContentsMargins(0, 0, 0, 0);
	overlay_layout->setSpacing(0);
	overlay_layout->addSpacing(10);
	overlay_layout->addWidget(m_definitions_button);
	overlay_layout->addStretch();
	overlay_layout->addWidget(m_clock, 0, Qt::AlignCenter);
	overlay_layout->addStretch();
	overlay_layout->addWidget(m_hint_button);
	overlay_layout->addSpacing(10);

	// Lay out board
	QGridLayout* layout = new QGridLayout(contents);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(view, 0, 0);
	layout->addWidget(m_success, 0, 0, Qt::AlignCenter);
	layout->addWidget(m_overlay, 0, 0, Qt::AlignHCenter | Qt::AlignTop);

	// Create menus
	QMenu* menu = menuBar()->addMenu(tr("&Game"));
	QAction* action = menu->addAction(tr("&New"), this, &Window::newGame);
	action->setShortcut(QKeySequence::New);
	action = menu->addAction(tr("&Choose..."), this, &Window::chooseGame);
	action->setShortcut(QKeySequence::Open);
	menu->addSeparator();
	m_pause_action = menu->addAction(tr("&Pause"), m_board, &Board::setPaused);
	m_pause_action->setShortcut(tr("P"));
	m_pause_action->setCheckable(true);
	m_pause_action->setDisabled(true);
	action = menu->addAction(tr("&Hint"), m_board, &Board::showHint);
	action->setShortcut(tr("H"));
	action->setDisabled(true);
	connect(m_board, &Board::hintAvailable, action, &QAction::setEnabled);
	m_definitions_action = menu->addAction(tr("D&efinitions"), this, &Window::showDefinitions);
	m_definitions_action->setShortcut(tr("D"));
	m_definitions_action->setDisabled(true);
	menu->addSeparator();
	m_details_action = menu->addAction(tr("&Details"), this, &Window::showDetails);
	m_details_action->setDisabled(true);
	action = menu->addAction(tr("High &Scores"), this, &Window::showScores);
	action->setShortcut(tr("Ctrl+H"));
	menu->addSeparator();
	action = menu->addAction(tr("&Quit"), qApp, &QApplication::quit);
	action->setShortcut(QKeySequence::Quit);
	action->setMenuRole(QAction::QuitRole);

	menu = menuBar()->addMenu(tr("&Settings"));
	QAction* dark_mode = menu->addAction(tr("&Dark Mode"), m_board, &Board::setDarkMode);
	dark_mode->setCheckable(true);
	menu->addSeparator();
	menu->addAction(tr("Application &Language..."), this, &Window::setLocale);

	menu = menuBar()->addMenu(tr("&Help"));
	action = menu->addAction(tr("&About"), this, &Window::about);
	action->setMenuRole(QAction::AboutRole);
	action = menu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
	action->setMenuRole(QAction::AboutQtRole);

	// Restore window geometry
	QSettings settings;
	resize(800, 600);
	restoreGeometry(settings.value("Geometry").toByteArray());

	// Set dark mode
	if (settings.value("DarkMode", false).toBool()) {
		dark_mode->trigger();
	}

	// Continue previous or start new game
	show();
	if (!m_board->continueGame()) {
		newGame();
	}
}

//-----------------------------------------------------------------------------

Window::~Window()
{
	m_board->saveGame();
}

//-----------------------------------------------------------------------------

void Window::newGame()
{
	NewGameDialog dialog(m_board, this);
	dialog.exec();
}

//-----------------------------------------------------------------------------

void Window::chooseGame()
{
	bool ok = false;
	QString number = QInputDialog::getText(this, tr("Choose Game"), tr("Game Number:"), QLineEdit::Normal, QString(), &ok);
	if (ok && !number.isEmpty()) {
		number = number.trimmed();
		if (!m_board->openGame(number)) {
			QMessageBox::warning(this, tr("Sorry"), tr("Unable to start requested game."));
		}
	}
}

//-----------------------------------------------------------------------------

void Window::closeEvent(QCloseEvent* event)
{
	m_board->saveGame();
	QSettings().setValue("Geometry", saveGeometry());
	QMainWindow::closeEvent(event);
}

//-----------------------------------------------------------------------------

bool Window::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress) {
		if (object == m_definitions_button) {
			showDefinitions();
			return true;
		} else if (object == m_hint_button) {
			m_board->showHint();
			return true;
		}
	}
	return QMainWindow::eventFilter(object, event);
}

//-----------------------------------------------------------------------------

void Window::about()
{
	QMessageBox::about(this, tr("About"), QString("<p><center><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></center></p><p><center>%6</center></p>")
		.arg(tr("Connectagram"), QCoreApplication::applicationVersion(),
			tr("A word unscrambling game"),
			tr("Copyright &copy; 2009-%1 by Graeme Gott").arg("2026"),
			tr("Released under the <a href=\"http://www.gnu.org/licenses/gpl.html\">GPL 3</a> license"),
			tr("Definitions are from <a href=\"http://wiktionary.org/\">Wiktionary</a>"))
	);
}

//-----------------------------------------------------------------------------

void Window::showDefinitions()
{
	const int height = m_definitions_button->height();
	const QPoint pos = m_definitions_button->mapToGlobal(QPoint(0, height));
	m_definitions->popup(pos);
}

//-----------------------------------------------------------------------------

void Window::showDetails()
{
	Pattern* pattern = m_board->pattern();
	if (!pattern) {
		return;
	}
	QMessageBox dialog(QMessageBox::Information,
		tr("Details"),
		QString("<p><b>%1</b> %2<br><b>%3</b> %4<br><b>%5</b> %6<br><b>%7</b> %8<br><b>%9</b> %10</p>")
			.arg(tr("Pattern:")
				, pattern->name()
				, NewGameDialog::tr("Language:")
				, WordList::languageName(m_board->words()->language())
				, NewGameDialog::tr("Amount of Words:")
				, NewGameDialog::densityString(pattern->wordCount())
				, NewGameDialog::tr("Word Length:")
				, NewGameDialog::tr("%n letter(s)", "", pattern->wordLength() + 1)
				, tr("Game Number:"))
			.arg(m_board->gameNumber()),
		QMessageBox::NoButton,
		this);
	dialog.setIconPixmap(QIcon(QString(":/patterns/%1.png").arg(pattern->type())).pixmap(96,96));
	dialog.exec();
}

//-----------------------------------------------------------------------------

void Window::showScores()
{
	ScoresDialog scores(this);
	scores.exec();
}

//-----------------------------------------------------------------------------

void Window::setLocale()
{
	LocaleDialog dialog(this);
	dialog.exec();
}

//-----------------------------------------------------------------------------

void Window::gameStarted()
{
	m_overlay->show();
	m_clock->setEnabled(true);
	m_clock->start();
	m_pause_action->setEnabled(true);
	m_definitions_button->setEnabled(true);
	m_definitions_action->setEnabled(true);
	m_details_action->setEnabled(true);
	m_success->hide();
}

//-----------------------------------------------------------------------------

void Window::gameFinished()
{
	QSettings settings;
	const int count = settings.value("Current/Count").toInt();
	const int length = settings.value("Current/Length").toInt();
	const int msecs = settings.value("Current/Time").toInt();
	settings.remove("Current");

	m_clock->stop();
	m_clock->setDisabled(true);
	m_pause_action->setDisabled(true);
	m_success->show();

	ScoresDialog scores(this);
	if (scores.addScore(std::lround(msecs / 1000.0), count, length)) {
		scores.exec();
	}
}

//-----------------------------------------------------------------------------

void Window::gamePauseChanged()
{
	bool paused = m_board->isPaused();
	m_pause_action->setChecked(paused);
	m_clock->setPaused(paused);
}

//-----------------------------------------------------------------------------
