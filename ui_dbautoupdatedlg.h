/********************************************************************************
** Form generated from reading UI file 'dbautoupdatedlg.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DBAUTOUPDATEDLG_H
#define UI_DBAUTOUPDATEDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DBAutoUpdateDlg
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QProgressBar *progressBar;
    QPlainTextEdit *DBErrorLog;
    QHBoxLayout *horizontalLayout;
    QPushButton *OkBtn;
    QSpacerItem *horizontalSpacer;

    void setupUi(QDialog *DBAutoUpdateDlg)
    {
        if (DBAutoUpdateDlg->objectName().isEmpty())
            DBAutoUpdateDlg->setObjectName(QString::fromUtf8("DBAutoUpdateDlg"));
        DBAutoUpdateDlg->resize(617, 440);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DBAutoUpdateDlg->sizePolicy().hasHeightForWidth());
        DBAutoUpdateDlg->setSizePolicy(sizePolicy);
        DBAutoUpdateDlg->setMinimumSize(QSize(617, 313));
        verticalLayout = new QVBoxLayout(DBAutoUpdateDlg);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(DBAutoUpdateDlg);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        progressBar = new QProgressBar(DBAutoUpdateDlg);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);

        verticalLayout->addWidget(progressBar);

        DBErrorLog = new QPlainTextEdit(DBAutoUpdateDlg);
        DBErrorLog->setObjectName(QString::fromUtf8("DBErrorLog"));
        DBErrorLog->setReadOnly(true);

        verticalLayout->addWidget(DBErrorLog);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        OkBtn = new QPushButton(DBAutoUpdateDlg);
        OkBtn->setObjectName(QString::fromUtf8("OkBtn"));
        OkBtn->setEnabled(false);

        horizontalLayout->addWidget(OkBtn);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(DBAutoUpdateDlg);
        QObject::connect(OkBtn, SIGNAL(clicked()), DBAutoUpdateDlg, SLOT(accept()));

        QMetaObject::connectSlotsByName(DBAutoUpdateDlg);
    } // setupUi

    void retranslateUi(QDialog *DBAutoUpdateDlg)
    {
        DBAutoUpdateDlg->setWindowTitle(QApplication::translate("DBAutoUpdateDlg", "Wiping complete", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DBAutoUpdateDlg", "Updating wipe history", 0, QApplication::UnicodeUTF8));
        OkBtn->setText(QApplication::translate("DBAutoUpdateDlg", "&Done", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DBAutoUpdateDlg: public Ui_DBAutoUpdateDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DBAUTOUPDATEDLG_H
