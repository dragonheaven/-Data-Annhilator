#include <string>
#include <iostream>
#include <sstream>
#include <assert.h>
using namespace std;

#include <QtGui>
#include <QApplication>
#include <QDateTime>
#include "dbupdatedlg.h"
#include "maindialog.h"
#include "liblshw/osutils.h"

using namespace std;

bool getNrWipesFromDb(int *pNrWipes, QString DBName, QWidget *parent);
#include <QDebug>
extern SysInfo *CurrentSysInfo;
DBUpdateDlg::DBUpdateDlg(QWidget *parent):QDialog(parent)
{
	setupUi(this);
	pager->setCurrentIndex(0);
	mdlg = static_cast<MainDialog *>(parent);
	localDBName = mdlg->localDBName;
	localBtn->setText(localDBName);
	QString remoteDBName;
	remoteDBName.sprintf("QMYSQL://%s:%s@%s:%s/%s",
				qPrintable(mdlg->db_username),
				qPrintable(mdlg->db_password),
				qPrintable(mdlg->db_hostname),
			     	qPrintable(mdlg->db_hosturl),
			     	qPrintable(mdlg->db_port),
				qPrintable(mdlg->db_dbname));
	remoteBtn->setText(remoteDBName);
	backBtn->setEnabled(false);
	closeBtn->setEnabled(false);
}

void
logDB(QPlainTextEdit *log, char *fmt, ...)
{
	va_list		args;
	char msg[1024];

	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	log->appendPlainText(msg);
	log->repaint();
	QApplication::processEvents();
	va_end(args);
}

void
logDBError(QSqlDatabase *db, QPlainTextEdit *log, char *fmt, ...)
{
	if (!log)
	{
		return;
	}

	va_list		args;
	char msg[1024], fmsg[1024];

	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	snprintf(fmsg, sizeof(fmsg), "Database ERROR:Updation HALTED:"
		 "Error [%s], %s",
		 qPrintable(db->lastError().text()), msg);
	log->appendPlainText(fmsg);
	log->repaint();
	QApplication::processEvents();
	va_end(args);
}

void stashQuery(const QVariantMap& stash)
{
	QDir dir("stash");
	if (!dir.exists())
	{
		dir.mkdir(dir.path());
	}

	int name;
	auto list = dir.entryList(QStringList() << "*.stash", QDir::Files, QDir::Name | QDir::Reversed);
	if (list.count())
	{
		name = list[0].toInt();
	}
	else
	{
		name = 0;
	}
	name = name + 1;

	QFile file("stash/" + QString::number(name).rightJustified(8, '0') + ".stash");
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QDataStream out(&file);
    out << stash;
}

bool doPerformQuery(
	QSqlDatabase* db, QPlainTextEdit* log,
	const QString& query, const QVariantMap& params
)
{
	QSqlQuery qy(*db);
	qy.prepare(query);
	for (auto key : params.keys())
	{
		qy.bindValue(key, params[key]);
	}
	qy.exec();
	if (!qy.isActive()) {
		logDBError(db, log, "Insert failed!");
		return false;
	}
	return true;
}

bool performQuery(
	QSqlDatabase* localDb, QSqlDatabase* remoteDb, QPlainTextEdit* log,
	const QString& query, const QVariantMap& params
)
{
	if (!doPerformQuery(localDb, log, query, params))
	{
		return false;
	}

	if (!doPerformQuery(remoteDb, log, query, params))
	{
		QVariantMap stash;
		stash["type"] = "query";
		stash["query"] = query;
		stash["params"] = params;
		stashQuery(stash);
	}

	return true;
}

bool doPerformUpsertQuery(
	QSqlDatabase* db, QPlainTextEdit* log,
	const QString& selectQuery, const QVariantMap& selectParams,
	const QString& insertQuery, const QVariantMap& insertParams,
	const QString& updateQuery, const QVariantMap& updateParams
)
{
	QSqlQuery qy(*db);
	bool doUpdate = false;
	if (selectQuery.length())
	{
		qy.prepare(selectQuery);
		for (auto key : selectParams.keys())
		{
			qy.bindValue(key, selectParams[key]);
		}
		qy.exec();
		if (!qy.isActive()) {
			logDBError(db, log, "Select failed!");
			return false;
		}

		int nrrows = 0;
		while (qy.next()) {
			nrrows++;
		}
		if (nrrows == 1) {
			doUpdate = true;
		} else {
			doUpdate = false;
		}
	}

	
	if (doUpdate) {
		qy.prepare(updateQuery);
		for (auto key : updateParams.keys())
		{
			qy.bindValue(key, updateParams[key]);
		}
	} else {
		qy.prepare(insertQuery);
		for (auto key : insertParams.keys())
		{
			qy.bindValue(key, insertParams[key]);
		}
	}

	qy.exec();
	if (!qy.isActive()) {
		logDBError(db, log, "Update failed! [%s], rows = %d",
			   qPrintable(qy.lastError().text()),
			   qy.numRowsAffected());
		return false;
	} else {
		return true;
	}

	return true;
}

