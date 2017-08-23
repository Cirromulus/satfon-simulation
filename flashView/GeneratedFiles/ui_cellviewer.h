/********************************************************************************
** Form generated from reading UI file 'cellviewer.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CELLVIEWER_H
#define UI_CELLVIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cellViewerClass
{
public:

    void setupUi(QWidget *cellViewerClass)
    {
        if (cellViewerClass->objectName().isEmpty())
            cellViewerClass->setObjectName(QStringLiteral("cellViewerClass"));
        cellViewerClass->resize(600, 400);

        retranslateUi(cellViewerClass);

        QMetaObject::connectSlotsByName(cellViewerClass);
    } // setupUi

    void retranslateUi(QWidget *cellViewerClass)
    {
        cellViewerClass->setWindowTitle(QApplication::translate("cellViewerClass", "cellViewer", 0));
    } // retranslateUi

};

namespace Ui {
    class cellViewerClass: public Ui_cellViewerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CELLVIEWER_H
