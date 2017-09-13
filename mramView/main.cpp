#include <QtWidgets/QApplication>

#include "mramviewer.h"

int main(int argc, char *argv[])
{
	FlashDebugInterfaceClient dbgIf;

	QApplication a (argc, argv);
	MramViewer w (&dbgIf);
	w.show();


	return a.exec();
}
