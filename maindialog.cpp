#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <mntent.h>
#include <errno.h>
#include <unistd.h>
using namespace std;

#include <QtGui>
#include <QApplication>
#include <QDateTime>
#include <QMap>
#include "maindialog.h"
#include "scandialog.h"
#include "wipedialog.h"
#include "dbupdatedlg.h"
#include "errordialog.h"
#include "confirmdialog.h"
#include "histdialog.h"
#include "creditsdlg.h"
#include "liblshw/osutils.h"
#include "raiddrv.h"
#include "license.h"

SysInfo *CurrentSysInfo = NULL;
string bootdisk;
bool retail_mode;

bool
findBootDev(QString &devfile, QString &mntpt)
{
	FILE *fp;
	struct mntent *me;

	fp = setmntent("/etc/fstab", "r");
	if (!fp) {
		return false;
	}
	while ((me = getmntent(fp))) {
		QString sigfile, sigfile2;

		sigfile = QString(me->mnt_dir) + "/cde/optional/wipeit.tcz";
		sigfile2 = QString(me->mnt_dir) + "/etc/passwd";
		if (!access(qPrintable(sigfile), R_OK) ||
		    !access(qPrintable(sigfile2), R_OK)) {
			devfile = QString(me->mnt_fsname);
			mntpt = QString(me->mnt_dir);
			endmntent(fp);
			return true;
		}
	}

	endmntent(fp);
	return false;
}

void
MainDialog::makeRetailEdition()
{
	widget_1->setVisible(false);
	widget_2->setVisible(false);
	widget_3->setVisible(false);
	line->setVisible(false);
	prcheck->setVisible(false);
	widget_inhouse->setVisible(false);
	widget_retail->setVisible(true);
	logo->setMaximumSize(450,240);
	sysinfo_retail->setMaximumHeight(240);
}
	

MainDialog::MainDialog(QWidget *parent):QDialog(parent)
{
	setupUi(this);
	setModal(true);
	maxWipers->setCurrentIndex(5);

	connect(wmodeDisk, SIGNAL(toggled(bool)),
		this, SLOT(wmodeChanged(bool)));
	connect(wmodeMachine, SIGNAL(toggled(bool)),
		this, SLOT(wmodeChanged(bool)));
	wmodeMachine->setChecked(true);
	wmodeChanged(true);
	showBootDrive = false;

	/*
	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), serialnr,
			SLOT(setFocus()));
			*/

	prcheck = new PromptCheckBox;
	prcheck->setText("List Boot Disk");
	prcheck->setPrompt(
		"Are you sure you want to list the <b>BOOT DISK</b> "
		"for data annihilation ? It usually does not make sense "
		"to wipe the data annihilator boot disk clean! "
		"Confirm you want to list the data annihilator boot disk");
	prcheck->setChecked(false);
	miscHLayout->addWidget(prcheck);
	connect(prcheck, SIGNAL(toggled(bool)),
		this, SLOT(listBootDrive(bool)));

	/*
	 * Read in settings.
	 */
#ifdef TEST
	localPath = "./";
#else
	if (findBootDev(boot_devfile, boot_mntpt)) {
		fprintf(stderr, "Boot device = %s\n", qPrintable(boot_devfile));
		localPath = boot_mntpt + "/";
		QString inipath = localPath + "wipeit.ini";
		if (!access(qPrintable(inipath), R_OK)) {
			goto gotit;
		}
	}
	if (!access("./wipeit.ini", R_OK)) {
		localPath = "./";
		goto gotit;
	}
	QMessageBox::warning(parent, "Could not find boot disk",
		"Need the location of the boot disk to read INI file "
		"and local database. Database update feature will "
		"work in degraded mode.");
	localPath = "./";
#endif
gotit:
	localDBName = localPath + "WipedData.db";
	QString inipath = localPath + "wipeit.ini";
	QSettings wipeset(inipath, QSettings::IniFormat);

	wipeset.beginGroup("Database");
	db_hostname = wipeset.value("hostname", QString()).toString();
	db_portnr = wipeset.value("portnr", QString()).toString();
	db_dbname = wipeset.value("dbname", QString()).toString();
	db_username = wipeset.value("username", QString()).toString();
	db_password = wipeset.value("password", QString()).toString();
	wipeset.endGroup();

	/// XXX Need validator for username
#ifdef TEST
	userName->setText("Gautham L");
#endif

#ifdef RETAIL
	// retail_mode = true;
	// makeRetailEdition();
#ifndef TEST
	if (!getSerialNr(boot_devfile, &theSerialNr)) {
		QMessageBox::critical(parent, "Could not get serial Nr",
			"Could not get the serial number of data annihilator, aborting");
		exit(1);
	}
	if (theSerialNr == "INVALID#") {
		QMessageBox::critical(parent, "Serial nr not set",
			"Serial number has not been set on the data annihilator, aborting");
		exit(1);
	}
#endif
#else
	retail_mode = false;
#endif
}

