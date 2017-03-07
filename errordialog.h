#ifndef __ERRORDIALOG_H__
#define __ERRORDIALOG_H__
#include <vector>
#include <string>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include "ui_errordialog.h"
#include "wipeit.h"

class ErrorDialog :public QDialog, public Ui::ErrorDialog
{
	Q_OBJECT
public:
	ErrorDialog(QWidget *parent = 0);

private:
	void displayLog();

public slots:
	void on_refresh_clicked();
	void on_copy_clicked();
};
#endif
