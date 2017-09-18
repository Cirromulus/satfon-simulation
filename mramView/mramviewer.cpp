#include "mramviewer.h"

#include <QtWidgets/QApplication>


MramViewer::MramViewer(MramDebugInterfaceClient* mdbgIf, QWidget *mparent)
	: QWidget(mparent), dbgIf(mdbgIf)
{

	ui.setupUi(this);
	resize(dbgIf->getBlockWidth() * size_factor,
			(headerHeight + heightInPixels) * size_factor);
	setFixedSize(size());
}

MramViewer::~MramViewer()
{

}

void MramViewer::drawMainPage(QImage* mem){
	for (unsigned int block = 0; block < heightInPixels; block++){
		unsigned int address = activePage * blockWidth * heightInPixels + blockWidth * block;
		//printf("Block %u address %u-%u\n", block, address, address + blockWidth);
		if(address + blockWidth > dbgIf->getSize()){
			//Too far
			printf("Address %u-%u is too far\n", address, address + blockWidth);
			return;
		}
		unsigned char* value = dbgIf->getValue(address);
		if(value != NULL){
			for(unsigned int byte = 0; byte < blockWidth; byte++){
				unsigned char r = ((value[byte] & 0b00000111) << 4);
				unsigned char g = ((value[byte] & 0b00111000) << 1);
				unsigned char b = ((value[byte] & 0b11000000) >> 1);

				if(value[byte] != 0){
					r |= 0b10001111;
					g |= 0b10001111;
					b |= 0b10011111;
				}

				mem->setPixel(byte, block, qRgb(r,g,b));
			}
		}else{
			printf("Got null from Value\n");
			return;
		}
	}
}

void MramViewer::paintEvent(QPaintEvent *){
	QPainter painter(this);

	painter.scale(size_factor, size_factor);

	//Draw Header
	char planeNumber[20];
	sprintf(planeNumber, "Page: %02u/%02u", activePage + 1,
			dbgIf->getSize() / blockWidth / heightInPixels);
	char headerViewtype[50];
	sprintf(headerViewtype, "Size: %u Byte", dbgIf->getSize());

	QFont f = painter.font();
	f.setPointSize(11);
	painter.setFont(f);
	painter.setPen(Qt::black);
	painter.drawText(QRect(headerPadding, headerPadding/2, MIN_WIDTH - headerPadding, headerHeight - headerPadding),
			Qt::AlignVCenter | Qt::AlignLeft, headerViewtype);
	painter.drawText(QRect(MIN_WIDTH/2 - headerPadding, headerPadding/2,
			MIN_WIDTH/2 - 2*headerPadding, headerHeight - headerPadding),
			Qt::AlignVCenter | Qt::AlignRight, planeNumber);

	
	//QPainter mempaint(&memory);
	if(dbgIf->isConnected()){
		rescaleWindow();
		QImage memory(dbgIf->getBlockWidth(), heightInPixels, QImage::Format_RGB444);
		drawMainPage(&memory);
		painter.drawImage(QPoint(0, headerHeight), memory);
	}else{
		painter.save();
		painter.setBrush(Qt::darkRed);

		painter.drawRect(helpMargin + (MIN_WIDTH - 2 * helpMargin)/4, helpMargin + (MIN_HEIGHT - 2 * helpMargin)/4, (MIN_WIDTH - 2 * helpMargin)/2, (MIN_HEIGHT - 2 * helpMargin)/2);
		painter.drawText(QRect(helpMargin + helpPadding, helpMargin + helpPadding,
							MIN_WIDTH - 2 * (helpMargin + helpPadding),
							MIN_HEIGHT - 2 * (helpMargin + helpPadding)),
			Qt::AlignVCenter | Qt::AlignHCenter, QString::fromUtf8("NOT CONNECTED"));
		painter.restore();
	}



	if (help_sign){
		painter.save();
		painter.setBrush(Qt::darkGray);
		
		painter.drawRect(helpMargin, helpMargin, MIN_WIDTH - 2 * helpMargin, MIN_HEIGHT - 2 * helpMargin);
		painter.drawText(QRect(helpMargin + helpPadding, helpMargin + helpPadding,
							MIN_WIDTH - 2 * (helpMargin + helpPadding),
							MIN_HEIGHT - 2 * (helpMargin + helpPadding)),
			Qt::AlignVCenter | Qt::AlignLeft, QString::fromUtf8(helptext));
		painter.restore();
	}

	painter.end();

	update();
}

void MramViewer::rescaleWindow(bool force){
	unsigned int w = dbgIf->getBlockWidth();
	if(w == blockWidth && !force){
		//nothing changed
		return;
	}
	if(w == 0){
		printf("Something is wrong with the size values.\n"
		"Got w: %d\n", w);
		return;
	}
	blockWidth = w;
	int h = headerHeight + heightInPixels;
	w *= size_factor;
	h *= size_factor;
	setFixedSize(QSize(w,h));
	resize(w,h);
}

void MramViewer::keyPressEvent(QKeyEvent *e)
{
	switch (e->key()){
	case Qt::Key_Right:
		activePage = (activePage + 1) >
			(dbgIf->getSize() - 1) / blockWidth / heightInPixels ?
					0 : activePage + 1;
		break;
	case Qt::Key_Left:
		activePage = static_cast<long>(activePage) - 1 < 0 ?
				(dbgIf->getSize() - 1) / blockWidth / heightInPixels : activePage - 1;
		break;
	case Qt::Key_H:
		help_sign = !help_sign;
		break;
	case Qt::Key_G:
	{
		size_factor = size_factor + .5 > 3 ? 1 : size_factor + .5;
		rescaleWindow(true);
		break;
	}
	case Qt::Key_Q:
		close();
		break;
	default:
		printf("Nuffin key: %d\n", e->key());
	}
	update();
}