void
MainDialog::listBootDrive(bool listboot)
{
	showBootDrive = listboot;
	updateInfo(CurrentSysInfo);
}

void
MainDialog::wmodeChanged(bool)
{
	if (wmodeDisk->isChecked()) {
		wmode_is_disk = true;
	} else {
		wmode_is_disk = false;
	}

	if (wmode_is_disk) {
		machineInfo->setVisible(false);
	} else {
		machineInfo->setVisible(true);
	}
}

void MainDialog::resizeEvent(QResizeEvent *event)
{
	QSize logoSize = logo->pixmap()->size();
	assert(logoSize.width() != 0);
	float aspectratio = (float)logoSize.height() / logoSize.width();
	assert(aspectratio != 0);

	/*
	logoSize.scale(event->size(), Qt::KeepAspectRatio);
	logo->setFixedSize(logoSize);
	*/
	(void)event;
}

void removeLayout(QLayout* layout)
{
	QLayoutItem *child;

	while((child = layout->takeAt(0)) != 0) {
		if(child->layout() != 0) {
			removeLayout(child->layout());
		} else if(child->widget() != 0) {
			delete child->widget();
		}

		delete child;
	}
}

/*
 * Manages the state row of a disk entry in the UI.
 * If one is not present, adds it.
 * If one is present, updates it.
 */
void
MainDialog::makeDiskState(DiskEntry *diskEntry)
{
	assert(diskEntry->disk);
	DiskInfo *disk = diskEntry->disk;

	QString wipestr;
	QTextStream txt(&wipestr);
	if (disk->wiped_state == DiskInfo::WIPED) {
		txt << "<b><big><font color=green>Data Annihilated</font></big></b>";
		txt << " on ";
		QDateTime dt = QDateTime::fromTime_t(disk->wiped_date);
		txt << dt.toString();
		if (!retail_mode) {
			txt << " by ";
			txt << disk->wiped_user.c_str();
		}
		txt << " with ";
		txt << disk->wiped_mode << (disk->wiped_mode > 1 ? " passes" : " pass");
	} else if (disk->wiped_state == DiskInfo::IOERROR) {
		txt <<"<b><big><i><font color=red>IO ERROR</font></i></big></b>";
		txt << " when wiping on ";
		QDateTime dt = QDateTime::fromTime_t(disk->wiped_date);
		txt << dt.toString();
		txt << " by ";
		txt << disk->wiped_user.c_str();
		txt << " with ";
		txt << disk->wiped_mode << " passes";
	} else if (disk->wiped_state == DiskInfo::BADDISK) {
		txt <<"<b><big><i><font color=red>Disk found defective"
			"</font></i></big></b>";
		txt << " when wiping on ";
		QDateTime dt = QDateTime::fromTime_t(disk->wiped_date);
		txt << dt.toString();
		txt << " by ";
		txt << disk->wiped_user.c_str();
		txt << " with ";
		txt << disk->wiped_mode << " passes";
	} else {
		wipestr = "<b><big><font color=red>Ready For Wipe"
			  "</font></b></big>";
	}
	mediaTableWidget->setCellWidget(diskEntry->row, STATE_COL, new QLabel(wipestr));

	if (!diskEntry->wipecheck) {
		QCheckBox *wipecheck;
		if (disk->bootdisk) {
			PromptCheckBox *prcheck = new PromptCheckBox;
			prcheck->setPrompt(
				"Are you sure you want to wipe the "
				"data annihilator boot disk ?",
				"Confirm you want to select the boot disk");
			wipecheck = prcheck;
		} else {
			wipecheck = new QCheckBox;
		}
		wipecheck->setChecked(disk->wipenow);
		mediaTableWidget->setCellWidget(diskEntry->row, WIPE_CHECK_COL, wipecheck);
		diskEntry->wipecheck = wipecheck;
	} else {
		diskEntry->wipecheck->setChecked(disk->wipenow);
	}
}

