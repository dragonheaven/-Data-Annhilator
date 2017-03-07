#include <string>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <sys/time.h>
using namespace std;

#include <QtGui>
#include <QApplication>
#include <QDateTime>
#include "wipedialog.h"
#include "dbupdatedlg.h"
#include "liblshw/osutils.h"
#include "raiddrv.h"
#include "creditsdlg.h"

extern SysInfo *CurrentSysInfo;

#define MIN(a, b) ((a) < (b) ? (a) : (b))

WipeDialog::WipeDialog(QWidget *parent):QDialog(parent)
{
	mdlg = static_cast<MainDialog *>(parent);
	setupUi(this);
	setModal(true);
	canceled = false;

	pthread_mutex_init(&totalsLock, NULL);
	sem_init(&threadExit, 0, 0);
}

QLabel *
addTextField(const QString &label, const QString &data, QBoxLayout *lay)
{
	lay->addWidget(new QLabel(label));
	QLabel *dl = new QLabel(data);
	dl->setFrameStyle(QFrame::Panel|QFrame::Sunken);
	lay->addWidget(dl);
	return dl;
}

void
getnowplusdelta(struct timespec *ts, long microsecs)
{
	clock_gettime(CLOCK_REALTIME, ts);
	ts->tv_nsec += microsecs * 1000L;
}

void
thr_setLabel(QLabel *label, const QString &str)
{
	QMetaObject::invokeMethod(label, "setText",
				  Q_ARG(QString, str));
}

void
thr_setProgress(QProgressBar *bar, int completion)
{
	QMetaObject::invokeMethod(bar, "setValue",
				  Q_ARG(int, completion));
}

DiskWipeUi *
makeDiskWipeUi(DiskInfo *disk, WipeDialog *wd)
{
	QHBoxLayout *hbox;
	DiskWipeUi *dwui;

	dwui = new DiskWipeUi;
	dwui->disk = disk;
	dwui->wipeDialog = wd;
	dwui->ui = new QVBoxLayout;
	dwui->IODone = 0;
	dwui->lastThroughput = 0;
	hbox = new QHBoxLayout;

	QString diskSizeStr = QString(decimalkilos(disk->dev_size).c_str());
	dwui->diskSize = addTextField("Disk size", diskSizeStr, hbox);
	dwui->passNr = addTextField("Pass number", "<M/N>", hbox);
	dwui->sectorsWiped = addTextField("Sectors wiped", "<M/N>", hbox);
	dwui->ui->addLayout(hbox);

	hbox = new QHBoxLayout;
	hbox->addWidget(new QLabel("Progress"));
	dwui->progressBar = new QProgressBar;
	hbox->addWidget(dwui->progressBar);
	dwui->ui->addLayout(hbox);

	hbox = new QHBoxLayout;
	dwui->throughput = addTextField("Throughput", "<mbps>", hbox);
	dwui->completionTime = addTextField("Completion time", "<time>", hbox);
	dwui->ui->addLayout(hbox);

	return dwui;
}

