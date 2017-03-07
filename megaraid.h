#ifndef __MEGARAID_H__
#include "raiddrv.h"
#include <vector>

class MegaRAIDCtlr;
class MegaRAID
{
private:
	static bool scanned;
	static std::vector<MegaRAIDCtlr> Ctlrs;

public:
	static int getCtlrCnt();
	static RAIDCtlr *getCtlr(int i);
};

class MegaVirtDrv:public VirtDrv
{
	friend class MegaRAIDCtlr;
	int ID, ctlrID, targetID;
	std::string devName;
	int numDisks;
	std::vector<int> disks;
	std::string RAIDLevel;
public:
	std::string getName();
	std::string getDevName()
	{ return devName; }
	int getID()
	{ return ID; }
};

class MegaPhysDrv:public PhysDrv
{
	friend class MegaRAIDCtlr;
	int enclID, slotNr;
	int vdrvID;
	DiskInfo diskInfo;
public:
	std::string getName();
	std::string mapDrive();
	DiskInfo *getDiskInfo();
};

class MegaRAIDCtlr:public RAIDCtlr
{
	friend class MegaRAID;
	int ID, hostID;
	std::string name;
	bool scandone;
	std::vector<MegaVirtDrv> virtDrvs;
	std::vector<MegaPhysDrv> physDrvs;

	int scan();
	int getPhysDrvES(int enclID, int slotNr);
	MegaVirtDrv *getVirtDrvByID(int ID);
	bool preparePhys(int idx);

public:
	MegaRAIDCtlr(int i);
	std::string getName();

	int getVirtDrvCnt();
	VirtDrv *getVirtDrv(int i);
	int getPhysDrvCnt();
	PhysDrv *getPhysDrv(int i);
	bool destroyVirt(int ID);
};
#endif
