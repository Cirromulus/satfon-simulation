#include <QtWidgets/QApplication>
#include "flashviewer.h"

int main(int argc, char *argv[])
{
	debugInterfaceClient dbgIf;

	QApplication a (argc, argv);
	flashViewer w (&dbgIf);
	w.show();


	return a.exec();
}