bool updateProgress(void *privData, DiskInfo *disk, float throughput,
       			int passnr, u64_t written, u64_t sectorsdone)
{
	int percentover;
	float timeremain;
	QString str, str2;
	u64_t diskiosize;
	DiskWipeUi *dwui = (DiskWipeUi *)privData;
	WipeDialog *wd = dwui->wipeDialog;

	assert(dwui->disk == disk);
	str.sprintf("%d/%d", passnr + 1, wd->nrPasses);
	thr_setLabel(dwui->passNr, str);
	str = QString::fromStdString(kilobytes(throughput));
	str += "/s";
	thr_setLabel(dwui->throughput, str);

	str.sprintf("%llu/%llu", sectorsdone,
		    disk->dev_size / disk->sector_size);
	thr_setLabel(dwui->sectorsWiped, str);

	diskiosize = disk->dev_size * wd->nrPasses;
	dwui->IODone += written;
	percentover = dwui->IODone * 100 / diskiosize;
	thr_setProgress(dwui->progressBar, percentover);

	timeremain = (diskiosize - dwui->IODone) / throughput;
	str =  QString(timeunits(timeremain));
	thr_setLabel(dwui->completionTime, str);

	pthread_mutex_lock(&wd->totalsLock);
	wd->totalIODone += written;
	percentover = wd->totalIODone * 100 / wd->totalIOSize;
	if ((percentover == 100) || (percentover - wd->totalPercent >= 2)) {
		thr_setProgress(wd->totalProgress, percentover);
		wd->totalPercent = percentover;
	}
	wd->totalThroughput -= dwui->lastThroughput;
	dwui->lastThroughput = (u64_t)throughput;
	wd->totalThroughput += dwui->lastThroughput;
	str = QString::fromStdString(kilobytes(wd->totalThroughput));
	str += "/s";

	timeremain = (float)(wd->totalIOSize - wd->totalIODone)
		     / wd->totalThroughput;
	str2 = QString(timeunits(timeremain));
	pthread_mutex_unlock(&wd->totalsLock);
	
	thr_setLabel(wd->totalThroughputLbl, str);
	thr_setLabel(wd->totalTime, str2);

	if (wd->canceled) {
		return false;
	}
	return true;
}

/*
bool updateProgress(void *privData, DiskInfo *disk, float throughput,
       			int passnr, u64_t written, u64_t sectorsdone)
{
	int percentover;
	float timeremain;
	QString str;
	u64_t diskiosize;
	WipeDialog *wipeDialog = (WipeDialog *)privData;

	str.sprintf("%d/%d", passnr + 1, wipeDialog->nrPasses);
	wipeDialog->diskPassnr->setText(str);
	str = QString::fromStdString(kilobytes(throughput));
	str += "/s";
	wipeDialog->diskThroughput->setText(str);

	str.sprintf("%llu/%llu", sectorsdone,
		    disk->dev_size / disk->sector_size);
	wipeDialog->sectorsWiped->setText(str);

	diskiosize = disk->dev_size * wipeDialog->nrPasses;
	percentover = written * 100 / diskiosize;
	wipeDialog->diskProgress->setValue(percentover);

	timeremain = (diskiosize - written) / throughput;
	str =  QString(timeunits(timeremain));
	wipeDialog->diskTime->setText(str);

	percentover = (wipeDialog->totalIODone + written) * 100
			/ wipeDialog->totalIOSize;
	wipeDialog->totalProgress->setValue(percentover);

	timeremain = (wipeDialog->totalIOSize - wipeDialog->totalIODone -
			written) / throughput;
	str = QString(timeunits(timeremain));
	wipeDialog->totalTime->setText(str);

	QApplication::processEvents();

	if (wipeDialog->canceled) {
		return false;
	}
	return true;
}
*/

