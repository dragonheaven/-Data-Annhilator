/********************************************************************************
** Form generated from reading UI file 'confirmdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIRMDIALOG_H
#define UI_CONFIRMDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_ConfirmDlg
{
public:
    QLabel *label;
    QPushButton *yesButt;
    QPushButton *noButt;
    QLabel *tooMuchTime;
    QSpinBox *nrPasses;
    QLabel *label_2;

    void setupUi(QDialog *ConfirmDlg)
    {
        if (ConfirmDlg->objectName().isEmpty())
            ConfirmDlg->setObjectName(QString::fromUtf8("ConfirmDlg"));
        ConfirmDlg->resize(553, 202);
        ConfirmDlg->setModal(true);
        label = new QLabel(ConfirmDlg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 251, 131));
        label->setWordWrap(true);
        yesButt = new QPushButton(ConfirmDlg);
        yesButt->setObjectName(QString::fromUtf8("yesButt"));
        yesButt->setGeometry(QRect(20, 160, 98, 27));
        noButt = new QPushButton(ConfirmDlg);
        noButt->setObjectName(QString::fromUtf8("noButt"));
        noButt->setGeometry(QRect(130, 160, 98, 27));
        tooMuchTime = new QLabel(ConfirmDlg);
        tooMuchTime->setObjectName(QString::fromUtf8("tooMuchTime"));
        tooMuchTime->setGeometry(QRect(320, 70, 181, 61));
        tooMuchTime->setTextFormat(Qt::RichText);
        tooMuchTime->setWordWrap(true);
        nrPasses = new QSpinBox(ConfirmDlg);
        nrPasses->setObjectName(QString::fromUtf8("nrPasses"));
        nrPasses->setGeometry(QRect(410, 20, 61, 41));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(nrPasses->sizePolicy().hasHeightForWidth());
        nrPasses->setSizePolicy(sizePolicy);
        nrPasses->setMinimumSize(QSize(0, 0));
        nrPasses->setMinimum(1);
        nrPasses->setMaximum(7);
        label_2 = new QLabel(ConfirmDlg);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(320, 30, 82, 17));
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);
        label->raise();
        yesButt->raise();
        noButt->raise();
        tooMuchTime->raise();

        retranslateUi(ConfirmDlg);

        QMetaObject::connectSlotsByName(ConfirmDlg);
    } // setupUi

    void retranslateUi(QDialog *ConfirmDlg)
    {
        ConfirmDlg->setWindowTitle(QApplication::translate("ConfirmDlg", "Confirm annihilation", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ConfirmDlg", "<html><head/><body><p>Are you sure you want to wipe these disks ? If you click YES, all of your data will be <span style=\" font-weight:600;\">PERMANENTLY DELETED</span>.</p><p><br/></p><p>Do you want to wipe ?</p></body></html>", 0, QApplication::UnicodeUTF8));
        yesButt->setText(QApplication::translate("ConfirmDlg", "&YES", 0, QApplication::UnicodeUTF8));
        noButt->setText(QApplication::translate("ConfirmDlg", "&NO", 0, QApplication::UnicodeUTF8));
        tooMuchTime->setText(QApplication::translate("ConfirmDlg", "MSG", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ConfirmDlg", "Nr of passes", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ConfirmDlg: public Ui_ConfirmDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIRMDIALOG_H
