#include <string>
#include <iostream>
#include <sstream>
#include <assert.h>
using namespace std;

#include <QtGui>
#include <QApplication>
#include <QDateTime>
#include "errordialog.h"
#include "liblshw/osutils.h"


ErrorDialog::ErrorDialog(QWidget *parent):QDialog(parent)
{
	setupUi(this);
	setModal(false);
	logWid->setReadOnly(true);

	displayLog();
}

void ErrorDialog::displayLog()
{
	extern vector<string> ErrorLog;
	QString logstr;

	for (unsigned int i = 0; i < ErrorLog.size(); i++) {
		logstr += QString::fromStdString(ErrorLog[i]);
	}
	if (!logstr.length()) {
		logstr = "No errors";
	}
	logWid->setPlainText(logstr);
}

void ErrorDialog::on_refresh_clicked()
{
	displayLog();
}

void ErrorDialog::on_copy_clicked()
{
	QClipboard *clipboard = QApplication::clipboard();

	clipboard->setText(logWid->toPlainText());
}