void
PromptCheckBox::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton &&
	    message.length() != 0 &&
	    checkState() == Qt::Unchecked) {
		int ret = QMessageBox::question(this, title, message,
				QMessageBox::Yes,
				QMessageBox::No|QMessageBox::Default);
		if (ret == QMessageBox::Yes) {
			setCheckState(Qt::Checked);
		}
	} else {
		QCheckBox::mousePressEvent(event);
	}
}

DiskEntry * MainDialog::makeDiskEntry(int disknr, DiskInfo *disk,
		int row)
{
	struct DiskEntry *diskEntry = new DiskEntry;
	diskEntry->disk = disk;
	diskEntry->disknr = disknr;
	diskEntry->row = row;

	string sizestr = decimalkilos(disk->dev_size);
	mediaTableWidget->setItem(row, SIZE_COL, new QTableWidgetItem(QString::fromStdString(sizestr)));

	mediaTableWidget->setItem(row, MFG_COL, new QTableWidgetItem(QString::fromStdString(disk->vendor)));

	mediaTableWidget->setItem(row, MODEL_COL, new QTableWidgetItem(QString::fromStdString(disk->model)));

	if (disk->enclid.length()) {
		mediaTableWidget->setItem(row, ENCL_ID_COL, new QTableWidgetItem(QString::fromStdString(disk->enclid)));
	} else {
		mediaTableWidget->setItem(row, ENCL_ID_COL, new QTableWidgetItem("None"));
	}

	mediaTableWidget->setItem(row, MFG_SN_COL, new QTableWidgetItem(QString::fromStdString(disk->hwserialnr)));

	if (!retail_mode) {
		QLineEdit *serialnr = new QLineEdit;
		serialnr->setText(QString::fromStdString(disk->serialnr));
		mediaTableWidget->setCellWidget(row, SERIAL_NR_COL, serialnr);
		diskEntry->serialnr = serialnr;
	}

	makeDiskState(diskEntry);

	return diskEntry;
}

/*
 * Update incore state from UI.
 * UI is represented by vector<DiskEntry>, incore state by SysInfo by ptrs
 * in DiskEntry.
 */
