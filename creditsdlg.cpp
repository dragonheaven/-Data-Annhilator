#define _LARGEFILE64_SOURCE
#include <QtGui>
#include <QApplication>
#include <QDateTime>
#include <QtSql>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "creditsdlg.h"
#include "maindialog.h"
#include "license.h"
#include "simplecrypt.h"

	
bool
getNrWipesFromDb(int *pNrWipes, QString DBName, QWidget *parent)
{
	QSqlDatabase db;
	db = QSqlDatabase::addDatabase("QSQLITE", "CRDB");
	db.setDatabaseName(DBName);
	if (!db.open()) {
		QMessageBox::critical(parent, "Database error", "Could not open credits database");
		QSqlDatabase::removeDatabase("CRDB");
		return false;
	}
	
	QSqlQuery qy(db);
	qy.prepare("select sum(WipeDelta) from credits");
	qy.exec();
	if (!qy.isActive()) {
		QMessageBox::critical(parent, "Database error", "Could not open credits database");
		db.close();
		return false;
	}
	if (!qy.next()) {
		QMessageBox::critical(parent, "Database error", "Could not get net credits");
		db.close();
		return false;
	}
	*pNrWipes = qy.value(0).toInt();
	db.close();
	return true;
}

bool
addWipesToDb(QString LicCode, int NrWipes, QString DBName, QWidget *parent)
{
	QSqlDatabase db;
	db = QSqlDatabase::addDatabase("QSQLITE", "WIPEDB");
	db.setDatabaseName(DBName);
	if (!db.open()) {
		QMessageBox::critical(parent, "Database error", "Could not open credits database");
		return false;
	}
	QSqlQuery qy(db);

	// If License code specified, check that it does not already exist
	if (!LicCode.isEmpty()) {
		qy.prepare("select LicCode from credits where LicCode = :liccode");
		qy.bindValue(":liccode", LicCode);
		qy.exec();
		if (!qy.isActive()) {
			QMessageBox::critical(parent, "Database error", "Could not search credits database");
			db.close();
			return false;
		}
		int NrRows = 0;
		while (qy.next()) {
			NrRows++;
		}
		if (NrRows) {
			QMessageBox::critical(parent, "License error", "License already exists");
			db.close();
			return false;
		}
	}
		
	qy.prepare("insert into credits(DTS, LicCode, WipeDelta) values(:dts, :lic, :delta)");
	qy.bindValue(":dts", QDateTime::currentDateTime());
	qy.bindValue(":lic", LicCode);
	qy.bindValue(":delta", NrWipes);
	qy.exec();
	if (!qy.isActive()) {
		fprintf(stderr,"SQL error:%s\n", qPrintable(qy.lastError().text()));
		db.close();
		return false;
	}

	db.close();
	return true;
}

CreditsDlg::CreditsDlg(QWidget *parent) :QDialog(parent)
{
	md = static_cast<MainDialog *>(parent);

	setupUi(this);
	ui_UserName->setText(theSerialNr);
}

void
CreditsDlg::showEvent(QShowEvent *event)
{
	updateView();
	QDialog::showEvent(event);
}

void
CreditsDlg::updateView(void)
{
	int NrWipes, ret;

	ret = getNrWipesFromDb(&NrWipes, md->localDBName, md);
	if (!ret) {
		reject();
	}
	if (NrWipes > UNLIMIT_VALUE) {
		ui_NrWipes->setText("UNLIMITED");
	} else {
		ui_NrWipes->setText(QString::number(NrWipes));
	}
}

void
CreditsDlg::on_close_clicked()
{
	accept();
}

void
CreditsDlg::on_add_clicked()
{
	if (ui_UserName->text().isEmpty() || ui_LicCode->text().isEmpty()) {
		QMessageBox::warning(this, "Invalid entry",
				     "Please provide a license code");
		return;
	}

	QString LicCode, UserName;
	LicCode = ui_LicCode->text();
	UserName = ui_UserName->text();
	int nrWipes;
	if (!decodeLicense(LicCode, UserName, &nrWipes)) {
		QMessageBox::warning(this, "Invalid license",
				     "Please provide a valid license code");
		return;
	}

	if (!addWipesToDb(LicCode, nrWipes, md->localDBName, this)) {
		return;
	}
	ui_UserName->clear();
	ui_LicCode->clear();
	updateView();
	QMessageBox::information(this, "Successfully added", tr("You have successfully added %1 wipes. "
					"Thank you for purchasing.").arg(nrWipes));
}
