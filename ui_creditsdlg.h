/********************************************************************************
** Form generated from reading UI file 'creditsdlg.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREDITSDLG_H
#define UI_CREDITSDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CreditsDlg
{
public:
    QLabel *label;
    QLabel *ui_NrWipes;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_3;
    QLineEdit *ui_UserName;
    QLabel *label_4;
    QLineEdit *ui_LicCode;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *add;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *close;

    void setupUi(QDialog *CreditsDlg)
    {
        if (CreditsDlg->objectName().isEmpty())
            CreditsDlg->setObjectName(QString::fromUtf8("CreditsDlg"));
        CreditsDlg->resize(549, 269);
        label = new QLabel(CreditsDlg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 20, 161, 41));
        ui_NrWipes = new QLabel(CreditsDlg);
        ui_NrWipes->setObjectName(QString::fromUtf8("ui_NrWipes"));
        ui_NrWipes->setGeometry(QRect(200, 30, 71, 21));
        ui_NrWipes->setFrameShape(QFrame::Panel);
        ui_NrWipes->setFrameShadow(QFrame::Sunken);
        groupBox = new QGroupBox(CreditsDlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(30, 80, 491, 133));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        ui_UserName = new QLineEdit(groupBox);
        ui_UserName->setObjectName(QString::fromUtf8("ui_UserName"));
        ui_UserName->setFocusPolicy(Qt::NoFocus);
        ui_UserName->setReadOnly(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, ui_UserName);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_4);

        ui_LicCode = new QLineEdit(groupBox);
        ui_LicCode->setObjectName(QString::fromUtf8("ui_LicCode"));

        formLayout->setWidget(1, QFormLayout::FieldRole, ui_LicCode);


        verticalLayout->addLayout(formLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        add = new QPushButton(groupBox);
        add->setObjectName(QString::fromUtf8("add"));

        horizontalLayout_3->addWidget(add);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_3);

        close = new QPushButton(CreditsDlg);
        close->setObjectName(QString::fromUtf8("close"));
        close->setGeometry(QRect(230, 230, 98, 27));

        retranslateUi(CreditsDlg);

        QMetaObject::connectSlotsByName(CreditsDlg);
    } // setupUi

    void retranslateUi(QDialog *CreditsDlg)
    {
        CreditsDlg->setWindowTitle(QApplication::translate("CreditsDlg", "Wiping credits", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("CreditsDlg", "Available wiping credits", 0, QApplication::UnicodeUTF8));
        ui_NrWipes->setText(QApplication::translate("CreditsDlg", "TextLabel", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("CreditsDlg", "Enter newly bought credits", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("CreditsDlg", "Serial Nr", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("CreditsDlg", "License code", 0, QApplication::UnicodeUTF8));
        add->setText(QApplication::translate("CreditsDlg", "&Add", 0, QApplication::UnicodeUTF8));
        close->setText(QApplication::translate("CreditsDlg", "&Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CreditsDlg: public Ui_CreditsDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREDITSDLG_H
