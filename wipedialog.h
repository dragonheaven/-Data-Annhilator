#ifndef __WIPEDIALOG_H__
#define __WIPEDIALOG_H__
#include <vector>
#include <string>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include "ui_wipedialog.h"
#include "wipeit.h"
#include "maindialog.h"

#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

class WipeDialog;
struct DiskWipeUi
{
	QLabel *diskSize, *passNr, *sectorsWiped;
	QProgressBar *progressBar;
	QLabel *throughput, *completionTime;
	DiskInfo *disk;
	u64_t IODone, lastThroughput;
	WipeDialog *wipeDialog;
	QVBoxLayout *ui;
};
bool updateProgress(void *privData, DiskInfo *disk,
				float throughput, int passnr,
				u64_t written);

class WipeDialog :public QDialog, public Ui::WipeDialog
{
	Q_OBJECT
public:
	int nrPasses, nrErrors, maxWipers;
	std::string userName;
	u64_t totalIOSize, totalIODone, totalThroughput;
	int totalPercent;
	struct timeval wipeStartTime;
	volatile bool canceled;

	std::vector<UserAction> userActions;

	WipeDialog(QWidget *parent = 0);
	bool doWipe();
	void keyPressEvent(QKeyEvent *e);

public slots:
	void on_cancelButton_clicked();

private:
	void updateErrors(void);
	MainDialog *mdlg;

	std::vector<DiskWipeUi *> diskUis;
	pthread_mutex_t totalsLock;
	sem_t threadExit;
	friend bool updateProgress(void *privData, DiskInfo *disk,
					float throughput, int passnr,
		 			u64_t written, u64_t sectorsdone);
	friend void *wiper_thread(void *arg); 
};
#endif