void *
wiper_thread(void *arg)
{
	DiskWipeUi *dwui = (DiskWipeUi *)arg;
	WipeDialog *wd = dwui->wipeDialog;
	DiskInfo *disk = dwui->disk;
	QString str;

	/*
	 * Wipe this disk
	 */
	thr_setLabel(dwui->passNr, QString::number(1));
	thr_setProgress(dwui->progressBar, 0);
	thr_setLabel(dwui->throughput, "N/A");
	thr_setLabel(dwui->completionTime, "N/A");

	/*
	 * Log wipe start
	 */
	struct UserAction act;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	act.dts = tv.tv_sec;
	act.user = wd->userName;
	act.event = "WIPESTART";
	act.disk = disk;
	pthread_mutex_lock(&wd->totalsLock);
	wd->userActions.push_back(act);
	pthread_mutex_unlock(&wd->totalsLock);

	QApplication::processEvents();
	bool bret = wipeit(disk, wd->nrPasses, wd->userName, updateProgress,
			   dwui);

	/*
	 * Log wipe end or cancel.
	 */
	gettimeofday(&tv, NULL);
	act.dts = tv.tv_sec;
	act.user = wd->userName;
	if (!bret) {
		act.event = "WIPECANCEL";
	} else {
		act.event = "WIPEEND";
		if (disk->wiped_state == DiskInfo::IOERROR) {
			act.note = "IOERRORS seen";
		} else if (disk->wiped_state == DiskInfo::BADDISK) {
			act.note = "Disk defective, wiping terminated";
		} else {
			act.note = "Data annihilated";
		}
	}
	act.disk = disk;
	pthread_mutex_lock(&wd->totalsLock);
	wd->userActions.push_back(act);
	wd->totalThroughput -= dwui->lastThroughput;
	dwui->lastThroughput = 0;
	str = QString::fromStdString(kilobytes(wd->totalThroughput));
	pthread_mutex_unlock(&wd->totalsLock);
	str += "/s";
	thr_setLabel(wd->totalThroughputLbl, str);

	if (bret && disk->wiped_state == DiskInfo::IOERROR) {
		pthread_mutex_lock(&wd->totalsLock);
		wd->nrErrors++;
		QString str;
		str.sprintf("<b><font color=red>%d</font></b>", wd->nrErrors);
		thr_setLabel(wd->wipeErrors, str);
		pthread_mutex_unlock(&wd->totalsLock);
	}
	disk->wipenow = false;
	QString complMsg;
	if (disk->wiped_state == DiskInfo::IOERROR) {
		complMsg = "<b><font color=red>IOERROR</font></b>";
	} else {
		complMsg = "Complete";
	}
	thr_setLabel(dwui->completionTime, complMsg);
	thr_setLabel(dwui->throughput, "N/A");

	sem_post(&wd->threadExit);
	return NULL;
}


void WipeDialog::updateErrors(void)
{
	if (nrErrors) {
		QString str;
		str.sprintf("<b><font color=red>%d</font></b>", nrErrors);
		wipeErrors->setText(str);
	} else {
		wipeErrors->setText(QString::number(nrErrors));
	}
}

