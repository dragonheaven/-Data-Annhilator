#include "confirmdialog.h"
#include "maindialog.h"
#include <QApplication>

#define MSG "Please note that multi-pass wipe take considerably that a single-pass wipe"

ConfirmDlg::ConfirmDlg(QWidget *mdlg) :QDialog(mdlg)
{
	setupUi(this);
	tooMuchTime->setText(MSG);
}

void
ConfirmDlg::on_yesButt_clicked()
{
	accept();
}

void
ConfirmDlg::on_noButt_clicked()
{
	reject();
}

void
ConfirmDlg::on_nrForbidden_value_Changed()
{
	reject();
}	

void
ConfirmDlg::on_nrPasses_valueChanged()
{
	if (nrPasses->value() > 1) {
		tooMuchTime->setText("<u>" MSG);
	} else {
		tooMuchTime->setText(MSG);
	}
}
