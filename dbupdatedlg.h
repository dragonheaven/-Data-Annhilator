#ifndef __DBUPDATEDLG_H__
#define __DBUPDATEDLG_H__
#include <vector>
#include <string>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QtSql>
#include "ui_dbupdatedlg.h"
#include "ui_dbautoupdatedlg.h"
#include "wipeit.h"
#include "maindialog.h"

class DBUpdateDlg :public QDialog, public Ui::DBUpdateDlg
{
	Q_OBJECT
public:
	DBUpdateDlg(QWidget *parent = 0);

public slots:
	void on_updateBtn_clicked();
	void on_backBtn_clicked();
	void on_closeBtn_clicked();

private:
	MainDialog *mdlg;
	QString localDBName;
	void updateOver();
};

class DBAutoUpdateDlg :public QDialog, public Ui::DBAutoUpdateDlg
{
	Q_OBJECT
public:
	DBAutoUpdateDlg(QWidget *parent = 0);

};

bool
DoUpdateDbDlg(QWidget *parent, MainDialog *mdlg, SysInfo *sys,
	      const std::vector<UserAction> &actions);
#endif