bool WipeDialog::doWipe()
{
	int diskcount;
	unsigned nrctrls;

	assert(CurrentSysInfo);

	diskcount = 0;
	totalIOSize = 0;
	nrErrors = 0;
	updateErrors();
	QVBoxLayout *vlay = new QVBoxLayout;
	for (unsigned int i = 0; i < CurrentSysInfo->disks.size(); i++) {
		DiskInfo *disk = &CurrentSysInfo->disks[i];
		if (disk->wipenow) {
			diskcount++;
			totalIOSize += disk->dev_size * nrPasses;

			DiskWipeUi *diskui = makeDiskWipeUi(disk, this);
			if (!diskUis.empty()) {
				QFrame *line = new QFrame;
				line->setFrameShape(QFrame::HLine);
				line->setFrameShadow(QFrame::Sunken);
				vlay->addWidget(line);
			}
			vlay->addLayout(diskui->ui);
			diskUis.push_back(diskui);
		}
	}

	nrctrls = CurrentSysInfo->RAIDCtlrs.size();
	for (unsigned i = 0; i < nrctrls; i++) {
		RAIDCtlr *rc = CurrentSysInfo->RAIDCtlrs[i];
		int nrdrvs;

		/*
		 * Destroy virtual drives.
		 */

		// XXX Need a message to say that we are destroying VDs.
		nrdrvs = rc->getVirtDrvCnt();
		for (int j = 0; j < nrdrvs; j++) {
			VirtDrv *vd = rc->getVirtDrv(j);
			if (!vd->toDestroy) {
				continue;
			}
			if (!rc->destroyVirt(vd->getID())) {
				nrErrors++;
				updateErrors();
				// XXX Any further handling ?
			}
		}

		/*
		 * Prepare all physical drives to get a host accessible
		 * device file.
		 */
		nrdrvs = rc->getPhysDrvCnt();
		for (int j = 0; j < nrdrvs; j++) {
			PhysDrv *pd = rc->getPhysDrv(j);
			DiskInfo *di = pd->getDiskInfo();
			if (!di->wipenow) {
				continue;
			}
			diskcount++;
			totalIOSize += di->dev_size * nrPasses;
			if (!rc->preparePhys(j)) {
				nrErrors++;
				updateErrors();
				continue;
				// XXX if preparation fails should we still
				// be doing a wipeit on it down below ?
			}
		}

	}
	vlay->addStretch();
	QWidget *contents = new QWidget;
	contents->setLayout(vlay);
	diskList->setWidget(contents);

	totalIODone = 0;
	totalThroughput = 0;
	totalPercent = 0;
	totalProgress->setValue(0);
	totalThroughputLbl->setText("N/A");
	totalTime->setText("N/A");
	int diskidx = 0, diskswiped = 0;

	/*
	 * First wipe locally attached disks.
	 */
	int nrwipers = 0, newwipers, avl;
	gettimeofday(&this->wipeStartTime, NULL);
	while (diskswiped < diskcount) {
		QString diskwipe_str;
		diskwipe_str.sprintf("%d/%d", diskswiped, diskcount);
		disksWiped->setText(diskwipe_str);

		avl = maxWipers - nrwipers;
		newwipers = MIN(avl, diskcount - diskswiped - nrwipers);
		while (newwipers) {
			DiskWipeUi *dwui = diskUis[diskidx++];
			pthread_t tinfo;
			if (pthread_create(&tinfo, NULL, wiper_thread, dwui)) {
				// bump threadExit
			}
			newwipers--;
			nrwipers++;
		}

		if (nrwipers) {
			assert(diskswiped < diskcount);
			while (true) {
				struct timespec timeout;
				getnowplusdelta(&timeout, 500);

				if (!sem_timedwait(&threadExit, &timeout)) {
					break;
				}
				QApplication::processEvents();
			}
			nrwipers--;
			diskswiped++;
		}

		if (canceled) {
			while (nrwipers) {
				struct timespec timeout;
				getnowplusdelta(&timeout, 1000);

				if (!sem_timedwait(&threadExit, &timeout)) {
					nrwipers--;
				}
				QApplication::processEvents();
			}
			break;
		}
	}


#if 0
	for (unsigned int i = 0; i < CurrentSysInfo->disks.size(); i++) {
		DiskInfo *disk = &CurrentSysInfo->disks[i];
		if (!disk->wipenow) {
			continue;
		}
		/*
		 * Wipe this disk
		 */
		QString disknr_str;
		disknr_str.sprintf("%d/%d", disknr, diskcount);
		diskNr->setText(disknr_str);
		diskSize->setText(QString(decimalkilos(disk->dev_size).c_str()));
		diskPassnr->setText(QString::number(1));
		diskProgress->setValue(0);
		diskThroughput->setText("N/A");
		diskTime->setText("N/A");

		QApplication::processEvents();
		bret = wipeit(disk, nrPasses, userName, updateProgress, this);
		if (!bret && disk->errorno) {
			nrErrors++;
			QString str;
			str.sprintf("<b><font color=red>%d</font></b>",
				    nrErrors);
			wipeErrors->setText(str);
		}
		
		disknr++;
		totalIODone += disk->dev_size * nrPasses;

		if (canceled) {
			goto out;
		}
		disk->wipenow = false;
	}

	/*
	 * Then wipe RAID controller attached disks.
	 */

	nrctrls = CurrentSysInfo->RAIDCtlrs.size();
	for (unsigned i = 0; i < nrctrls; i++) {
		RAIDCtlr *rc = CurrentSysInfo->RAIDCtlrs[i];

		int nrdrvs = rc->getPhysDrvCnt();
		for (int j = 0; j < nrdrvs; j++) {
			PhysDrv *pd = rc->getPhysDrv(j);
			DiskInfo *di = pd->getDiskInfo();
			if (!di->wipenow) {
				continue;
			}

			/*
			 * Wipe this disk
			 */
			QString disknr_str;
			disknr_str.sprintf("%d/%d", disknr, diskcount);
			diskNr->setText(disknr_str);
			QString disksz_str = QString::fromStdString(
						decimalkilos(di->dev_size));
			diskSize->setText(disksz_str);
			diskPassnr->setText(QString::number(1));
			diskProgress->setValue(0);
			diskThroughput->setText("N/A");
			diskTime->setText("N/A");

			QApplication::processEvents();
			bret = wipeit(di, nrPasses, userName, updateProgress,
				      this);
			if (!bret && di->errorno) {
				nrErrors++;
				updateErrors();
			}
			
			disknr++;
			totalIODone += di->dev_size * nrPasses;

			if (canceled) {
				goto out;
			}
			di->wipenow = false;

		}
	}
#endif
	// Update credits for disks successfully wiped
	if (retail_mode && !mdlg->m_unlimited) {
		vector<UserAction> &actions = userActions;
		int nrDisksWiped = 0;
		for (unsigned i = 0; i < actions.size(); i++) {
			UserAction *act = &actions[i];
			if (act->event != "WIPEEND") {
				continue;
			}
			if (act->disk->wiped_state != DiskInfo::WIPED) {
				continue;
			}
			nrDisksWiped++;
		}
		bool bret = addWipesToDb(QString(), -nrDisksWiped, mdlg->localDBName, this);
		(void)bret;
	}

	if (mdlg->updateAuto->isChecked()) {
		DoUpdateDbDlg(this, mdlg, CurrentSysInfo, userActions);
	}

	return !canceled;
}