bool performUpsertQuery(
	QSqlDatabase* localDb, QSqlDatabase* remoteDb, QPlainTextEdit* log,
	const QString& selectQuery, const QVariantMap& selectParams,
	const QString& insertQuery, const QVariantMap& insertParams,
	const QString& updateQuery, const QVariantMap& updateParams
)
{
	if (!doPerformUpsertQuery(localDb, log,
								selectQuery, selectParams,
								insertQuery, insertParams,
								updateQuery, updateParams))
	{
		return false;
	}

	if (!doPerformUpsertQuery(remoteDb, log,
								selectQuery, selectParams,
								insertQuery, insertParams,
								updateQuery, updateParams))
	{
		QVariantMap stash;
		stash["type"] = "upsertQuery";
		stash["selectQuery"] = selectQuery;
		stash["selectParams"] = selectQuery;
		stash["insertQuery"] = insertQuery;
		stash["insertParams"] = insertQuery;
		stash["updateQuery"] = updateQuery;
		stash["updateParams"] = updateQuery;
		stashQuery(stash);
	}

	return true;
}

void
DBUpdateDlg::updateOver()
{
	backBtn->setEnabled(true);
	closeBtn->setEnabled(true);
}

QString
wipestate_str(enum DiskInfo::wipestate state)
{
	const char *strs[] = {
		"UNWIPED", "PARTWIPE", "WIPED", "IOERROR", "BADDISK"
	};

	return QString(strs[state]);
}