int
MainDialog::updateDiskState()
{
	int diskstowipe = 0;

	if (!retail_mode) {
		CurrentSysInfo->serialnr = serialnr->text().trimmed().toStdString();
		if (wmode_is_disk == false && CurrentSysInfo->serialnr.length() == 0) {
			QMessageBox::critical(this, "No machine serial number",
				"For a machine level wipe, you need to provide the "
				"<b>machine serial number</b>");
			return -1;
		}
	}

	/*
	 * Clear out wipenow field in all DiskInfo structures in SysInfo.
	 * Then set for those that are listed and selected in the UI.
	 */
	for (unsigned i = 0; i < CurrentSysInfo->disks.size(); i++) {
		CurrentSysInfo->disks[i].wipenow = false;
	}

	QString nosns;
	QMap<QString, bool> disk_sn;
	for (unsigned int i = 0; i < diskEntries.size(); i++) {
		QCheckBox *wipecheck = diskEntries[i].wipecheck;
		QLineEdit *serialnr = diskEntries[i].serialnr;
		DiskInfo *disk = diskEntries[i].disk;

		disk->wipenow = wipecheck->isChecked();
		if (disk->wipenow) {
			diskstowipe++;
		}
		if (!retail_mode) {
			disk->serialnr = serialnr->text().trimmed().toStdString();
			if (wmode_is_disk && disk->serialnr.length() == 0) {
				if (nosns.length()) {
					nosns += ", " + QString::number(i + 1);
				} else {
					nosns = QString::number(i + 1);
				}
			}
		} else {
			continue;
		}
		if (disk->serialnr.length() == 0) {
			continue;
		}

		QString serial = QString::fromStdString(disk->serialnr);
		if (disk_sn.contains(serial)) {
			QString msg;
			msg.sprintf("Serial nr [%s] has been duplicated. "
				    "Please provide unique serial numbers",
				    disk->serialnr.c_str());
			QMessageBox::critical(this, "Duplicate serial numbers",
					      msg);
			return -1;
		}
		disk_sn[serial] = true;
	}
	if (nosns.length()) {
		QMessageBox::critical(this, "No disk serial number",
			"For a disk level wipe, you need to provide "
			"serial number for all disks. Serial number for disk " +
			nosns + " is <b>missing</b>.");
		return -1;
	}
	return diskstowipe;
}

/*
 * Update UI from incore state.
 * UI is represented by vector<DiskEntry>, incore state by SysInfo
 */
