/********************************************************************************
** Form generated from reading UI file 'histdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HISTDIALOG_H
#define UI_HISTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_HistDialog
{
public:
    QPushButton *done;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QPlainTextEdit *hist;
    QPushButton *ui_clear;

    void setupUi(QDialog *HistDialog)
    {
        if (HistDialog->objectName().isEmpty())
            HistDialog->setObjectName(QString::fromUtf8("HistDialog"));
        HistDialog->resize(846, 407);
        done = new QPushButton(HistDialog);
        done->setObjectName(QString::fromUtf8("done"));
        done->setGeometry(QRect(390, 360, 98, 31));
        groupBox = new QGroupBox(HistDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 40, 821, 301));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        hist = new QPlainTextEdit(groupBox);
        hist->setObjectName(QString::fromUtf8("hist"));

        horizontalLayout->addWidget(hist);

        ui_clear = new QPushButton(HistDialog);
        ui_clear->setObjectName(QString::fromUtf8("ui_clear"));
        ui_clear->setGeometry(QRect(740, 10, 98, 20));

        retranslateUi(HistDialog);

        QMetaObject::connectSlotsByName(HistDialog);
    } // setupUi

    void retranslateUi(QDialog *HistDialog)
    {
        HistDialog->setWindowTitle(QApplication::translate("HistDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        done->setText(QApplication::translate("HistDialog", "&Done", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("HistDialog", "History", 0, QApplication::UnicodeUTF8));
        ui_clear->setText(QApplication::translate("HistDialog", "&Clear history", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class HistDialog: public Ui_HistDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HISTDIALOG_H
