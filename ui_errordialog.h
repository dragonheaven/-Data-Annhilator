/********************************************************************************
** Form generated from reading UI file 'errordialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ERRORDIALOG_H
#define UI_ERRORDIALOG_H

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
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ErrorDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *logWid;
    QHBoxLayout *horizontalLayout;
    QPushButton *refresh;
    QSpacerItem *horizontalSpacer;
    QPushButton *copy;
    QPushButton *close;

    void setupUi(QDialog *ErrorDialog)
    {
        if (ErrorDialog->objectName().isEmpty())
            ErrorDialog->setObjectName(QString::fromUtf8("ErrorDialog"));
        ErrorDialog->resize(714, 334);
        verticalLayout_2 = new QVBoxLayout(ErrorDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox = new QGroupBox(ErrorDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        logWid = new QPlainTextEdit(groupBox);
        logWid->setObjectName(QString::fromUtf8("logWid"));

        verticalLayout->addWidget(logWid);


        verticalLayout_2->addWidget(groupBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        refresh = new QPushButton(ErrorDialog);
        refresh->setObjectName(QString::fromUtf8("refresh"));

        horizontalLayout->addWidget(refresh);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        copy = new QPushButton(ErrorDialog);
        copy->setObjectName(QString::fromUtf8("copy"));

        horizontalLayout->addWidget(copy);

        close = new QPushButton(ErrorDialog);
        close->setObjectName(QString::fromUtf8("close"));
        close->setDefault(true);

        horizontalLayout->addWidget(close);


        verticalLayout_2->addLayout(horizontalLayout);


        retranslateUi(ErrorDialog);
        QObject::connect(close, SIGNAL(clicked()), ErrorDialog, SLOT(accept()));
        QObject::connect(ErrorDialog, SIGNAL(finished(int)), ErrorDialog, SLOT(deleteLater()));

        QMetaObject::connectSlotsByName(ErrorDialog);
    } // setupUi

    void retranslateUi(QDialog *ErrorDialog)
    {
        ErrorDialog->setWindowTitle(QApplication::translate("ErrorDialog", "Error log", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ErrorDialog", "Errors", 0, QApplication::UnicodeUTF8));
        refresh->setText(QApplication::translate("ErrorDialog", "&Refresh", 0, QApplication::UnicodeUTF8));
        copy->setText(QApplication::translate("ErrorDialog", "Copy to Cli&pboard", 0, QApplication::UnicodeUTF8));
        close->setText(QApplication::translate("ErrorDialog", "&Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ErrorDialog: public Ui_ErrorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ERRORDIALOG_H
