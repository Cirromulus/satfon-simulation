/********************************************************************************
** Form generated from reading UI file 'mramviewer.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#pragma once

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mramViewerClass
{
public:

    void setupUi(QWidget *mramViewerClass)
    {
        if (mramViewerClass->objectName().isEmpty())
            mramViewerClass->setObjectName(QStringLiteral("mramViewerClass"));
        mramViewerClass->resize(600, 400);

        retranslateUi(mramViewerClass);

        QMetaObject::connectSlotsByName(mramViewerClass);
    } // setupUi

    void retranslateUi(QWidget *mramViewerClass)
    {
        mramViewerClass->setWindowTitle(QApplication::translate("mramViewerClass", "mramViewer", 0));
    } // retranslateUi

};

namespace Ui {
    class mramViewerClass: public Ui_mramViewerClass {};
} // namespace Ui

QT_END_NAMESPACE