bool UpdateDiskDb(DiskInfo *disk, SysInfo *sys,
		  QSqlDatabase *localDb, QSqlDatabase *remoteDb, QPlainTextEdit *log)
{
	if (!retail_mode) {
		logDB(log, "Updating disk info for serial no [%s], "
			   "manufacturer [%s], model [%s], size [%s].",
			   disk->serialnr.c_str(), disk->vendor.c_str(),
			   disk->model.c_str(),
			   decimalkilos(disk->dev_size).c_str());
	}

	int nrWipes = 0;
	if (retail_mode) {
		int ret;
		ret = getNrWipesFromDb(&nrWipes,
					the_main_dialog->localDBName,
					the_main_dialog);
		if (!ret) {
			return false;
		}
	}

	QString selectQuery;
	QVariantMap selectParams;
	if (disk->serialnr.length())
	{
		selectQuery = "select Id from tmptbldatadestructionitem where SerialNo = :sn";
		selectParams[":sn"] = QString::fromStdString(disk->serialnr);
	}

	QString insertQuery = "insert into tmptbldatadestructionitem "
			 "(SerialNo, ItemType, ItemSize, "
			 "ItemManufacturer, ItemModel, "
			 "ItemEnclId, ItemManufacturerSN, "
			 "RPM, FormFactor, "
			 "Cylinders, Heads, SectorsPerTrack, NumberOfSectors, "
			 "PhysicalSectorSize, CacheSize, "
			 "WipeStartDate, WipeEndDate, "
			 "WipeResult, NumberOfPasses, "
			 "UserId, StationId, "
			 "StationSerialNo, WipeVersion, CreditBalance) values "
			 "(:sn, :it, :is, :imfr, :imodel, :ienclid, :imfrsn, "
			 ":rpm, :ff, :cyl, :head, :spt, :nos, :pss, :cachesz, "
			 ":wipestart, :wipeend, :wres, :nrpasses, :user, "
			 "("
			 	"select Id from tmptbldatadestructionstation "
			   	"where SerialNo = :s_sn and "
			   	"Manufacturer = :s_mfr and Model = :s_model"
			 "), :ssn, :wv, :cb)";
	QVariantMap insertParams;

	QString updateQuery = "update tmptbldatadestructionitem set "
			 "SerialNo = :sn, ItemType = :it, ItemSize = :is, "
			 "ItemManufacturer = :imfr, ItemModel = :imodel, "
			 "ItemEnclId = :ienclid, ItemManufacturerSN = :imfrsn, "
			 "RPM = :rpm, FormFactor = :ff, "
			 "Cylinders = :cyl, Heads = :head, "
			 "SectorsPerTrack = :spt, NumberOfSectors = :nos, "
			 "PhysicalSectorSize = :pss, CacheSize = :cachesz, "
			 "WipeStartDate = :wipestart, WipeEndDate = :wipeend, "
			 "WipeResult = :wres, NumberOfPasses = :nrpasses, "
			 "UserId = :user, StationId = ("
			 	"select Id from tmptbldatadestructionstation "
			   	"where SerialNo = :s_sn and "
			   	"Manufacturer = :s_mfr and Model = :s_model"
			 "), "
			 "StationSerialNo = :ssn, "
			 "CreditBalance = :cb, "
			 "WipeVersion = :wv "
			 "where SerialNo = :sn2";
	QVariantMap updateParams;

	insertParams[":wv"] = the_da_version;
	insertParams[":cb"] = nrWipes;
	insertParams[":sn"] = QString::fromStdString(disk->serialnr);

	/*
	 * If we are doing a disk level wipe, station rowid and serial nr
	 * are null for the disk's row.
	 */
	insertParams[":s_sn"] = QString::fromStdString(sys->serialnr);
	insertParams[":s_mfr"] = QString::fromStdString(sys->vendor);
	insertParams[":s_model"] = QString::fromStdString(sys->model);
	insertParams[":ssn"] = QString::fromStdString(sys->serialnr);
	insertParams[":it"] = QString::fromStdString(disk->type);
	insertParams[":is"] = disk->dev_size / 1048576LL;
	insertParams[":imfr"] = QString::fromStdString(disk->vendor);
	insertParams[":imodel"] = QString::fromStdString(disk->model);
	insertParams[":ienclid"] = QString::fromStdString(disk->enclid);
	insertParams[":imfrsn"] = QString::fromStdString(disk->hwserialnr);

	fprintf(stderr, "Disk RPM = %d\n", disk->rpm);
	insertParams[":rpm"] = disk->rpm;
	insertParams[":ff"] = QString::fromStdString(disk->form_factor);
	insertParams[":cyl"] = disk->cylinders;
	insertParams[":head"] = disk->heads;
	insertParams[":spt"] = disk->sectors_per_track;
	insertParams[":nos"] = disk->number_of_sectors;
	insertParams[":pss"] = disk->physical_sector_size;
	insertParams[":cachesz"] = disk->cache_size;

	insertParams[":wres"] = QString(wipestate_str(disk->wiped_state));
	if (disk->wiped_state == DiskInfo::UNWIPED) {
		insertParams[":nrpasses"] = QVariant(QVariant::Int);
		insertParams[":user"] = QVariant(QVariant::String);
		insertParams[":wipestart"] = QVariant(QVariant::DateTime);
		insertParams[":wipeend"] = QVariant(QVariant::DateTime);
	} else {
		insertParams[":nrpasses"] = disk->wiped_mode;
		insertParams[":user"] = QString::fromStdString(disk->wiped_user);
		insertParams[":wipestart"] = QDateTime::fromTime_t(disk->wiped_start);
		insertParams[":wipeend"] = QDateTime::fromTime_t(disk->wiped_date);
	}

	updateParams = insertParams;
	if (disk->serialnr.length()) {
		updateParams[":sn2"] = QString::fromStdString(disk->serialnr);
	}

	performUpsertQuery(localDb, remoteDb, log,
					selectQuery, selectParams,
					insertQuery, insertParams,
					updateQuery, updateParams);

	return true;
}

