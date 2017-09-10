#include "flashviewer.h"
#include <QtWidgets/QApplication>

const char *viewTypeString[] = { "Wear", "Value", "Wear/value mixed" };


flashViewer::flashViewer(FlashDebugInterfaceClient* mdbgIf, QWidget *mparent)
	: QWidget(mparent), dbgIf(mdbgIf)
{
	//experimental
	/*auto f = std::bind(&flashViewer::notifyChange, this);
	dbgIf->registerOnChangeFunction(f);*/


	ui.setupUi(this);
	resize(dbgIf->getPageSize() * size_factor, (headerHeight + (dbgIf->getBlockSize() + 1)*dbgIf->getPlaneSize()) * size_factor);
	setFixedSize(size());
}

flashViewer::~flashViewer()
{

}

void flashViewer::drawMainPage(QImage* mem){
	int pageSize  = dbgIf->getPageSize();
	int blockSize = dbgIf->getBlockSize();
	int planeSize = dbgIf->getPlaneSize();

	for (int block = 0; block < planeSize; block++){
		for (int page = 0; page <= blockSize; page++){
			if(page < blockSize){
				switch (view){
				case USAGE:
				{
					AccessValues* av = dbgIf->getAccessValues(active_plane, block, page);
					Failpoint* fv = dbgIf->getFailpoint(active_plane, block, page);
					unsigned int* dose = dbgIf->getRadiationDose(active_plane, block, page);
					if(av != NULL && fv != NULL && dose != NULL){
						for(int byte = 0; byte < pageSize; byte++){
							int r, g, b;
							r = (av[byte].times_read / 10000.) * 255;
							r = r > 255 ? 255 : r;
							g = (dose[byte] / static_cast<float>(fv[byte].rad)) * 255;
							g = g > 255 ? 255 : g;
							b = (av[byte].times_reset / static_cast<float>(fv[byte].erases)) * 255;
							b = b > 255 ? 255 : b;
							mem->setPixel(byte, (block * (blockSize + 1)) + page, qRgb(r, g, b));
						}
					}else{
						//ok, we are not connected
						return;
					}
					break;
				}
				case VALUE:
				{
					DATA_TYPE* value = dbgIf->getValue(active_plane, block, page);
					if(value != NULL){
						for(int byte = 0; byte < pageSize; byte++){
							unsigned char r = ((value[byte] & 0b00000111) << 4);
							unsigned char g = ((value[byte] & 0b00111000) << 1);
							unsigned char b = ((value[byte] & 0b11000000) >> 1);

							if(value[byte] != 0){
								r |= 0b10001111;
								g |= 0b10001111;
								b |= 0b10011111;
							}

							mem->setPixel(byte, (block * (blockSize + 1)) + page, qRgb(r,g,b));
						}
					}else{
						return;
					}
					break;
				}
				case MIXED:
				{
					DATA_TYPE* value = dbgIf->getValue(active_plane, block, page);
					AccessValues* av = dbgIf->getAccessValues(active_plane, block, page);
					Failpoint* fv = dbgIf->getFailpoint(active_plane, block, page);
					DATA_TYPE* latchmask = dbgIf->getLatchMask(active_plane, block, page);
					if(value != NULL && av != NULL && fv != NULL && latchmask != NULL){
						for(int byte = 0; byte < pageSize; byte++){
							int b = (av[byte].times_reset / static_cast<float>(fv[byte].erases)) * 255;
							mem->setPixel(byte, (block * (blockSize + 1)) + page, qRgb(b, value[byte], latchmask[byte]));
						}
					}else{
						return;
					}
					break;
				}
				default:
					for(int byte = 0; byte < pageSize; byte++){
						mem->setPixel(byte, (block * (blockSize + 1)) + page, qRgb(0xFF, 0, 0));
					}
				}
			}else{
				for (int f_byte = 0; f_byte < pageSize; f_byte++){
					mem->setPixel(f_byte, (block * (blockSize + 1)) + page, qRgb(35, 35, 35));
				}
			}
		}
	}
}

void flashViewer::paintEvent(QPaintEvent *){
	QPainter painter(this);

	painter.scale(size_factor, size_factor);

	//Draw Header
	char planeNumber[20];
	sprintf(planeNumber, "Plane: %d", active_plane);
	char headerViewtype[50];
	sprintf(headerViewtype, "Display type: %s", viewTypeString[view]);

	QFont f = painter.font();
	f.setPointSize(11);
	painter.setFont(f);
	painter.setPen(Qt::black);
	painter.drawText(QRect(headerPadding, headerPadding/2, MIN_WIDTH - headerPadding, headerHeight - headerPadding),
			Qt::AlignVCenter | Qt::AlignLeft, headerViewtype);
	painter.drawText(QRect(MIN_WIDTH/2 + headerPadding, headerPadding/2,
			MIN_WIDTH/2 - 2*headerPadding, headerHeight - headerPadding),
			Qt::AlignVCenter | Qt::AlignRight, planeNumber);

	
	//QPainter mempaint(&memory);
	if(dbgIf->isConnected()){
		rescaleWindow();
		QImage memory(dbgIf->getPageSize(), (dbgIf->getBlockSize() + 1)*dbgIf->getPlaneSize(), QImage::Format_RGB444);
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

void flashViewer::rescaleWindow(){
	int w = dbgIf->getPageSize();
	int h = headerHeight + (dbgIf->getBlockSize() + 1)*dbgIf->getPlaneSize();
	w *= size_factor;
	h *= size_factor;
	if(w > 0 && h > 0){
		setFixedSize(QSize(w,h));
		resize(w,h);
	}else{
		printf("Something is wrong with the size values.\n"
		"Got w: %d, got h: %d\n", w, h);
	}
}

void flashViewer::keyPressEvent(QKeyEvent *e)
{
	switch (e->key()){
	case Qt::Key_Space:
	{
		int intview = static_cast<int>(view);
		intview = intview + 1 >= static_cast<int>(_size) ? 0 : intview + 1;
		view = static_cast<viewType>(intview);
		break;
	}
	case Qt::Key_S:
		randomize();
		break;
	case Qt::Key_Right:
		active_plane = (active_plane + 1) >= dbgIf->getCellSize() ? active_plane : active_plane + 1;
		break;
	case Qt::Key_Left:
		active_plane = static_cast<long>(active_plane) - 1 < 0 ? active_plane : active_plane - 1;
		break;
	case Qt::Key_H:
		help_sign = !help_sign;
		break;
	case Qt::Key_G:
	{
		size_factor = size_factor + .5 > 3 ? 1 : size_factor + .5;
		rescaleWindow();
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

void flashViewer::randomize(){
/*	printf("Random reading/writing Pages...\n");
	int times = rand() % 50;
	for (int i = 0; i < times; i++){
		int block = rand() % dbgIf->getPlaneSize();
		int page = rand() % dbgIf->getBlockSize();
		int f_byte = rand() % dbgIf->getPageSize();
	
		DATA_TYPE *buff = new DATA_TYPE[dbgIf->getPageSize()];
		dbgIf->getCell()->readPage(active_plane, block, page, buff);
		if (rand() % 2){
			buff[f_byte] = rand() % 255;
			dbgIf->getCell()->writePage(active_plane, block, page, buff);
		}
		delete[] buff;
	}
	*/
}

void flashViewer::notifyChange(){
	update();
}
