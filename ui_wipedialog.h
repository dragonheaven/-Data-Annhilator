/********************************************************************************
** Form generated from reading UI file 'wipedialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIPEDIALOG_H
#define UI_WIPEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WipeDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QLabel *disksWiped;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QScrollArea *diskList;
    QWidget *scrollAreaWidgetContents;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_17;
    QProgressBar *totalProgress;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLabel *totalThroughputLbl;
    QLabel *label_22;
    QLabel *totalTime;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_18;
    QLabel *wipeErrors;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *cancelButton;

    void setupUi(QDialog *WipeDialog)
    {
        if (WipeDialog->objectName().isEmpty())
            WipeDialog->setObjectName(QString::fromUtf8("WipeDialog"));
        WipeDialog->resize(579, 503);
        verticalLayout_3 = new QVBoxLayout(WipeDialog);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label = new QLabel(WipeDialog);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_4->addWidget(label);

        disksWiped = new QLabel(WipeDialog);
        disksWiped->setObjectName(QString::fromUtf8("disksWiped"));
        disksWiped->setFrameShape(QFrame::Panel);
        disksWiped->setFrameShadow(QFrame::Sunken);

        horizontalLayout_4->addWidget(disksWiped);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);


        verticalLayout_3->addLayout(horizontalLayout_4);

        groupBox = new QGroupBox(WipeDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        diskList = new QScrollArea(groupBox);
        diskList->setObjectName(QString::fromUtf8("diskList"));
        diskList->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 533, 259));
        diskList->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(diskList);


        verticalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(WipeDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_17 = new QLabel(groupBox_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        horizontalLayout_2->addWidget(label_17);

        totalProgress = new QProgressBar(groupBox_2);
        totalProgress->setObjectName(QString::fromUtf8("totalProgress"));
        totalProgress->setValue(24);

        horizontalLayout_2->addWidget(totalProgress);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_3->addWidget(label_2);

        totalThroughputLbl = new QLabel(groupBox_2);
        totalThroughputLbl->setObjectName(QString::fromUtf8("totalThroughputLbl"));
        totalThroughputLbl->setFrameShape(QFrame::Panel);
        totalThroughputLbl->setFrameShadow(QFrame::Sunken);

        horizontalLayout_3->addWidget(totalThroughputLbl);

        label_22 = new QLabel(groupBox_2);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        horizontalLayout_3->addWidget(label_22);

        totalTime = new QLabel(groupBox_2);
        totalTime->setObjectName(QString::fromUtf8("totalTime"));
        totalTime->setFrameShape(QFrame::Panel);
        totalTime->setFrameShadow(QFrame::Sunken);

        horizontalLayout_3->addWidget(totalTime);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout_2->addLayout(horizontalLayout_3);


        verticalLayout_3->addWidget(groupBox_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_18 = new QLabel(WipeDialog);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        horizontalLayout_5->addWidget(label_18);

        wipeErrors = new QLabel(WipeDialog);
        wipeErrors->setObjectName(QString::fromUtf8("wipeErrors"));
        wipeErrors->setFrameShape(QFrame::Panel);
        wipeErrors->setFrameShadow(QFrame::Sunken);

        horizontalLayout_5->addWidget(wipeErrors);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout_5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        cancelButton = new QPushButton(WipeDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        horizontalLayout->addWidget(cancelButton);


        verticalLayout_3->addLayout(horizontalLayout);


        retranslateUi(WipeDialog);

        QMetaObject::connectSlotsByName(WipeDialog);
    } // setupUi

    void retranslateUi(QDialog *WipeDialog)
    {
        WipeDialog->setWindowTitle(QApplication::translate("WipeDialog", "Wiping disks", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("WipeDialog", "Disks wiped", 0, QApplication::UnicodeUTF8));
        disksWiped->setText(QApplication::translate("WipeDialog", "<M/N>", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("WipeDialog", "Individual progress", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("WipeDialog", "Total progress", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("WipeDialog", "Progress", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("WipeDialog", "Throughput", 0, QApplication::UnicodeUTF8));
        totalThroughputLbl->setText(QApplication::translate("WipeDialog", "N/A", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("WipeDialog", "Completion time", 0, QApplication::UnicodeUTF8));
        totalTime->setText(QApplication::translate("WipeDialog", "N/A", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("WipeDialog", "Errors during wipe", 0, QApplication::UnicodeUTF8));
        wipeErrors->setText(QApplication::translate("WipeDialog", "<n>", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("WipeDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class WipeDialog: public Ui_WipeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIPEDIALOG_H