#if 0
bool WipeDialog::doWipe()
{
	int diskcount, disknr;
	unsigned nrctrls;
	bool bret;
	vector<UserAction> actions;

	assert(CurrentSysInfo);

	diskcount = 0;
	totalIOSize = 0;
	nrErrors = 0;
	updateErrors();
	for (unsigned int i = 0; i < CurrentSysInfo->disks.size(); i++) {
		DiskInfo *disk = &CurrentSysInfo->disks[i];
		if (disk->wipenow) {
			diskcount++;
			totalIOSize += disk->dev_size * nrPasses;
		}
	}

	nrctrls = CurrentSysInfo->RAIDCtlrs.size();
	for (unsigned i = 0; i < nrctrls; i++) {
		RAIDCtlr *rc = CurrentSysInfo->RAIDCtlrs[i];
		int nrdrvs;

		/*
		 * Destroy virtual drives.
		 */

		// XXX Need a message to say that we are destroying VDs.
		nrdrvs = rc->getVirtDrvCnt();
		for (int j = 0; j < nrdrvs; j++) {
			VirtDrv *vd = rc->getVirtDrv(j);
			if (!vd->toDestroy) {
				continue;
			}
			if (!rc->destroyVirt(vd->getID())) {
				nrErrors++;
				updateErrors();
				// XXX Any further handling ?
			}
		}

		/*
		 * Prepare all physical drives to get a host accessible
		 * device file.
		 */
		nrdrvs = rc->getPhysDrvCnt();
		for (int j = 0; j < nrdrvs; j++) {
			PhysDrv *pd = rc->getPhysDrv(j);
			DiskInfo *di = pd->getDiskInfo();
			if (!di->wipenow) {
				continue;
			}
			diskcount++;
			totalIOSize += di->dev_size * nrPasses;
			if (!rc->preparePhys(j)) {
				nrErrors++;
				updateErrors();
				continue;
				// XXX if preparation fails should we still
				// be doing a wipeit on it down below ?
			}
		}

	}


	totalIODone = 0;
	disknr = 1;
	totalProgress->setValue(0);
	totalTime->setText("N/A");

	/*
	 * First wipe locally attached disks.
	 */
	for (unsigned int i = 0; i < CurrentSysInfo->disks.size(); i++) {
		DiskInfo *disk = &CurrentSysInfo->disks[i];
		if (!disk->wipenow) {
			continue;
		}
		/*
		 * Wipe this disk
		 */
		QString disknr_str;
		disknr_str.sprintf("%d/%d", disknr, diskcount);
		diskNr->setText(disknr_str);
		diskSize->setText(QString(decimalkilos(disk->dev_size).c_str()));
		diskPassnr->setText(QString::number(1));
		diskProgress->setValue(0);
		diskThroughput->setText("N/A");
		diskTime->setText("N/A");

		QApplication::processEvents();

		/*
		 * Log wipe start
		 */
		struct UserAction act;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		act.dts = tv.tv_sec;
		act.user = userName;
		act.event = "WIPESTART";
		act.disk = disk;
		actions.push_back(act);

		/*
		 * Do the wipe.
		 */
		bret = wipeit(disk, nrPasses, userName, updateProgress, this);
		if (!bret && disk->errorno) {
			nrErrors++;
			QString str;
			str.sprintf("<b><font color=red>%d</font></b>",
				    nrErrors);
			wipeErrors->setText(str);
		}
		
		disknr++;
		totalIODone += disk->dev_size * nrPasses;

		/*
		 * Log wipe end or cancel.
		 */
		gettimeofday(&tv, NULL);
		act.dts = tv.tv_sec;
		//XXX check if bret == false for canceled too
		act.user = userName;
		if (canceled && !bret) {
			act.event = "WIPECANCEL";
		} else {
			act.event = "WIPEEND";
		}
		act.disk = disk;
		actions.push_back(act);

		if (canceled) {
			goto out;
		}
		disk->wipenow = false;
	}

	/*
	 * Then wipe RAID controller attached disks.
	 */

	nrctrls = CurrentSysInfo->RAIDCtlrs.size();
	for (unsigned i = 0; i < nrctrls; i++) {
		RAIDCtlr *rc = CurrentSysInfo->RAIDCtlrs[i];

		int nrdrvs = rc->getPhysDrvCnt();
		for (int j = 0; j < nrdrvs; j++) {
			PhysDrv *pd = rc->getPhysDrv(j);
			DiskInfo *di = pd->getDiskInfo();
			if (!di->wipenow) {
				continue;
			}

			/*
			 * Wipe this disk
			 */
			QString disknr_str;
			disknr_str.sprintf("%d/%d", disknr, diskcount);
			diskNr->setText(disknr_str);
			QString disksz_str = QString::fromStdString(
						decimalkilos(di->dev_size));
			diskSize->setText(disksz_str);
			diskPassnr->setText(QString::number(1));
			diskProgress->setValue(0);
			diskThroughput->setText("N/A");
			diskTime->setText("N/A");

			QApplication::processEvents();
			bret = wipeit(di, nrPasses, userName, updateProgress,
				      this);
			if (!bret && di->errorno) {
				nrErrors++;
				updateErrors();
			}
			
			disknr++;
			totalIODone += di->dev_size * nrPasses;

			if (canceled) {
				goto out;
			}
			di->wipenow = false;

		}
	}

out:
	if (mdlg->updateAuto->isChecked()) {
		DoUpdateDbDlg(this, mdlg, CurrentSysInfo, actions);
	}
	return !canceled;
}
#endif

void WipeDialog::on_cancelButton_clicked()
{
	int ret = QMessageBox::question(this, "Confirm Cancel",
			"Are you sure you want to cancel the wipe ?",
			QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);
	if (ret != QMessageBox::Yes) {
		return;
	}

	canceled = true;
}

void WipeDialog::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape) {
		/* Do Nothing */
	} else {
		QDialog::keyPressEvent(e);
	}
}