void
MainDialog::updateInfo(SysInfo *sys)
{
	/*
	 * Display license related stuff: serial nr and nr of wipes remaining
	 */
	uiAnnSN->setText(theSerialNr);
	int nrWipes;
	if (!getNrWipesFromDb(&nrWipes, localDBName, this)) {
		QMessageBox::critical(this, "Could not get nr of wipes", "Error reading license database");
		exit(1);
	}
	uiNrWipes->setText(QString::number(nrWipes));

	vendor->setText(QString::fromStdString(sys->vendor));
	model->setText(QString::fromStdString(sys->model));
	processor->setText(QString::fromStdString(sys->processor));
	memory->setText(QString::fromStdString(kilobytes(sys->memory)));
	hwserialnr->setText(QString::fromStdString(sys->hwserialnr));
	procs->setText(QString::number(sys->processor_count));
	cores->setText(QString::number(sys->processor_cores));
	threads->setText(QString::number(sys->processor_threads));

	/*
	 * Set data on retail info fields too.
	 */
	vendor_2->setText(QString::fromStdString(sys->vendor));
	model_2->setText(QString::fromStdString(sys->model));
	processor_2->setText(QString::fromStdString(sys->processor));
	memory_2->setText(QString::fromStdString(kilobytes(sys->memory)));
	hwserialnr_2->setText(QString::fromStdString(sys->hwserialnr));

	/*
	 * Get a vertical layout, first put local disks in a grid, RAID
	 * disks follow in a hbox of a label and a grid. In local disks,
	 * put the boot disk at the last.
	 */
	int diskNr = 1;
	mediaTableWidget->clear();
	mediaTableWidget->setRowCount(0);
	mediaTableWidget->setColumnCount(9);
	mediaTableWidget->setHorizontalHeaderLabels(
		QStringList()
			<< "RAID Controller"
			<< "Size"
			<< "MFG"
			<< "Model"
			<< "Encl ID"
			<< "MFG SN"
			<< "All Green SN"
			<< "State"
			<< "Wipe Now?"
	);
	mediaTableWidget->horizontalHeader()->resizeSection(RAID_CONTROLLER_COL, 150);
	mediaTableWidget->horizontalHeader()->resizeSection(SIZE_COL, 75);
	mediaTableWidget->horizontalHeader()->resizeSection(MFG_COL, 200);
	mediaTableWidget->horizontalHeader()->resizeSection(MODEL_COL, 150);
	mediaTableWidget->horizontalHeader()->resizeSection(ENCL_ID_COL, 150);
	mediaTableWidget->horizontalHeader()->resizeSection(MFG_SN_COL, 150);
	mediaTableWidget->horizontalHeader()->resizeSection(SERIAL_NR_COL, 150);
	mediaTableWidget->horizontalHeader()->resizeSection(STATE_COL, 250);
	mediaTableWidget->horizontalHeader()->resizeSection(WIPE_CHECK_COL, 75);
	diskEntries.clear();
	DiskInfo *bootdisk = NULL;
	for (unsigned int i = 0; i < sys->disks.size(); i++) {
		QString devfile =
			QString::fromStdString(sys->disks[i].dev_file);
		if (boot_devfile.length() && boot_devfile.startsWith(devfile)) {
			sys->disks[i].bootdisk = true;
			sys->disks[i].wipenow = false;
			if (showBootDrive) {
				bootdisk = &sys->disks[i];
			}
			continue;
		}
		mediaTableWidget->insertRow(diskEntries.size());
		DiskEntry *onedisk = makeDiskEntry(diskNr++, &sys->disks[i],
						   diskEntries.size());
		diskEntries.push_back(*onedisk);
		delete onedisk;
	}
	if (bootdisk) {
		mediaTableWidget->insertRow(diskEntries.size());
		DiskEntry *onedisk = makeDiskEntry(diskNr++, bootdisk,
						   diskEntries.size());
		diskEntries.push_back(*onedisk);
		delete onedisk;
	}

	for (vector<RAIDCtlr *>::iterator ri = sys->RAIDCtlrs.begin();
	     ri != sys->RAIDCtlrs.end(); ri++) {
		RAIDCtlr *rc = *ri;
		QString ctlrName = QString::fromStdString(rc->getName());		

		int nrDrives = rc->getPhysDrvCnt();
		for (int i = 0; i < nrDrives; i++) {
			PhysDrv *pd = rc->getPhysDrv(i);
			DiskInfo *di = pd->getDiskInfo();

			mediaTableWidget->insertRow(diskEntries.size());
			mediaTableWidget->setItem(diskEntries.size(), RAID_CONTROLLER_COL, new QTableWidgetItem(ctlrName));
			DiskEntry *onedisk = makeDiskEntry(diskNr++, di, diskEntries.size());
			diskEntries.push_back(*onedisk);
			delete onedisk;
		}
	}
	diskNr--;

	if (diskNr > 0) {
		nrDisks->setText(QString::number(diskNr));
		undetected_disks = false;
	} else {
		int ret = QMessageBox::question(this, "No hard disks found",
			"Data annihilator could not find any hard disks on the system\n"
			"Are you certain that there are no hard disks on the system?",
				QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);
		if (ret == QMessageBox::Yes) {
			undetected_disks = false;
		} else {
			undetected_disks = true;
		}
	}
}

void
UpdateScan(string item, void *pData)
{
	ScanDialog *scanDialog = (ScanDialog *)pData;

	scanDialog->setCurrent(item);
	QApplication::processEvents();
}

bool
MainDialog::scanSystem()
{
	ScanDialog *scanDialog = new ScanDialog(this);
	SysInfo *newSysInfo;

	scanDialog->show();
	newSysInfo = get_system(UpdateScan, scanDialog);
	scanDialog->close();
	if (!newSysInfo) {
		return false;
	}

	if (CurrentSysInfo) {
		delete CurrentSysInfo;
	}
	CurrentSysInfo = newSysInfo;
	return true;
}

void MainDialog::on_rtl_wipe_clicked()
{
	this->on_doWipe_clicked();
	updateInfo(CurrentSysInfo);
}

void MainDialog::on_rtl_history_clicked()
{
	HistDialog hd(this);

	hd.exec();
}

void MainDialog::on_rtl_credits_clicked()
{
	CreditsDlg cd(this);
	
	cd.exec();
	updateInfo(CurrentSysInfo);
}

void MainDialog::on_rtl_reboot_clicked()
{
	system("reboot");
}

