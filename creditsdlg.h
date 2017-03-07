#ifndef __CREDITSDLG_H__
#define __CREDITSDLG_H__
#include <QDialog>
#include "ui_creditsdlg.h"
#include "maindialog.h"


class CreditsDlg :public QDialog, public Ui::CreditsDlg
{
	Q_OBJECT

public:
	CreditsDlg(QWidget *parent = 0);
	MainDialog *md;

private slots:
	void on_close_clicked();
	void on_add_clicked();
protected:
	void showEvent(QShowEvent *event);
	void updateView(void);
};

bool getNrWipesFromDb(int *pNrWipes, QString DBName, QWidget *parent);
bool addWipesToDb(QString LicCode, int NrWipes, QString DBName, QWidget *parent);
#endif
