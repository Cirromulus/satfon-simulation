#ifndef CELLVIEWER_H
#define CELLVIEWER_H

#define MIN_HEIGHT (590/2)
#define MIN_WIDTH (1056/2)

#include <QWidget>
#include <QtWidgets>
#include <random>
#include <string>
#include "ui_cellviewer.h"
#include "../simu/flashCell.h"
#include "../simu/debugInterfaceClient.hpp"

enum viewType { USAGE, VALUE, MIXED, _size};	//Auch die Strings in cellviewer.cpp ändern!

class flashViewer : public QWidget
{
	Q_OBJECT

public:
	flashViewer(debugInterfaceClient* mdbgif, QWidget *mparent = 0);
	~flashViewer();
	void paintEvent(QPaintEvent *);
	void keyPressEvent(QKeyEvent *e);
	void drawMainPage(QImage* mem);

	void notifyChange();
private:
	Ui::cellViewerClass ui;
	debugInterfaceClient *dbgIf;
	viewType view = VALUE;
	unsigned int active_plane = 0;
	bool help_sign = false;
	double size_factor = 1;
	unsigned int headerHeight = 30;
	unsigned int headerPadding = 10;
	unsigned int helpMargin = 50;
	unsigned int helpPadding = 5;
	const char *helptext = {
"Leertaste - Ansicht umschalten\n\
Rechte Pfeiltaste - Plane hochschalten\n\
Linke Pfeiltaste - Plane herunterschalten\n\
'S' - Zufällige Werte in die ausgewählte Plane schreiben\n\
'G' - Größe umschalten\n\
'H' - Diesen Hilfetext umschalten\n\
'Q' - CellViewer beenden\n\
" };

	void randomize();
};

#endif // CELLVIEWER_H
