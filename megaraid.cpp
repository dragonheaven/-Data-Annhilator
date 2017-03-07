#include <iostream>
#include <QString>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "megaraid.h"

using namespace std;

int runOut(QString exe, QStringList args, QString &output);

QString
extractOne(QString &str, QString pattern, QString defval = QString())
{
	QRegExp re(pattern);

	if (re.indexIn(str) < 0) {
		return defval;
	}
	if (re.captureCount() < 1) {
		return defval;
	}
	return re.cap(1).simplified();
}

QStringList
extractMany(QString &str, QString pattern)
{
	QRegExp re(pattern);

	if (re.indexIn(str) < 0) {
		return QStringList();
	}
	if (re.captureCount() < 1) {
		return QStringList();
	}
	QStringList out, caps = re.capturedTexts();
	for (int i = 1; i < caps.size(); i++) {
		out << caps[i].trimmed();
	}
	return out;
}

QList<QStringList>
extractGroup(QString &input, QString pattern)
{
	QRegExp re(pattern);
	re.setMinimal(true);
	QList<QStringList> out;
	QStringList matches;
	int inputlen, i, pos;

	inputlen = input.length();
	i = 0;
	while (i < inputlen) {
		pos = re.indexIn(input, i);
		if (pos < 0) {
			break;
		}
		i = pos + re.cap(0).length();
		matches = re.capturedTexts().mid(1);
		out << matches;
	}
	return out;
}

bool MegaRAID::scanned = false;
vector<MegaRAIDCtlr> MegaRAID::Ctlrs;

void
dumplist(QStringList &list, char *title = 0)
{
	if (title) {
		cerr << title << endl;
	}
	for (int i = 0; i < list.size(); i++) {
		cerr << list[i].toStdString() << endl;
	}
}

QStringList
getSCSIHosts()
{
	QString scsihost = "/sys/class/scsi_host";
	QDir dir(scsihost, "host*", QDir::Name);
	QStringList host_ids = dir.entryList();

	QStringList hostdrvs;
	for (int i = 0; i < host_ids.size(); i++) {
		QFile file(scsihost + "/" + host_ids[i] + "/proc_name");
		if (!file.open(QIODevice::ReadOnly)) {
			hostdrvs << "<unknown>";
			continue;
		}
		QString drvname = file.readLine();
		drvname = drvname.trimmed();
		if (drvname.isEmpty()) {
			hostdrvs << "<unknown?>";
			continue;
		}
		hostdrvs << drvname;
	}
	dumplist(hostdrvs);
	return hostdrvs;
}

int MegaRAID::getCtlrCnt()
{
	QString out;
	int ret;

	if (scanned) {
		return Ctlrs.size();
	}
		
	ret = runOut("./MegaCli", QStringList() << "-AdpCount", out);
	QString countstr;
	countstr = extractOne(out, "Controller Count:\\s*(\\d+)", "0");
	int cnt = countstr.toInt();
	if (ret != cnt) {
		log_error("MegaCli -AdpCount returned with exit code %d\n"
			  "This does not match output which says\n"
			  "Output:[%s]\n", ret, qPrintable(out));
		return 0;
	}

#ifndef NOHOSTACCESS
	QStringList hostdrvs = getSCSIHosts();
	int hostindex = 0;
#endif

	for (int i = 0; i < cnt; i++) {
		MegaRAIDCtlr ctlr(i);
#ifndef NOHOSTACCESS
		int id = hostdrvs.indexOf(QRegExp(".*megaraid.*"), hostindex);
		if (id >= 0) {
			hostindex = id;
		} else {
			log_error("Megaraid:Could not determine hostid for "
				  "adapter");
			continue;
		}
#else
		int id = 0;
#endif

		ret = runOut("./MegaCli", QStringList()
				<< "-AdpAllinfo"
				<< "-a" + QString::number(i), out);
		if (ret != 0) {
			log_error("MegaCli -AdpAllinfo Count failed "
				  "with exit code %d\n"
				  "Output:[%s]\n", ret, qPrintable(out));
			continue;
		}
		QString name;
		name = extractOne(out, "Product Name\\s*:\\s*([^\\n]+)\\n",
				  "MegaRAID adapter card");
		name = name.trimmed();
		ctlr.name = name.toStdString();
		ctlr.hostID = id;
		Ctlrs.push_back(ctlr);

#ifdef TEST
		fprintf(stderr, "Controller %d %s\n", i, qPrintable(name));
#endif
	}

	return Ctlrs.size();
}

RAIDCtlr *MegaRAID::getCtlr(int i)
{
	if (i < (signed)Ctlrs.size()) {
		return &Ctlrs[i];
	} else {
		return NULL;
	}
}

/*
 * A MegaRAID controller.
 */
