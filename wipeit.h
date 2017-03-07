#ifndef __WIPEIT_H__
#define __WIPEIT_H__
#include <string>
#include <vector>
#include <iostream>

typedef unsigned long long u64_t;
typedef long long s64_t;
typedef unsigned int u32_t;

class DiskInfo
{
public:
	DiskInfo() {
		wipenow = false;
		bootdisk = false;
		useSG = false;
		rpm = cylinders = heads = sectors_per_track =
		number_of_sectors = physical_sector_size = cache_size = -1;
	}

	enum wipestate
	{
		UNWIPED = 0,
		PARTWIPE = 1,
		WIPED = 2,
		IOERROR = 3,
		BADDISK = 4
	};

	std::string model, vendor, serialnr, hwserialnr;
	std::string type;
	std::string enclid;
	std::string dev_file;
	u64_t dev_size;
	int sector_size;

	int rpm;
	std::string form_factor;
	int cylinders;
	int heads;
	int sectors_per_track;
	int number_of_sectors;
	int physical_sector_size;
	int cache_size;

	bool bootdisk;
	bool useSG;

	int itemid;

	enum wipestate wiped_state;
	u64_t wiped_sectnr;
	std::string wiped_user;
	u64_t wiped_start, wiped_date;
	int wiped_mode;

	bool wipenow;
	int errorno, diskfd;
};

class RAIDCtlr;
class SysInfo
{
public:
	std::string hostname, model, vendor, hwserialnr, serialnr;
	std::string processor;
	int processor_count, processor_cores, processor_threads;
	u64_t memory;
	std::vector<DiskInfo> disks;
	std::vector<RAIDCtlr *> RAIDCtlrs;
};

struct UserAction
{
	u64_t dts;
	std::string user;
	std::string event;
	DiskInfo *disk;
	std::string note;
};

SysInfo *get_system(void (*updateFn)(std::string item, void *pData), void *pData);
char *timeunits(float time_in_secs);
bool wipeit(DiskInfo *disk, int nrpasses, std::string username,
       bool (*statusfn)(void *privData, DiskInfo *, float throughput,
       			int passnr, u64_t written, u64_t sectorsdone),
	void *privData);
std::string makeReport(SysInfo *sys, bool machInfo);
void log_error(char *fmt, ...);
bool sign_exists(std::string &devfile);
bool check_disk(DiskInfo &newdisk);
#endif
