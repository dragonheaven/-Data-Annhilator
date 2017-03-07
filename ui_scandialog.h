/********************************************************************************
** Form generated from reading UI file 'scandialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCANDIALOG_H
#define UI_SCANDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ScanDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLabel *currentItem;

    void setupUi(QDialog *ScanDialog)
    {
        if (ScanDialog->objectName().isEmpty())
            ScanDialog->setObjectName(QString::fromUtf8("ScanDialog"));
        ScanDialog->resize(368, 134);
        QFont font;
        font.setPointSize(12);
        ScanDialog->setFont(font);
        verticalLayout = new QVBoxLayout(ScanDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(ScanDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        currentItem = new QLabel(ScanDialog);
        currentItem->setObjectName(QString::fromUtf8("currentItem"));
        currentItem->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(currentItem);


        retranslateUi(ScanDialog);

        QMetaObject::connectSlotsByName(ScanDialog);
    } // setupUi

    void retranslateUi(QDialog *ScanDialog)
    {
        ScanDialog->setWindowTitle(QApplication::translate("ScanDialog", "Scanning system", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ScanDialog", "Scanning", 0, QApplication::UnicodeUTF8));
        currentItem->setText(QApplication::translate("ScanDialog", "<current>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ScanDialog: public Ui_ScanDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCANDIALOG_H