MegaRAIDCtlr::MegaRAIDCtlr(int i)
{
	ID = i;
	scandone = false;
}

string MegaRAIDCtlr::getName()
{
	return name;
}

QString
getDevName(int hostID, int targetID)
{
	QString SCSITargets, devdir = "/sys/bus/scsi/devices/";
	SCSITargets.sprintf("%d:*:%d:*", hostID, targetID);
	QDir dir(devdir, SCSITargets, QDir::Name);
	QStringList targets = dir.entryList();
	int i;

	if (targets.size() == 0) {
		return QString();
	}
	dumplist(targets, "targets");
	for (i = 0; i < targets.size(); i++) {
		QFile file(devdir + targets[i] + "/type");
		if (!file.open(QIODevice::ReadOnly)) {
			continue;
		}
		QString type = file.readLine();
		bool ok;
		if (type.toInt(&ok) != 0 || !ok) {
			continue;
		}
		break;
	}
	if (i == targets.size()) {
		return QString();
	}

	QDir bdir(devdir + targets[i] + "/block/", "*", QDir::Name,
				QDir::AllEntries | QDir::NoDotAndDotDot);
	QStringList bents = bdir.entryList();
	dumplist(bents, "bents");
	if (bents.size() == 0) {
		return QString();
	}

	return "/dev/" + bents[0];
}

u64_t
getSize(QString &size, QString &units)
{
	u64_t scaling;

	units = units.toUpper();
	if (units == "TB") {
		scaling = 1024ULL*1024*1024*1024;
	} else if (units == "GB") {
		scaling = 1024ULL*1024*1024;
	} else if (units == "MB") {
		scaling = 1024ULL*1024;
	} else if (units == "KB") {
		scaling = 1024ULL;
	}
	return (u64_t)(size.toDouble() * scaling);
}

