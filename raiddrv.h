#ifndef __RAIDDRV_H__
#define __RAIDDRV_H__
#include <iostream>
#include <wipeit.h>

class VirtDrv;
class PhysDrv;

class RAIDCtlr
{
public:
	virtual std::string getName() = 0;

	virtual int getVirtDrvCnt() = 0;
	virtual VirtDrv *getVirtDrv(int i) = 0;
	virtual int getPhysDrvCnt() = 0;
	virtual PhysDrv *getPhysDrv(int i) = 0;
	virtual bool destroyVirt(int ID) = 0;
	virtual bool preparePhys(int idx) = 0;
};

class VirtDrv
{
public:
	bool toDestroy;
	virtual std::string getName() = 0;
	virtual std::string getDevName() = 0;
	virtual int getID() = 0;
};

class PhysDrv
{
public:
	virtual std::string getName() = 0;
	virtual std::string mapDrive() = 0;
	virtual DiskInfo *getDiskInfo() = 0;
};
#endif
