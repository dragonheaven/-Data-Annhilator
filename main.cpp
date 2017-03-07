#include <iostream>
#include <QObject>
#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include "maindialog.h"
#include "license.h"
#include "wipeit.h"
#include "megaraid.h"

#include <assert.h>
#include <unistd.h>
#include <cstring>

extern SysInfo *CurrentSysInfo;

using namespace std;
bool startmax = false;
MainDialog *the_main_dialog;
QString the_da_version = "1.12";

QString theSerialNr;

int
main(int ac, char *av[])
{
	if (ac > 1 && strcmp(av[1], "--maximized") == 0) {
		startmax = true;
	}

	QApplication app(ac, av);

	if (geteuid() != 0) {
		QMessageBox::critical(NULL, "Not running as root",
			"This program has to be run as root or with root "
			"priviledges. Try running 'sudo wipeit'. The "
			"program will now exit.", QMessageBox::Ok);
		return 0;
	}

	QProcess::execute("xset dpms 0 0 0");
	QProcess::execute("xset s off");
	QProcess::execute("for i in {0..256}; do setterm -blank 0 -powersave off >> /dev/tty$i; done");

	MainDialog *md = new MainDialog;
	the_main_dialog = md;
	if (!startmax) {
		md->show();
	} else {
		md->showMaximized();
	}
	if (md->scanSystem()) {
		assert(CurrentSysInfo);
		md->updateInfo(CurrentSysInfo);
	} else {
		QMessageBox::critical(md, "Could not scan the system",
			"The utility encountered a problem while scanning "
			"the system to get information about it. The "
			"program will now exit.", QMessageBox::Ok);
		md->done(QDialog::Rejected);
		return 0;
	}

	return app.exec();
}
