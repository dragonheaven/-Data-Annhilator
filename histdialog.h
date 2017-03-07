#ifndef __HISTDIALOG_H__
#define __HISTDIALOG_H__
#include <QDialog>
#include "ui_histdialog.h"


class HistDialog :public QDialog, public Ui::HistDialog
{
	Q_OBJECT

public:
	HistDialog(QWidget *parent = 0);
	QString	DBName;

private slots:
	void on_done_clicked();
	void on_ui_clear_clicked();

};
#endif
