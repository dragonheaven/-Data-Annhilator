/********************************************************************************
** Form generated from reading UI file 'maindialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINDIALOG_H
#define UI_MAINDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *logo;
    QWidget *sysinfo_retail;
    QVBoxLayout *verticalLayout_8;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_2;
    QLabel *vendor_2;
    QLabel *label_14;
    QLabel *label_19;
    QLabel *memory_2;
    QLabel *model_2;
    QLabel *label_18;
    QLabel *label_17;
    QLabel *processor_2;
    QLabel *hwserialnr_2;
    QLabel *label_16;
    QWidget *widget_1;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_9;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *wmodeMachine;
    QRadioButton *wmodeDisk;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_9;
    QComboBox *nrPasses;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *updateActions;
    QCheckBox *updateAuto;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label;
    QComboBox *maxWipers;
    QHBoxLayout *miscHLayout;
    QLabel *label_13;
    QLineEdit *nrDisks;
    QPushButton *rescanDisks;
    QSpacerItem *horizontalSpacer_6;
    QFrame *line;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_8;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_10;
    QLineEdit *userName;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_10;
    QGroupBox *machineInfo;
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QLabel *label_3;
    QLabel *vendor;
    QLabel *label_2;
    QLabel *model;
    QLabel *label_4;
    QLabel *processor;
    QLabel *label_7;
    QLabel *procs;
    QLabel *label_8;
    QLabel *cores;
    QLabel *label_11;
    QLabel *threads;
    QLabel *label_5;
    QLabel *memory;
    QLabel *label_6;
    QLabel *hwserialnr;
    QLabel *label_12;
    QLineEdit *serialnr;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QTableWidget *mediaTableWidget;
    QWidget *widget_retail;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_13;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *rtl_wipe;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_12;
    QGridLayout *gridLayout_3;
    QLabel *label_15;
    QLabel *uiAnnSN;
    QLabel *label_21;
    QLabel *uiNrWipes;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *rtl_history;
    QPushButton *rtl_credits;
    QPushButton *rtl_reboot;
    QWidget *widget_inhouse;
    QHBoxLayout *horizontalLayout_11;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *doWipe;
    QSpacerItem *horizontalSpacer;
    QPushButton *updDB;
    QPushButton *saveReport;
    QPushButton *viewError;
    QPushButton *quit;

    void setupUi(QDialog *MainDialog)
    {
        if (MainDialog->objectName().isEmpty())
            MainDialog->setObjectName(QString::fromUtf8("MainDialog"));
        MainDialog->resize(1333, 860);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/pics/icon.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        MainDialog->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(MainDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        logo = new QLabel(MainDialog);
        logo->setObjectName(QString::fromUtf8("logo"));
        logo->setMaximumSize(QSize(375, 150));
        logo->setPixmap(QPixmap(QString::fromUtf8(":/pics/logo2.jpg")));
        logo->setScaledContents(true);

        horizontalLayout_5->addWidget(logo);

        sysinfo_retail = new QWidget(MainDialog);
        sysinfo_retail->setObjectName(QString::fromUtf8("sysinfo_retail"));
        verticalLayout_8 = new QVBoxLayout(sysinfo_retail);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        groupBox_4 = new QGroupBox(sysinfo_retail);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy);
        gridLayout_2 = new QGridLayout(groupBox_4);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setSizeConstraint(QLayout::SetMinimumSize);
        vendor_2 = new QLabel(groupBox_4);
        vendor_2->setObjectName(QString::fromUtf8("vendor_2"));
        vendor_2->setMinimumSize(QSize(130, 0));
        vendor_2->setMaximumSize(QSize(16777215, 16777215));
        vendor_2->setFrameShape(QFrame::Panel);
        vendor_2->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(vendor_2, 0, 1, 1, 1);

        label_14 = new QLabel(groupBox_4);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout_2->addWidget(label_14, 1, 0, 1, 1);

        label_19 = new QLabel(groupBox_4);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        gridLayout_2->addWidget(label_19, 2, 0, 1, 1);

        memory_2 = new QLabel(groupBox_4);
        memory_2->setObjectName(QString::fromUtf8("memory_2"));
        memory_2->setMaximumSize(QSize(16777215, 16777215));
        memory_2->setFrameShape(QFrame::Panel);
        memory_2->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(memory_2, 1, 3, 1, 1);

        model_2 = new QLabel(groupBox_4);
        model_2->setObjectName(QString::fromUtf8("model_2"));
        model_2->setMinimumSize(QSize(0, 0));
        model_2->setFrameShape(QFrame::Panel);
        model_2->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(model_2, 0, 3, 1, 1);

        label_18 = new QLabel(groupBox_4);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setMinimumSize(QSize(0, 0));
        label_18->setMaximumSize(QSize(16777215, 16777215));
        label_18->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_18, 0, 2, 1, 1);

        label_17 = new QLabel(groupBox_4);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout_2->addWidget(label_17, 0, 0, 1, 1);

        processor_2 = new QLabel(groupBox_4);
        processor_2->setObjectName(QString::fromUtf8("processor_2"));
        processor_2->setFrameShape(QFrame::Panel);
        processor_2->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(processor_2, 1, 1, 1, 1);

        hwserialnr_2 = new QLabel(groupBox_4);
        hwserialnr_2->setObjectName(QString::fromUtf8("hwserialnr_2"));
        hwserialnr_2->setFrameShape(QFrame::Panel);
        hwserialnr_2->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(hwserialnr_2, 2, 1, 1, 1);

        label_16 = new QLabel(groupBox_4);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout_2->addWidget(label_16, 1, 2, 1, 1);


        verticalLayout_8->addWidget(groupBox_4);


        horizontalLayout_5->addWidget(sysinfo_retail);

        widget_1 = new QWidget(MainDialog);
        widget_1->setObjectName(QString::fromUtf8("widget_1"));
        verticalLayout_5 = new QVBoxLayout(widget_1);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        groupBox_3 = new QGroupBox(widget_1);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        sizePolicy.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        wmodeMachine = new QRadioButton(groupBox_3);
        wmodeMachine->setObjectName(QString::fromUtf8("wmodeMachine"));
        wmodeMachine->setChecked(false);

        verticalLayout_3->addWidget(wmodeMachine);

        wmodeDisk = new QRadioButton(groupBox_3);
        wmodeDisk->setObjectName(QString::fromUtf8("wmodeDisk"));
        wmodeDisk->setChecked(true);

        verticalLayout_3->addWidget(wmodeDisk);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_7->addWidget(label_9);

        nrPasses = new QComboBox(groupBox_3);
        nrPasses->setObjectName(QString::fromUtf8("nrPasses"));

        horizontalLayout_7->addWidget(nrPasses);


        verticalLayout_3->addLayout(horizontalLayout_7);


        horizontalLayout_9->addWidget(groupBox_3);

        groupBox = new QGroupBox(widget_1);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_4 = new QVBoxLayout(groupBox);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        updateActions = new QCheckBox(groupBox);
        updateActions->setObjectName(QString::fromUtf8("updateActions"));
        updateActions->setChecked(true);

        verticalLayout_4->addWidget(updateActions);

        updateAuto = new QCheckBox(groupBox);
        updateAuto->setObjectName(QString::fromUtf8("updateAuto"));
        updateAuto->setChecked(true);

        verticalLayout_4->addWidget(updateAuto);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_6->addWidget(label);

        maxWipers = new QComboBox(groupBox);
        maxWipers->setObjectName(QString::fromUtf8("maxWipers"));

        horizontalLayout_6->addWidget(maxWipers);


        verticalLayout_4->addLayout(horizontalLayout_6);


        horizontalLayout_9->addWidget(groupBox);


        verticalLayout_6->addLayout(horizontalLayout_9);

        miscHLayout = new QHBoxLayout();
        miscHLayout->setObjectName(QString::fromUtf8("miscHLayout"));
        label_13 = new QLabel(widget_1);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        miscHLayout->addWidget(label_13);

        nrDisks = new QLineEdit(widget_1);
        nrDisks->setObjectName(QString::fromUtf8("nrDisks"));
        nrDisks->setMaximumSize(QSize(50, 16777215));
        nrDisks->setReadOnly(true);

        miscHLayout->addWidget(nrDisks);

        rescanDisks = new QPushButton(widget_1);
        rescanDisks->setObjectName(QString::fromUtf8("rescanDisks"));
        rescanDisks->setAutoDefault(false);

        miscHLayout->addWidget(rescanDisks);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        miscHLayout->addItem(horizontalSpacer_6);


        verticalLayout_6->addLayout(miscHLayout);


        verticalLayout_5->addLayout(verticalLayout_6);


        horizontalLayout_5->addWidget(widget_1);


        verticalLayout_2->addLayout(horizontalLayout_5);

        line = new QFrame(MainDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line);

        widget_2 = new QWidget(MainDialog);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_8 = new QHBoxLayout(widget_2);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_10 = new QLabel(widget_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_3->addWidget(label_10);

        userName = new QLineEdit(widget_2);
        userName->setObjectName(QString::fromUtf8("userName"));

        horizontalLayout_3->addWidget(userName);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        horizontalLayout_8->addLayout(horizontalLayout_3);


        verticalLayout_2->addWidget(widget_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget_3 = new QWidget(MainDialog);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        horizontalLayout_10 = new QHBoxLayout(widget_3);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        machineInfo = new QGroupBox(widget_3);
        machineInfo->setObjectName(QString::fromUtf8("machineInfo"));
        machineInfo->setEnabled(true);
        machineInfo->setFlat(false);
        horizontalLayout = new QHBoxLayout(machineInfo);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_3 = new QLabel(machineInfo);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        vendor = new QLabel(machineInfo);
        vendor->setObjectName(QString::fromUtf8("vendor"));
        vendor->setMinimumSize(QSize(130, 0));
        vendor->setMaximumSize(QSize(16777215, 16777215));
        vendor->setFrameShape(QFrame::Panel);
        vendor->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(vendor, 0, 1, 1, 2);

        label_2 = new QLabel(machineInfo);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(0, 0));
        label_2->setMaximumSize(QSize(16777215, 16777215));
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(label_2, 0, 3, 1, 1);

        model = new QLabel(machineInfo);
        model->setObjectName(QString::fromUtf8("model"));
        model->setMinimumSize(QSize(0, 0));
        model->setFrameShape(QFrame::Panel);
        model->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(model, 0, 4, 1, 2);

        label_4 = new QLabel(machineInfo);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        processor = new QLabel(machineInfo);
        processor->setObjectName(QString::fromUtf8("processor"));
        processor->setFrameShape(QFrame::Panel);
        processor->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(processor, 1, 1, 1, 2);

        label_7 = new QLabel(machineInfo);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 1, 3, 1, 1);

        procs = new QLabel(machineInfo);
        procs->setObjectName(QString::fromUtf8("procs"));
        procs->setFrameShape(QFrame::Panel);
        procs->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(procs, 1, 4, 1, 2);

        label_8 = new QLabel(machineInfo);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(label_8, 2, 0, 1, 1);

        cores = new QLabel(machineInfo);
        cores->setObjectName(QString::fromUtf8("cores"));
        cores->setFrameShape(QFrame::Panel);
        cores->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(cores, 2, 1, 1, 2);

        label_11 = new QLabel(machineInfo);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(label_11, 2, 3, 1, 1);

        threads = new QLabel(machineInfo);
        threads->setObjectName(QString::fromUtf8("threads"));
        threads->setFrameShape(QFrame::Panel);
        threads->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(threads, 2, 4, 1, 2);

        label_5 = new QLabel(machineInfo);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 3, 0, 1, 1);

        memory = new QLabel(machineInfo);
        memory->setObjectName(QString::fromUtf8("memory"));
        memory->setMaximumSize(QSize(16777215, 16777215));
        memory->setFrameShape(QFrame::Panel);
        memory->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(memory, 3, 1, 1, 2);

        label_6 = new QLabel(machineInfo);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 3, 3, 1, 1);

        hwserialnr = new QLabel(machineInfo);
        hwserialnr->setObjectName(QString::fromUtf8("hwserialnr"));
        hwserialnr->setFrameShape(QFrame::Panel);
        hwserialnr->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(hwserialnr, 3, 4, 1, 2);

        label_12 = new QLabel(machineInfo);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setTextFormat(Qt::PlainText);

        gridLayout->addWidget(label_12, 4, 0, 1, 1);

        serialnr = new QLineEdit(machineInfo);
        serialnr->setObjectName(QString::fromUtf8("serialnr"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(serialnr->sizePolicy().hasHeightForWidth());
        serialnr->setSizePolicy(sizePolicy1);
        serialnr->setMinimumSize(QSize(0, 0));
        serialnr->setReadOnly(false);

        gridLayout->addWidget(serialnr, 4, 1, 1, 2);


        horizontalLayout->addLayout(gridLayout);


        horizontalLayout_10->addWidget(machineInfo);


        verticalLayout->addWidget(widget_3);

        groupBox_2 = new QGroupBox(MainDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        mediaTableWidget = new QTableWidget(groupBox_2);
        mediaTableWidget->setObjectName(QString::fromUtf8("mediaTableWidget"));

        horizontalLayout_2->addWidget(mediaTableWidget);


        verticalLayout->addWidget(groupBox_2);

        widget_retail = new QWidget(MainDialog);
        widget_retail->setObjectName(QString::fromUtf8("widget_retail"));
        widget_retail->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(widget_retail->sizePolicy().hasHeightForWidth());
        widget_retail->setSizePolicy(sizePolicy2);
        widget_retail->setMinimumSize(QSize(100, 100));
        verticalLayout_7 = new QVBoxLayout(widget_retail);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_4);

        rtl_wipe = new QPushButton(widget_retail);
        rtl_wipe->setObjectName(QString::fromUtf8("rtl_wipe"));
        rtl_wipe->setMinimumSize(QSize(0, 75));

        horizontalLayout_13->addWidget(rtl_wipe);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_5);


        verticalLayout_7->addLayout(horizontalLayout_13);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_15 = new QLabel(widget_retail);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout_3->addWidget(label_15, 0, 0, 1, 1);

        uiAnnSN = new QLabel(widget_retail);
        uiAnnSN->setObjectName(QString::fromUtf8("uiAnnSN"));
        uiAnnSN->setFrameShape(QFrame::Panel);
        uiAnnSN->setFrameShadow(QFrame::Sunken);

        gridLayout_3->addWidget(uiAnnSN, 0, 1, 1, 1);

        label_21 = new QLabel(widget_retail);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        gridLayout_3->addWidget(label_21, 1, 0, 1, 1);

        uiNrWipes = new QLabel(widget_retail);
        uiNrWipes->setObjectName(QString::fromUtf8("uiNrWipes"));
        uiNrWipes->setFrameShape(QFrame::Panel);
        uiNrWipes->setFrameShadow(QFrame::Sunken);

        gridLayout_3->addWidget(uiNrWipes, 1, 1, 1, 1);


        horizontalLayout_12->addLayout(gridLayout_3);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_3);

        rtl_history = new QPushButton(widget_retail);
        rtl_history->setObjectName(QString::fromUtf8("rtl_history"));

        horizontalLayout_12->addWidget(rtl_history);

        rtl_credits = new QPushButton(widget_retail);
        rtl_credits->setObjectName(QString::fromUtf8("rtl_credits"));

        horizontalLayout_12->addWidget(rtl_credits);

        rtl_reboot = new QPushButton(widget_retail);
        rtl_reboot->setObjectName(QString::fromUtf8("rtl_reboot"));

        horizontalLayout_12->addWidget(rtl_reboot);


        verticalLayout_7->addLayout(horizontalLayout_12);


        verticalLayout->addWidget(widget_retail);

        widget_inhouse = new QWidget(MainDialog);
        widget_inhouse->setObjectName(QString::fromUtf8("widget_inhouse"));
        horizontalLayout_11 = new QHBoxLayout(widget_inhouse);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        doWipe = new QPushButton(widget_inhouse);
        doWipe->setObjectName(QString::fromUtf8("doWipe"));
        doWipe->setAutoDefault(false);

        horizontalLayout_4->addWidget(doWipe);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        updDB = new QPushButton(widget_inhouse);
        updDB->setObjectName(QString::fromUtf8("updDB"));
        updDB->setAutoDefault(false);

        horizontalLayout_4->addWidget(updDB);

        saveReport = new QPushButton(widget_inhouse);
        saveReport->setObjectName(QString::fromUtf8("saveReport"));
        saveReport->setAutoDefault(false);

        horizontalLayout_4->addWidget(saveReport);

        viewError = new QPushButton(widget_inhouse);
        viewError->setObjectName(QString::fromUtf8("viewError"));
        viewError->setAutoDefault(false);

        horizontalLayout_4->addWidget(viewError);

        quit = new QPushButton(widget_inhouse);
        quit->setObjectName(QString::fromUtf8("quit"));
        quit->setAutoDefault(false);

        horizontalLayout_4->addWidget(quit);


        horizontalLayout_11->addLayout(horizontalLayout_4);


        verticalLayout->addWidget(widget_inhouse);


        verticalLayout_2->addLayout(verticalLayout);

        line->raise();
        widget_2->raise();
#ifndef QT_NO_SHORTCUT
        label_10->setBuddy(userName);
        label_12->setBuddy(serialnr);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(wmodeMachine, wmodeDisk);
        QWidget::setTabOrder(wmodeDisk, nrPasses);
        QWidget::setTabOrder(nrPasses, updateActions);
        QWidget::setTabOrder(updateActions, updateAuto);
        QWidget::setTabOrder(updateAuto, maxWipers);
        QWidget::setTabOrder(maxWipers, nrDisks);
        QWidget::setTabOrder(nrDisks, userName);
        QWidget::setTabOrder(userName, serialnr);
        QWidget::setTabOrder(serialnr, updDB);
        QWidget::setTabOrder(updDB, doWipe);
        QWidget::setTabOrder(doWipe, rescanDisks);
        QWidget::setTabOrder(rescanDisks, saveReport);
        QWidget::setTabOrder(saveReport, viewError);
        QWidget::setTabOrder(viewError, quit);

        retranslateUi(MainDialog);
        QObject::connect(updateAuto, SIGNAL(toggled(bool)), updateActions, SLOT(setEnabled(bool)));
        QObject::connect(updateAuto, SIGNAL(toggled(bool)), updateActions, SLOT(setChecked(bool)));

        QMetaObject::connectSlotsByName(MainDialog);
    } // setupUi

    void retranslateUi(QDialog *MainDialog)
    {
        MainDialog->setWindowTitle(QApplication::translate("MainDialog", "Green Pulse Data Annihilator", 0, QApplication::UnicodeUTF8));
        logo->setText(QString());
        groupBox_4->setTitle(QApplication::translate("MainDialog", "System Information", 0, QApplication::UnicodeUTF8));
        vendor_2->setText(QApplication::translate("MainDialog", "<vendor>", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("MainDialog", "Processor Type", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("MainDialog", "MFG SN", 0, QApplication::UnicodeUTF8));
        memory_2->setText(QApplication::translate("MainDialog", "<memory>", 0, QApplication::UnicodeUTF8));
        model_2->setText(QApplication::translate("MainDialog", "<model>", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("MainDialog", "Model", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MainDialog", "Manufacturer", 0, QApplication::UnicodeUTF8));
        processor_2->setText(QApplication::translate("MainDialog", "<processor>", 0, QApplication::UnicodeUTF8));
        hwserialnr_2->setText(QApplication::translate("MainDialog", "<hwserialnr>", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("MainDialog", "Memory", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MainDialog", "Wipe Mode", 0, QApplication::UnicodeUTF8));
        wmodeMachine->setText(QApplication::translate("MainDialog", "Machine Level Wipe", 0, QApplication::UnicodeUTF8));
        wmodeDisk->setText(QApplication::translate("MainDialog", "Disk Level Wipe", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainDialog", "Number of Passes", 0, QApplication::UnicodeUTF8));
        nrPasses->clear();
        nrPasses->insertItems(0, QStringList()
         << QApplication::translate("MainDialog", "1-Pass DOD", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "1-Pass DOD with Certificate", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "3-Pass DOD", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "3-Pass DOD with Certificate", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "7-Pass DOD", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "7-Pass DOD with Certificate", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "11-Pass DOD", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "11-Pass DOD with Certificate", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "15-Pass DOD", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "15-Pass DOD with Certificate", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "17-Pass DOD", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "17-Pass DOD with Certificate", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "23-Pass DOD", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "23-Pass DOD with Certificate", 0, QApplication::UnicodeUTF8)
        );
        groupBox->setTitle(QApplication::translate("MainDialog", "Options", 0, QApplication::UnicodeUTF8));
        updateActions->setText(QApplication::translate("MainDialog", "Log User Activity", 0, QApplication::UnicodeUTF8));
        updateAuto->setText(QApplication::translate("MainDialog", "Update Database at end of Wipe", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainDialog", "Maximum Maximum Number of Simultaneous Wipes", 0, QApplication::UnicodeUTF8));
        maxWipers->clear();
        maxWipers->insertItems(0, QStringList()
         << QApplication::translate("MainDialog", "1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "4", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "16", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "24", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "32", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "50", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainDialog", "75", 0, QApplication::UnicodeUTF8)
        );
        label_13->setText(QApplication::translate("MainDialog", "Number of Disks Detected", 0, QApplication::UnicodeUTF8));
        rescanDisks->setText(QApplication::translate("MainDialog", "&Rescan", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainDialog", "Green Pulse User &ID", 0, QApplication::UnicodeUTF8));
        userName->setText(QString());
        machineInfo->setTitle(QApplication::translate("MainDialog", "System information", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainDialog", "Manufacturer", 0, QApplication::UnicodeUTF8));
        vendor->setText(QApplication::translate("MainDialog", "<vendor>", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainDialog", "Model", 0, QApplication::UnicodeUTF8));
        model->setText(QApplication::translate("MainDialog", "<model>", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainDialog", "Processor type", 0, QApplication::UnicodeUTF8));
        processor->setText(QApplication::translate("MainDialog", "<processor>", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainDialog", "Processor Count", 0, QApplication::UnicodeUTF8));
        procs->setText(QApplication::translate("MainDialog", "<procs>", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainDialog", "Core Count", 0, QApplication::UnicodeUTF8));
        cores->setText(QApplication::translate("MainDialog", "<cores>", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainDialog", "Threads per Proc.", 0, QApplication::UnicodeUTF8));
        threads->setText(QApplication::translate("MainDialog", "<threads>", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainDialog", "Memory", 0, QApplication::UnicodeUTF8));
        memory->setText(QApplication::translate("MainDialog", "<memory>", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainDialog", "MFG SN", 0, QApplication::UnicodeUTF8));
        hwserialnr->setText(QApplication::translate("MainDialog", "<hwserialnr>", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("MainDialog", "All Green &SN", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainDialog", "Disk Media", 0, QApplication::UnicodeUTF8));
        rtl_wipe->setText(QApplication::translate("MainDialog", "     &Wipe selected drives     ", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("MainDialog", "Annihilator SN", 0, QApplication::UnicodeUTF8));
        uiAnnSN->setText(QApplication::translate("MainDialog", "SN?", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("MainDialog", "Nr of wipes remaining", 0, QApplication::UnicodeUTF8));
        uiNrWipes->setText(QApplication::translate("MainDialog", "Wipes?", 0, QApplication::UnicodeUTF8));
        rtl_history->setText(QApplication::translate("MainDialog", "Wipe &history", 0, QApplication::UnicodeUTF8));
        rtl_credits->setText(QApplication::translate("MainDialog", "&Add wipe credits", 0, QApplication::UnicodeUTF8));
        rtl_reboot->setText(QApplication::translate("MainDialog", "&Reboot", 0, QApplication::UnicodeUTF8));
        doWipe->setText(QApplication::translate("MainDialog", "Do &wipe", 0, QApplication::UnicodeUTF8));
        updDB->setText(QApplication::translate("MainDialog", "&Update DB", 0, QApplication::UnicodeUTF8));
        saveReport->setText(QApplication::translate("MainDialog", "Save Re&port", 0, QApplication::UnicodeUTF8));
        viewError->setText(QApplication::translate("MainDialog", "View &Error log", 0, QApplication::UnicodeUTF8));
        quit->setText(QApplication::translate("MainDialog", "&Quit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainDialog: public Ui_MainDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINDIALOG_H