void MainDialog::on_doWipe_clicked()
{
	int diskstowipe = 0, nrpasses;

	/*
	 * Update DiskInfo structures from UI.
	 */
	diskstowipe = updateDiskState();
	if (diskstowipe < 0) {
		return;
	}
	if (!diskstowipe) {
		QMessageBox::warning(this, "No disks",
			"No disks have been selected for wiping!");
		return;
	}
	nrpasses =  nrPasses->currentText().split("-")[0].toInt();
	if (nrpasses <= 0 || nrpasses > 23) {
		QMessageBox::warning(this, "Invalid nr of passes",
			"Invalid number of passes. Number of passes must be"
			" non-zero and atmost 23.");
		return;
	}
		
	QString username = userName->text();
	username = username.simplified();
	if (username.isEmpty() && !retail_mode) {
		QMessageBox::warning(this, "User ID blank",
			"A user ID must be specified");
		return;
	}

	if (retail_mode) {
		// Check if user has enough number of wipe credits
		bool bret;
		int NrWipes;
		bret = getNrWipesFromDb(&NrWipes, localDBName, this);
		if (!bret) {
			return;
		}
		if (NrWipes > UNLIMIT_VALUE) {
			m_unlimited = true;
		} else {
			m_unlimited = false;
		}
		if (NrWipes < diskstowipe) {
			QString msg;
			msg += "Sorry you dont have enough wiping credits.\n";
			msg += "You have selected %1 disks, but have credits for wiping only %2 disks.\n";
			msg += "Please purchase more wiping credits or select lesser disks.\n";
			QMessageBox::warning(this, "Not enough credits", msg.arg(diskstowipe).arg(NrWipes));
			return;
		}

		// Confirm and get nr of wipe passes
		ConfirmDlg dlg(this);
		int ret = dlg.exec();
		if (!ret) {
			return;
		}
		nrpasses = dlg.nrPasses->value();
	}

	WipeDialog *wipeDialog = new WipeDialog(this);
	wipeDialog->nrPasses = nrpasses;
	wipeDialog->maxWipers = maxWipers->currentText().toInt();
	wipeDialog->userName = username.toStdString();

	wipeDialog->show();
	wipeDialog->doWipe();
	wipeDialog->close();
	
	delete wipeDialog;

	/*
	 * Update UI from DiskInfo structures.
	 */
	for (unsigned int i = 0; i < diskEntries.size(); i++) {
		makeDiskState(&diskEntries[i]);
	}
}

void MainDialog::on_quit_clicked()
{
	this->close();
}

void MainDialog::on_viewError_clicked()
{
	ErrorDialog *errDialog = new ErrorDialog(this);

	errDialog->show();
	/*
	 * errDialog is freed when finished SIGNAL is emitted. The signal
	 * is connected to deleteLater() at construction time.
	 */
}

void MainDialog::on_saveReport_clicked()
{
	if (updateDiskState() < 0) {
		return;
	}

	QFileDialog dialog(this, QString("Save report as"), localPath,
			   QString("Text files (*.txt)"));
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	int ret = dialog.exec();
	if (!ret) {
		return;
	}

	QString filename = dialog.selectedFiles().at(0);
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(this, "Save failure",
			"Could not open the named file for saving report",
			QMessageBox::Ok);
		return;
	}
	string report = makeReport(CurrentSysInfo, !wmode_is_disk);
	const char *report_str = report.c_str();
	file.write(report_str, strlen(report_str));
	file.close();
}

void MainDialog::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Escape) {
		/* Do Nothing */
	} else {
		QDialog::keyPressEvent(e);
	}
}

void
MainDialog::on_updDB_clicked()
{
	if (updateDiskState() < 0) {
		return;
	}

	DBUpdateDlg updlg(this);
	updlg.exec();
}

void
MainDialog::on_rescanDisks_clicked()
{
	if (scanSystem()) {
		assert(CurrentSysInfo);
		updateInfo(CurrentSysInfo);
	}
}