int
MegaRAIDCtlr::scan()
{
	QString out;
	int ret;

	scandone = true;

	/*
	 * First get the list of physical drives in the system with
	 * their enclosure and slot nrs.
	 */
	ret = runOut("./MegaCli", QStringList() << "-PDList"
				  << "-a" + QString::number(ID), out);
	if (ret != 0) {
		log_error("MegaCli -PDList failed with exit code %d\n"
			  "Output:[%s]\n", ret, qPrintable(out));
		return 0;
	}

	QList<QStringList> drvstrs;
	drvstrs = extractGroup(out, "Enclosure Device ID:\\s*(\\d+)\\s*"
				    "Slot Number:\\s*(\\d+)\\s+.*"
				    "Raw Size:\\s*([.\\d]+)\\s+(\\S\\S)");
	for (int i = 0; i < drvstrs.size(); i++) {
		QStringList fields = drvstrs[i];
		MegaPhysDrv physdrv;

		physdrv.enclID = fields[0].toInt();
		physdrv.slotNr = fields[1].toInt();
		physdrv.vdrvID = -1;

		/*
		 * Initialize the embedded DiskInfo structure.
		 */
		physdrv.diskInfo.vendor = "DELL";
		physdrv.diskInfo.model = name;
		physdrv.diskInfo.serialnr = "11111";
		char enclid[30];
		snprintf(enclid, sizeof(enclid), "[%d:%d]",
			 physdrv.enclID, physdrv.slotNr);
		physdrv.diskInfo.enclid = enclid;
		physdrv.diskInfo.dev_size = getSize(fields[2], fields[3]);
		physDrvs.push_back(physdrv);

		fprintf(stderr, "E:S = %d:%d, size = %s,%sL%llu\n", fields[0].toInt(), fields[1].toInt(), qPrintable(fields[2]), qPrintable(fields[3]), physdrv.diskInfo.dev_size);
	}

	/*
	 * Get virtual drives.
	 */
	ret = runOut("./MegaCli", QStringList() << "-LDPDInfo"
				  << "-a" + QString::number(ID), out);
	if (ret != 0) {
		log_error("MegaCli -LDPDInfo failed with exit code %d\n"
			  "Output:[%s]\n", ret, qPrintable(out));
		return 0;
	}

	QString vdcntstr;
	vdcntstr = extractOne(out, "Number of Virtual Disks:\\s*(\\d+)", "0");
	QStringList vdstrs;
	vdstrs = out.split("Virtual Drive: ");
	if (vdstrs.size() != vdcntstr.toInt() + 1) {
		log_error("MegaCLI Virtual drive info does not match count");
		return 0;
	}
	for (int i = 1; i < vdstrs.size(); i++) {
		MegaVirtDrv virtdrv;
		QStringList strs;
		strs = extractMany(vdstrs[i],
				   "\\s*(\\d+)\\D*Target Id: (\\d+).*"
				   "RAID Level\\s*:([^\n]+)\\n.*"
				   "Number Of Drives\\s*:\\s*(\\d+).*");
		//XXX check strs.size() == 4
		virtdrv.ctlrID = ID;
		virtdrv.ID = strs[0].toInt();
		virtdrv.targetID = strs[1].toInt();
		virtdrv.numDisks = strs[2].toInt();
		virtdrv.RAIDLevel = strs[3].toStdString();

		QList<QStringList> drvstrs;
		drvstrs = extractGroup(vdstrs[i],
					"Enclosure Device ID:\\s*(\\d+)\\s+"
					"Slot Number:\\s*(\\d+)\\s+");
		for (int j = 0; j < drvstrs.size(); j++) {
			QStringList fields = drvstrs[j];
			int enclID, slotNr, drvNr;

			enclID = fields[0].toInt();
			slotNr = fields[1].toInt();
			drvNr = getPhysDrvES(enclID, slotNr);
			if (drvNr < 0) {
				//XXX
				assert(0);
			}
			virtdrv.disks.push_back(drvNr);
		}

		fprintf(stderr, "Virt drive #%d: %d %d %s\n",
			virtdrv.ID,
			virtdrv.targetID,
			virtdrv.numDisks,
			virtdrv.RAIDLevel.c_str());
		for (int j = 0; j < (signed)virtdrv.disks.size(); j++) {
			int e, s;
			e = physDrvs[virtdrv.disks[j]].enclID;
			s = physDrvs[virtdrv.disks[j]].slotNr;
			fprintf(stderr, "\tDisk %d: %d:%d\n", j, e, s);
		}

#ifndef NOHOSTACCESS
		/*
		 * Determine device name on host for this virtual drive.
		 */
		QString devName = getDevName(hostID, virtdrv.targetID);
		if (devName.isEmpty()) {
			continue;
		}
		virtdrv.devName = devName.toStdString();

		/*
		 * Determine if this virtual disk was not created by wipeit
		 * to map a physical disk. Two conditions must be satisfied
		 * for it to be a mapping virtual disk,
		 * 1. Only one subdisk
		 * 2. Has signature.
		 * If its not a mapping virtual disk we mark it for destruction.
		 */
		if (virtdrv.disks.size() > 1 || !sign_exists(virtdrv.devName)) {
			virtdrv.toDestroy = true;
		} else {
			virtdrv.toDestroy = false;
		}
#else
		virtdrv.devName = "";
		virtdrv.toDestroy = false;
#endif
		virtDrvs.push_back(virtdrv);
		fprintf(stderr, "Virt device name: %s, %p\n", virtDrvs.back().devName.c_str(), &virtDrvs.back());

		/*
		 * Point subdisks to this virtual drive.
		 */
		for (int j = 0; j < (signed)virtdrv.disks.size(); j++) {
			int subdisk = virtdrv.disks[j];
			physDrvs[subdisk].vdrvID = virtdrv.ID;
		}
	}

#ifndef NOHOSTACCESS
	for (unsigned i = 0; i < physDrvs.size(); i++) {
		int e = physDrvs[i].enclID;
		int n = physDrvs[i].slotNr;
		cerr << "Beginning to prep:" << i << endl;
		if (preparePhys(i)) {
			fprintf(stderr, "Successfully prepared %d:%d\n", e, n);
		} else {
			fprintf(stderr, "FAILED to prepare %d:%d\n", e, n);
		}
	}
#endif
	return 0;

}

int
MegaRAIDCtlr::getPhysDrvES(int enclID, int slotNr)
{
	for (int i = 0; i < (signed)physDrvs.size(); i++) {
		if (physDrvs[i].enclID == enclID &&
		    physDrvs[i].slotNr == slotNr) {
			return i;
		}
	}

	return -1;
}

MegaVirtDrv *
MegaRAIDCtlr::getVirtDrvByID(int ID)
{
	for (vector<MegaVirtDrv>::iterator iter = virtDrvs.begin();
	     iter != virtDrvs.end();
	     iter++) {
		fprintf(stderr, "VD:%p %d %d\n", &*iter, ID, iter->ID);
		if (iter->ID == ID) {
			return &*iter;
		}
	}
	return NULL;
}


