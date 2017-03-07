#ifndef __CONFIRMDIALOG_H__
#define __CONFIRMDIALOG_H__
#include <QDialog>
#include "ui_confirmdialog.h"


class ConfirmDlg :public QDialog, public Ui::ConfirmDlg
{
	Q_OBJECT

public:
	ConfirmDlg(QWidget *parent = 0);

private slots:
	void on_yesButt_clicked();
	void on_noButt_clicked();
	void on_nrPasses_valueChanged();
};
#endif
