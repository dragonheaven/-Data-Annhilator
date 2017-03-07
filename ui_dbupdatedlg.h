/********************************************************************************
** Form generated from reading UI file 'dbupdatedlg.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DBUPDATEDLG_H
#define UI_DBUPDATEDLG_H

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
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DBUpdateDlg
{
public:
    QVBoxLayout *verticalLayout;
    QStackedWidget *pager;
    QWidget *page;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *localBtn;
    QRadioButton *remoteBtn;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *updateBtn;
    QPushButton *cancelBtn;
    QSpacerItem *horizontalSpacer;
    QWidget *page_2;
    QVBoxLayout *verticalLayout_6;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_5;
    QPlainTextEdit *DBErrorLog;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *backBtn;
    QPushButton *closeBtn;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *DBUpdateDlg)
    {
        if (DBUpdateDlg->objectName().isEmpty())
            DBUpdateDlg->setObjectName(QString::fromUtf8("DBUpdateDlg"));
        DBUpdateDlg->resize(567, 294);
        DBUpdateDlg->setModal(true);
        verticalLayout = new QVBoxLayout(DBUpdateDlg);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pager = new QStackedWidget(DBUpdateDlg);
        pager->setObjectName(QString::fromUtf8("pager"));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        verticalLayout_4 = new QVBoxLayout(page);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        groupBox = new QGroupBox(page);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        localBtn = new QRadioButton(groupBox);
        localBtn->setObjectName(QString::fromUtf8("localBtn"));

        verticalLayout_2->addWidget(localBtn);

        remoteBtn = new QRadioButton(groupBox);
        remoteBtn->setObjectName(QString::fromUtf8("remoteBtn"));
        remoteBtn->setChecked(true);

        verticalLayout_2->addWidget(remoteBtn);


        verticalLayout_3->addLayout(verticalLayout_2);


        verticalLayout_4->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        updateBtn = new QPushButton(page);
        updateBtn->setObjectName(QString::fromUtf8("updateBtn"));

        horizontalLayout->addWidget(updateBtn);

        cancelBtn = new QPushButton(page);
        cancelBtn->setObjectName(QString::fromUtf8("cancelBtn"));

        horizontalLayout->addWidget(cancelBtn);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_4->addLayout(horizontalLayout);

        pager->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        verticalLayout_6 = new QVBoxLayout(page_2);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        groupBox_2 = new QGroupBox(page_2);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_5 = new QVBoxLayout(groupBox_2);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        DBErrorLog = new QPlainTextEdit(groupBox_2);
        DBErrorLog->setObjectName(QString::fromUtf8("DBErrorLog"));
        DBErrorLog->setReadOnly(true);

        verticalLayout_5->addWidget(DBErrorLog);


        verticalLayout_6->addWidget(groupBox_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        backBtn = new QPushButton(page_2);
        backBtn->setObjectName(QString::fromUtf8("backBtn"));
        backBtn->setEnabled(false);

        horizontalLayout_2->addWidget(backBtn);

        closeBtn = new QPushButton(page_2);
        closeBtn->setObjectName(QString::fromUtf8("closeBtn"));
        closeBtn->setEnabled(false);

        horizontalLayout_2->addWidget(closeBtn);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_6->addLayout(horizontalLayout_2);

        pager->addWidget(page_2);

        verticalLayout->addWidget(pager);


        retranslateUi(DBUpdateDlg);
        QObject::connect(cancelBtn, SIGNAL(clicked()), DBUpdateDlg, SLOT(reject()));

        pager->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(DBUpdateDlg);
    } // setupUi

    void retranslateUi(QDialog *DBUpdateDlg)
    {
        DBUpdateDlg->setWindowTitle(QApplication::translate("DBUpdateDlg", "Update Database...", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("DBUpdateDlg", "Update database at", 0, QApplication::UnicodeUTF8));
        localBtn->setText(QApplication::translate("DBUpdateDlg", "<Local database>", 0, QApplication::UnicodeUTF8));
        remoteBtn->setText(QApplication::translate("DBUpdateDlg", "<Remote database>", 0, QApplication::UnicodeUTF8));
        updateBtn->setText(QApplication::translate("DBUpdateDlg", "&Update", 0, QApplication::UnicodeUTF8));
        cancelBtn->setText(QApplication::translate("DBUpdateDlg", "&Cancel", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("DBUpdateDlg", "Updation status ...", 0, QApplication::UnicodeUTF8));
        backBtn->setText(QApplication::translate("DBUpdateDlg", "&Back", 0, QApplication::UnicodeUTF8));
        closeBtn->setText(QApplication::translate("DBUpdateDlg", "&Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DBUpdateDlg: public Ui_DBUpdateDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DBUPDATEDLG_H
