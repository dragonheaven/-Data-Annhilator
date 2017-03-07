#ifndef __MAINDIALOG_H__
#define __MAINDIALOG_H__
#include <vector>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QDebug>
#include "ui_maindialog.h"
#include "wipeit.h"

#define UNLIMIT_VALUE		(30000)

class DiskEntry
{
	public:
	int disknr;
	int row;
	QLineEdit *serialnr;
	QCheckBox *wipecheck;
	DiskInfo *disk;

	DiskEntry()
	{
		serialnr = NULL;
		wipecheck = NULL;
		disk = NULL;
	}
};

class PromptCheckBox :public QCheckBox
{
public:
	PromptCheckBox(QWidget *parent = 0):QCheckBox(parent) {}
	void setPrompt(const QString &msg, const QString &ttl = "Confirm")
	{ message = msg; title = ttl; }
	void mousePressEvent(QMouseEvent *event);
private:
	QString message, title;
};

class MainDialog :public QDialog, public Ui::MainDialog
{
	Q_OBJECT
public:
	MainDialog(QWidget *parent = 0);
	DiskEntry *makeDiskEntry(int i, DiskInfo *disk, int row);
	int updateDiskState();
	void updateInfo(SysInfo *);
	bool scanSystem();
	void keyPressEvent(QKeyEvent *e);

	QString boot_devfile, boot_mntpt;
	QString localPath, localDBName;
	QString db_hostname, db_portnr, db_dbname, db_username, db_password;
	bool wmode_is_disk;
	bool showBootDrive;
	bool undetected_disks;
	bool m_unlimited;

private:
	std::vector<DiskEntry> diskEntries;
	void makeRetailEdition();
	PromptCheckBox *prcheck;
	void makeDiskState(DiskEntry *diskEntry);

	static const int RAID_CONTROLLER_COL = 0;
	static const int SIZE_COL = 1;
	static const int MFG_COL = 2;
	static const int MODEL_COL = 3;
 	static const int ENCL_ID_COL = 4;
	static const int MFG_SN_COL = 5;
	static const int SERIAL_NR_COL = 6;
	static const int STATE_COL = 7;
	static const int WIPE_CHECK_COL = 8;

private slots:
	void on_rtl_wipe_clicked();
	void on_rtl_history_clicked();
	void on_rtl_credits_clicked();
	void on_rtl_reboot_clicked();

	void on_doWipe_clicked();
	void on_updDB_clicked();
	void on_quit_clicked();
	void on_viewError_clicked();
	void on_saveReport_clicked();
	void on_rescanDisks_clicked();
	void resizeEvent(QResizeEvent *);
	void wmodeChanged(bool);
	void listBootDrive(bool);
};

extern bool retail_mode;
extern MainDialog *the_main_dialog;
extern QString the_da_version;
#endif
