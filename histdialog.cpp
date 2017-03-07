#include "histdialog.h"
#include "maindialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

HistDialog::HistDialog(QWidget *mdlg):QDialog(mdlg)
{
	MainDialog *md = static_cast<MainDialog *>(mdlg);
	int nrrows = 0;

	setupUi(this);
	DBName = md->localDBName;
	QSqlDatabase db;
	db = QSqlDatabase::addDatabase("QSQLITE", "WIPEDB");
	db.setDatabaseName(DBName);
	if (!db.open()) {
		hist->appendPlainText("Could not open history database");
		return;
	}
	
	QSqlQuery qy(db);
	qy.prepare("select WipeInfo from wipehistory order by Id");
	qy.exec();
	if (!qy.isActive()) {
		hist->appendPlainText("Could not read history database");
		db.close();
		return;
	}
	while (qy.next()) {
		nrrows++;
		hist->appendPlainText(qy.value(0).toString());
	}
	
	db.close();
	return;
}

void
HistDialog::on_done_clicked()
{
	this->close();
}

void