void
UpdateMachDb(SysInfo *sys, QSqlDatabase *localDb, QSqlDatabase *remoteDb, QPlainTextEdit *log)
{
	if (!retail_mode) {
		logDB(log, "Updating station info for serial no [%s], "
			   "manufacturer [%s], model [%s].",
			   sys->serialnr.c_str(), sys->vendor.c_str(),
			   sys->model.c_str());
	}

	QString selectQuery = "select Id from tmptbldatadestructionstation "
		   "where SerialNo = :sn and "
		   "Manufacturer = :mfr and Model = :model";
	QVariantMap selectParams;
	selectParams[":sn"] = QString::fromStdString(sys->serialnr);
	selectParams[":mfr"] = QString::fromStdString(sys->vendor);
	selectParams[":model"] = QString::fromStdString(sys->model);

	QString insertQuery = "insert into tmptbldatadestructionstation "
			 "(SerialNo, Manufacturer, Model, ProcessorType, "
			 "ProcessorCount, CoreCount, ThreadsPerProc, "
			 "Memory, ManufacturerSN, ItemCount) "
			 "values "
			 "(:sn, :mfr, :model, :ptype, :pcount, :ccount, "
			 ":tpproc, :memory, :mfrsn, :ic)";
	QVariantMap insertParams;

	QString updateQuery = "update tmptbldatadestructionstation set "
			 "SerialNo = :sn, Manufacturer = :mfr, Model = :model, "
			 "ProcessorType = :ptype, ProcessorCount = :pcount, "
			 "CoreCount = :ccount, ThreadsPerProc = :tpproc, "
			 "Memory = :memory, ManufacturerSN = :mfrsn, "
			 "ItemCount = :ic "
			 "where SerialNo = :sn2 and "
			 "Manufacturer = :mfr2 and Model = :model2";
	QVariantMap updateParams;

	insertParams[":sn"] = QString::fromStdString(sys->serialnr);
	insertParams[":mfr"] = QString::fromStdString(sys->vendor);
	insertParams[":model"] = QString::fromStdString(sys->model);
	insertParams[":ptype"] = QString::fromStdString(sys->processor);
	insertParams[":pcount"] = sys->processor_count;
	insertParams[":ccount"] = sys->processor_cores;
	insertParams[":tpproc"] = sys->processor_threads;
	insertParams[":memory"] = sys->memory / 1048576LL;
	insertParams[":mfrsn"] = QString::fromStdString(sys->hwserialnr);
	insertParams[":ic"] = (int)sys->disks.size();

	updateParams = insertParams;
	updateParams[":sn2"] = QString::fromStdString(sys->serialnr);
	updateParams[":mfr2"] = QString::fromStdString(sys->vendor);
	updateParams[":model2"] = QString::fromStdString(sys->model);

	performUpsertQuery(localDb, remoteDb, log,
					selectQuery, selectParams,
					insertQuery, insertParams,
					updateQuery, updateParams);
}

bool
UpdateDb(bool wmode_is_disk, SysInfo *sys, QSqlDatabase *localDb, QSqlDatabase *remoteDb,
	 QPlainTextEdit *log)
{
	bool bret;

	if (!wmode_is_disk) {
		UpdateMachDb(sys, localDb, remoteDb, log);

		/*
		 * Clear the disks for this machine row id if it already
		 * existed.
		 */
		if (!retail_mode) {
			logDB(log, "Clearing disk info of this machine.");
		}
		QSqlQuery qy(*localDb);
		qy.prepare("delete from tmptbldatadestructionitem where "
			   "StationId = "
			   "("
			 		"select Id from tmptbldatadestructionstation "
			   		"where SerialNo = :s_sn and "
			   		"Manufacturer = :s_mfr and Model = :s_model"
			 	")");
		qy.bindValue(":s_sn", QString::fromStdString(sys->serialnr));
		qy.bindValue(":s_mfr", QString::fromStdString(sys->vendor));
		qy.bindValue(":s_model", QString::fromStdString(sys->model));
		qy.exec();
		if (!qy.isActive()) {
			logDBError(localDb, log, "Delete failed!");
			return false;
		}
	}

	for (unsigned i = 0; i < sys->disks.size(); i++) {
		DiskInfo *disk = &sys->disks[i];
		if (disk->bootdisk) {
			continue;
		}
		bret = UpdateDiskDb(disk, sys, localDb, remoteDb, log);
		if (!bret) {
			return false;
		}
	}

	return true;
}

