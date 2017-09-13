/*
 * mramViewer.hpp
 *
 *  Created on: 10.09.2017
 *      Author: urinator
 */

#ifndef FLASHVIEW_MRAMVIEWER_HPP_
#define FLASHVIEW_MRAMVIEWER_HPP_



#define MIN_HEIGHT (590/2)
#define MIN_WIDTH (1056/2)

#include <QWidget>
#include <QtWidgets>
#include <random>
#include <string>
#include <ui_cellviewer.h>
#include "../simu/mram.hpp"
#include "../simu/flashDebugInterfaceClient.hpp"

enum viewType { USAGE, VALUE, MIXED, _size};	//Auch die Strings in cellviewer.cpp ändern!

class MramViewer : public QWidget
{
	Q_OBJECT

public:
	MramViewer(FlashDebugInterfaceClient* mdbgif, QWidget *mparent = 0);
	~MramViewer();
	void paintEvent(QPaintEvent *);
	void keyPressEvent(QKeyEvent *e);
	void drawMainPage(QImage* mem);

	void notifyChange();
private:
	Ui::cellViewerClass ui;
	FlashDebugInterfaceClient *dbgIf;
	viewType view = VALUE;
	unsigned int active_plane = 0;
	bool help_sign = false;
	double size_factor = 1;
	unsigned int headerHeight = 30;
	unsigned int headerPadding = 10;
	unsigned int helpMargin = 50;
	unsigned int helpPadding = 5;
	const char *helptext = {
"Linke / Rechte Pfeiltaste - Navigieren\n\
'G' - Größe umschalten\n\
'H' - Diesen Hilfetext umschalten\n\
'Q' - beenden\n\
" };
	void rescaleWindow();
	void randomize();
};



#endif /* FLASHVIEW_MRAMVIEWER_HPP_ */
