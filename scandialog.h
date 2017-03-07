#ifndef __SCANDIALOG_H__
#define __SCANDIALOG_H__
#include <QDialog>
#include "ui_scandialog.h"

class ScanDialog: public QDialog, public Ui::ScanDialog
{
public:
	ScanDialog(QWidget *parent = 0):QDialog(parent)
	{
		setupUi(this);
		setModal(true);
	}
	void setCurrent(string str)
	{
		currentItem->setText(QString::fromStdString(str));
	}
};
#endif