bool
UpdateActionsDb(const vector<UserAction> &actions, QSqlDatabase *localDb, QSqlDatabase *remoteDb,
		QPlainTextEdit *log)
{
	if (!actions.size()) {
		return true;
	}

	logDB(log, "Logging user actions.");

	QSqlQuery qy(*localDb);
	for (unsigned i = 0; i < actions.size(); i++) {
		const UserAction *act = &actions[i];

		QVariantMap params;
		params[":user"] = QString::fromStdString(act->user);
		params[":dts"] = QDateTime::fromTime_t(act->dts);
		params[":action"] = QString::fromStdString(act->event);
		params[":it"] = QString::fromStdString(act->disk->type);
		if (act->disk->serialnr.length()) {
			params[":sn"] = QString::fromStdString(act->disk->serialnr);
		} else {
			params[":sn"] = "<Undefined>";
		}
		if (act->note.length()) {
			params[":note"] = QString::fromStdString(act->note);
		} else {
			params[":note"] = QVariant(QVariant::String);
		}
		params[":itid"] = QString::fromStdString(act->disk->serialnr);

		performQuery(localDb, remoteDb, log,
			"insert into tmptbldatadestructionhistory "
			   "(UserId, DTS, Action, ItemType, SerialNo, Note, "
			   "ItemId) "
			   "values "
			   "(:user, :dts, :action, :it, :sn, :note, "
			   "(select Id from tmptbldatadestructionitem where SerialNo = :itid))",
			  params);
	}

	return true;
}

void
DBUpdateDlg::on_updateBtn_clicked()
{
	pager->setCurrentIndex(1);

	QSqlDatabase localDb;
	QSqlDatabase remoteDb;
	logDB(DBErrorLog, "Connecting to local database ...");
	localDb = QSqlDatabase::addDatabase("QSQLITE", "WIPEDB");
	localDb.setDatabaseName(localDBName);
	if (!localDb.open()) {
		logDBError(&localDb, DBErrorLog,
			   "Could not open local SQLite database");
		goto over;
	}
	
	remoteDb = QSqlDatabase::addDatabase("QMYSQL", "REMOTEWIPEDB");
	remoteDb.setHostName("12.41.72.28");
	remoteDb.setPort(3306);
	remoteDb.setDatabaseName("gpdb");
	remoteDb.setUserName("allgreen2");
	remoteDb.setPassword("7Ld4S8d4TaDWApVW");
	if (!remoteDb.open()) {
		logDBError(&remoteDb, DBErrorLog,
			   "Could not open all green database");
	}

	UpdateDb(mdlg->wmode_is_disk, CurrentSysInfo, &localDb, &remoteDb, DBErrorLog);
	if (localDb.isOpen()) {
		localDb.close();
	}
	if (remoteDb.isOpen()) {
		remoteDb.close();
	}
over:
	updateOver();
}

void
DBUpdateDlg::on_backBtn_clicked()
{
	pager->setCurrentIndex(0);
	DBErrorLog->clear();
	backBtn->setEnabled(false);
	closeBtn->setEnabled(false);
}

void
DBUpdateDlg::on_closeBtn_clicked()
{
	accept();
}

DBAutoUpdateDlg::DBAutoUpdateDlg(QWidget *parent):QDialog(parent)
{
	setupUi(this);
}