bool
MegaRAIDCtlr::preparePhys(int idx)
{
	MegaPhysDrv *pd = &physDrvs[idx];
	MegaVirtDrv *vd;
	int ret;
	QString out;

	if (pd->vdrvID == -1) {
		cerr << "No VD\n";
		ret = runOut("./MegaCli", QStringList() << "-CfgLDAdd"
				  << "-R0[" + QString::number(pd->enclID) + ":"
				  	+ QString::number(pd->slotNr) + "]"
				  << "-a" + QString::number(ID), out);
		if (ret != 0) {
			log_error("MegaCli -CfgLDAdd failed with exit code %d\n"
				  "Output:[%s]\n", ret, qPrintable(out));
			return false;
		}
		QString idstr = extractOne(out, "Created VD\\s*(\\d+)");
		if (idstr.isEmpty()) {
			log_error("preparePhys:Created mapping VD:"
				  "Could not get ID:Output [%s]\n",
				  qPrintable(out));
			return false;
		}
		int id = idstr.toInt();
		pd->vdrvID = id;
		MegaVirtDrv virtdrv;
		virtdrv.ctlrID = ID;
		virtdrv.ID = id;
		virtdrv.targetID = id;
		virtdrv.numDisks = 1;
		virtdrv.RAIDLevel = "Primary-0, Secondary-0, "
				 "RAID Level Qualifier-0";
		QString devName = getDevName(hostID, virtdrv.targetID);
		virtdrv.devName = devName.toStdString();
		virtdrv.disks.push_back(idx);
		virtdrv.toDestroy = false;
		virtDrvs.push_back(virtdrv);
		vd = &*(virtDrvs.end() - 1);
	} else {
		vd = getVirtDrvByID(pd->vdrvID);
		assert(vd);
		if (vd->toDestroy) {
			/*
			 * Physical drive used by non wipeit created virtual
			 * drive. Have to wait till the virtual drive is
			 * deleted.
			 */
			 return false;
		}
	}

	string aword;
	cerr << "Prepared disk with devname:" << vd->devName;
	cin >> aword;
	pd->diskInfo.dev_file = vd->devName;
	return check_disk(pd->diskInfo);
}

bool
MegaRAIDCtlr::destroyVirt(int virtID)
{
	int ret;
	QString out;
	unsigned i;

	for (i = 0; i < virtDrvs.size(); i++) {
		if (virtDrvs[i].ID == virtID) {
			break;
		}
	}
	if (i == virtDrvs.size()) {
		return false;
	}

	/*
	 * Delete the virtual drive.
	 */
	ret = runOut("./MegaCli", QStringList() << "-CfgLDDel"
				  << "-L" + QString::number(virtID)
				  << "-a" + QString::number(ID), out);
	if (ret != 0) {
		log_error("MegaCli -CfgLDDel failed with exit code %d\n"
			  "Output:[%s]\n", ret, qPrintable(out));
		return false;
	}

	/*
	 * Remove the virtual drive from the list in the controller.
	 * Also mark the subdisks used by the virtual disk free (vdrvID = -1).
	 */
	MegaVirtDrv *vdrv = &virtDrvs[i];
	for (unsigned j = 0; j < vdrv->disks.size(); j++) {
		int pindex = vdrv->disks[j];
		physDrvs[pindex].vdrvID = -1;
	}
	virtDrvs.erase(virtDrvs.begin()+i);
	return true;
}


int MegaRAIDCtlr::getVirtDrvCnt()
{
	if (!scandone) {
		scan();
	}
	return virtDrvs.size();
}

VirtDrv *MegaRAIDCtlr::getVirtDrv(int i)
{
	if (i < (signed)virtDrvs.size()) {
		return &virtDrvs[i];
	} else {
		return NULL;
	}
}

int MegaRAIDCtlr::getPhysDrvCnt()
{
	if (!scandone) {
		scan();
	}
	return physDrvs.size();
}

PhysDrv *MegaRAIDCtlr::getPhysDrv(int i)
{
	if (i < (signed)physDrvs.size()) {
		return &physDrvs[i];
	} else {
		return NULL;
	}
}

/*
 * Mega RAID Virtual drives.
 */
string MegaVirtDrv::getName()
{
	QString name;
	name.sprintf("MegaRAID Virtual drive: ID %d, target ID %d, "
		     "device file %s, num disks %d, RAID level [%s], "
		     "todestroy:%d",
		     ID, targetID, devName.c_str(), numDisks, RAIDLevel.c_str(),
		     toDestroy);
	return name.toStdString();
}

/*
 * Mega RAID Physical drives.
 */
string MegaPhysDrv::getName()
{
	return string("Mega Physical drive");
}

string MegaPhysDrv::mapDrive()
{
	return string("/dev/sdb");
}

DiskInfo *MegaPhysDrv::getDiskInfo()
{
	return &diskInfo;
}

int
runOut(QString exe, QStringList args, QString &output)
{
	QProcess proc;
	QByteArray ba;
	QString outstr;
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

	env.insert("LD_LIBRARY_PATH", ".");
	proc.setProcessEnvironment(env);
	proc.setProcessChannelMode(QProcess::MergedChannels);
	proc.start(exe, args);
	proc.closeWriteChannel();
	while (proc.waitForReadyRead(-1)) {
		ba += proc.readAllStandardOutput();
	}
	proc.waitForFinished(-1);

	output = ba;
	return proc.exitCode();
}
