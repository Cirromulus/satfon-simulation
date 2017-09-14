#ifndef CELLVIEWER_H
#define CELLVIEWER_H

#define MIN_HEIGHT (590/2)
#define MIN_WIDTH (1056/2)

#include <QWidget>
#include <QtWidgets>
#include <random>
#include <string>
#include <ui_cellviewer.h>
#include "../simu/mramDebugInterfaceClient.hpp"

class MramViewer : public QWidget
{
	Q_OBJECT
	unsigned int blockWidth = 512;
	unsigned int heightInPixels = 512;
public:
	MramViewer(MramDebugInterfaceClient* mdbgif, QWidget *mparent = 0);
	~MramViewer();
	void paintEvent(QPaintEvent *);
	void keyPressEvent(QKeyEvent *e);
	void drawMainPage(QImage* mem);
private:
	Ui::cellViewerClass ui;
	MramDebugInterfaceClient *dbgIf;
	unsigned int activePage = 0;
	bool help_sign = false;
	double size_factor = 1;
	unsigned int headerHeight = 30;
	unsigned int headerPadding = 10;
	unsigned int helpMargin = 50;
	unsigned int helpPadding = 5;
	const char *helptext = {
"Rechte Pfeiltaste - Plane hochschalten\n\
Linke Pfeiltaste - Plane herunterschalten\n\
'G' - Größe umschalten\n\
'H' - Diesen Hilfetext umschalten\n\
'Q' - MramViewer beenden\n\
" };
	void rescaleWindow(bool force = false);
};

#endif // CELLVIEWER_H