bool UpdateHistDb(SysInfo *sys, const vector<UserAction> &actions, QSqlDatabase *db, 
	QPlainTextEdit *log, MainDialog *md)
{
	int wiped_disks = 0, canceled_disks = 0, error_disks = 0;
	QString wiped_disks_str, canceled_disks_str, error_disks_str;

	QDateTime now = QDateTime::currentDateTime();
	QString nowstr = now.toString("d MMM yyyy h:mm AP");
	QString sys_str, disp_str;

	sys_str = QString::fromStdString("\tVendor: " + sys->vendor + "\n" +
		  "\tModel: " + sys->model + "\n" +
		  "\tProcessor: " + sys->processor + "\n" +
		  "\tMemory: " + decimalkilos(sys->memory) + "\n" +
		  "\tHW serial: " + sys->hwserialnr + "\n");

	for (unsigned i = 0; i < actions.size(); i++) {
		const UserAction *act = &actions[i];
		DiskInfo *disk = act->disk;
		QString diskstr = QString::fromStdString(disk->vendor + ", " +
			disk->model + ", " +
			decimalkilos(disk->dev_size) + ", " + disk->hwserialnr);
		if (act->event == "WIPECANCEL") {
			canceled_disks++;
			canceled_disks_str += ("\t" + diskstr + "\n");
		} else if (act->event == "WIPEEND") {
			if (act->disk->wiped_state != DiskInfo::WIPED) {
				error_disks++;
				error_disks_str += ("\t" + diskstr + "\n");
			} else {
				wiped_disks++;
				wiped_disks_str += ("\t" + diskstr + "\n");
			}
		}
	}

	QString wipeinfostr;
	if (wiped_disks) {
		wipeinfostr = "System Information:\n" + sys_str +
					"Disk Information\n" + wiped_disks_str;
		wipeinfostr = wipeinfostr.trimmed();
		QSqlQuery qy(*db);
		qy.prepare("insert into wipehistory(DTS, WipeInfo) values (:dts, :wipeinfo)");
		qy.bindValue(":dts", now);
		qy.bindValue(":wipeinfo", wipeinfostr);
		qy.exec();
		if (!qy.isActive()) {
			logDBError(db, log, "Insert failed!");
			return false;
		}
		logDB(log, "Wiping history updated.");
	}

	wipeinfostr = "System information:\n" + sys_str + "\n";
	if (wiped_disks) {
		wipeinfostr += QString("Disks wiped successfully: %1\n").arg(wiped_disks);
		wipeinfostr += wiped_disks_str + "\n";
	}
	if (canceled_disks) {
		wipeinfostr += QString("Disks cancelled: %1\n").arg(canceled_disks);
		wipeinfostr += canceled_disks_str + "\n";
	}
	if (error_disks) {
		wipeinfostr += QString("Disks which failed during wiping: %1\n")
				.arg(error_disks);
		wipeinfostr += error_disks_str + "\n";
	}
	
	if (wiped_disks + canceled_disks + error_disks == 0) {
		wipeinfostr += "No disks wiped!\n";
	}

	int nrWipes, ret;
	ret = getNrWipesFromDb(&nrWipes, md->localDBName, md);
	if (!ret) {
		wipeinfostr += "<ERROR: Could not retrieve credit balance from database>\n";
	} else {
		wipeinfostr += QString("Number of wipes remaining: %1\n").arg(nrWipes);
		wipeinfostr += "Please visit www.allgreenrecycling.com to purchase more wiping "
				"credits.\n";
		wipeinfostr += "Thank you for choosing All Green Data Annihilator.";
	}

	logDB(log, (char *)qPrintable(wipeinfostr));
	return true;
}

bool
DoUpdateDbDlg(QWidget *parent, MainDialog *mdlg, SysInfo *sys,
	      const vector<UserAction> &actions)
		
{
	/*
	DBAutoUpdateDlg *dlg = new DBAutoUpdateDlg(parent);
	bool updated = false;

	dlg->show();
	dlg->setModal(true);

	{
	QSqlDatabase db;

	if (retail_mode) {
		db = QSqlDatabase::addDatabase("QSQLITE", "WIPEDB");
		db.setDatabaseName(mdlg->localDBName);
		if (!db.open()) {
			logDB(dlg->DBErrorLog,
				   "Could not update history database, "
				   "HALTING UPDATION.");
			goto execwait;
		}
		dlg->progressBar->setValue(25);
		if (!UpdateDb(mdlg->wmode_is_disk, sys, &db, dlg->DBErrorLog)) {
			goto close;
		}
		dlg->progressBar->setValue(50);
		if (!UpdateHistDb(sys, actions, &db, dlg->DBErrorLog, mdlg)) {
			goto close;
		}
		dlg->progressBar->setValue(100);
	} else {
		logDB(dlg->DBErrorLog, "Connecting to remote database ...");
		db = connectRemote(mdlg);
		if (!db.isValid()) {
			logDB(dlg->DBErrorLog, "Could not connect to remote DB, %s.",
				qPrintable(db.lastError().text()));
			logDB(dlg->DBErrorLog, "Trying to open local database");
			
			db = QSqlDatabase::addDatabase("QSQLITE", "WIPEDB");
			db.setDatabaseName(mdlg->localDBName);
			if (!db.open()) {
				logDB(dlg->DBErrorLog,
					   "Could not open local SQLite database, "
					   "HALTING UPDATION.");
				goto execwait;
			}
		}
		if (!UpdateDb(mdlg->wmode_is_disk, sys, &db, dlg->DBErrorLog)) {
			goto close;
		}
		if (mdlg->updateActions->isChecked()) {
			if (!UpdateActionsDb(actions, &db, dlg->DBErrorLog)) {
				goto close;
			}
		}
	}
	updated = true;
	if (!retail_mode) {
		logDB(dlg->DBErrorLog, "Updation DONE.");
	}

close:
	db.close();
	}

execwait:
	QSqlDatabase::removeDatabase("WIPEDB");
	dlg->OkBtn->setEnabled(true);
	dlg->exec();

	return updated;
	*/
}
