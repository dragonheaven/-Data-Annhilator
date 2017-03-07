#define _LARGEFILE64_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/fs.h>
#include <arpa/inet.h>
#include <linux/hdreg.h>

#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include "liblshw/hw.h"
#include "liblshw/main.h"
#include "liblshw/options.h"
#include "liblshw/osutils.h"
#include "raiddrv.h"
#include "megaraid.h"
#include "sg_lib.h"

#include "wipeit.h"

using namespace std;
#define WIPEIT_SIG "WIPEDCLEANBYGREENPULSE"
const unsigned max_nr_badsectors = 100;

string UserName = "Gautham L";

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const bool LogToConsole = true;
vector<string> ErrorLog;

void
log_error(char *fmt, ...)
{
	va_list		args;
	char msg[1024], fmsg[1024];
	static int errornr = 1;

	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	snprintf(fmsg, sizeof(fmsg), "%d:Error:%s", errornr, msg);
	errornr++;
	if (LogToConsole) {
		cerr << fmsg;
	}
	ErrorLog.push_back(string(fmsg));
	va_end(args);
}

string
repstr(const string &str)
{
	if (str.empty()) {
		return "<undefined>";
	} else {
		return str;
	}
}

	
string
getdiskname(DiskInfo *disk)
{
	return decimalkilos(disk->dev_size) + ' ' + repstr(disk->vendor) +
		" hard disk, model " + repstr(disk->model) + ',' +
		" MFG number " + repstr(disk->hwserialnr) + ',' +
		" ALL Green number " + repstr(disk->serialnr);
}

bool
sign_exists(string &devfile)
{
	int fd;
	bool status = false;
	
	fd = open(devfile.c_str(), O_RDONLY);
	if (fd < 0) {
		log_error("Could not open device file %s:%s\n",
			  devfile.c_str(), strerror(errno));
		goto out;
	}
	
	char buf[512];
	ssize_t sread;
	sread = pread(fd, buf, sizeof(buf), 0);
	if (sread != 512) {
		log_error("%s: could not read first sector\n",
			  devfile.c_str());
		goto out2;
	}
	if (strncmp(buf, WIPEIT_SIG, strlen(WIPEIT_SIG)) == 0) {
		status = true;
	}

out2:
	close(fd);
out:
	return status;
}

static char *
strtok2(char *srcarg, char *delims)
{
	static char *src = NULL, *cp, *rp;

	if (srcarg) {
		src = srcarg;
	}

	cp = src;
	while (*cp != '\0') {
		if (strchr(delims, *cp)) {
			*cp = '\0';
			rp = src;
			src = cp + 1;
			return rp;
		}
		cp++;
	}
	return src;
}

void
extractcap(char *line, map<string, string> &caps)
{
	char *label, *cp;

	label = "Logical block length=";
	cp = strstr(line, label);
	if (cp) {
		cp += strlen(label);
		cp = strtok(cp, " \n\t");
		caps["sectsize"] = string(cp);
		return;
	}

	label = "Device size:";
	cp = strstr(line, label);
	if (cp) {
		cp += strlen(label);
		cp = strtok(cp, " \n\t");
		caps["devsize"] = string(cp);
		return;
	}
}

int
writesector(int fd, DiskInfo &disk, u64_t start, u64_t len, void *buffer)
{
	int sectsize = disk.sector_size;
	ssize_t ret;

	if (sectsize == 512) {
		/*
		fprintf(stderr, "Doing kernel write to dev %s sectors %llu %llu\n",
			disk.dev_file.c_str(),
			start, len);
		*/
		ret = pwrite64(fd, buffer, len * sectsize, start * sectsize);
		if (ret > 0 && (u64_t)ret == len * sectsize) {
			return 0;
		}
		fprintf(stderr, "kernel write failed to %s\n", disk.dev_file.c_str());
	}
	if (disk.useSG) {
		struct opts_t op;
		int nrblks;

		memset(&op, 0, sizeof(op));
		op.pref_cdb_size = DEF_WS_CDB_SIZE;
		op.timeout = DEF_TIMEOUT_SECS;
		op.xfer_len = sectsize;
		while (len) {
			nrblks = MIN(len, 65535);
			op.lba = start;
			op.numblocks = nrblks;
			fprintf(stderr, "Writing SG write same to dev %s sectors %llu %u\n",
				disk.dev_file.c_str(), start, nrblks);
			ret = do_write_same(fd, &op, buffer, NULL);
			if (ret) {
				disk.useSG = false;
				fprintf(stderr, "SG write same failed to %s, bailing out\n",
					disk.dev_file.c_str());
				goto error;
			}
			len -= nrblks;
			start += nrblks;
		}

		return 0;
	}

error:
	fprintf(stderr, "Write to disk %s sectors %llu %llu failed\n",
		disk.dev_file.c_str(), start, len);
	return EIO;

	if (disk.useSG) {
		struct flags_t ofp;
		int nrblks, s;

		memset(&ofp, 0, sizeof(ofp));
		ofp.cdbsz = DEF_SCSI_CDBSZ;
		while (len) {
			nrblks = MIN(len, 1024);

			if (sectsize == 520) {
				unsigned int *pu;

				for (s = 0; s < nrblks; s++) {
					pu = (unsigned int *)((char *)buffer +
						sectsize * s + 512);
					pu[0] = 0;
					pu[1] = 0;
					*(unsigned int *)(((char *)pu)+3) = 
						htonl(start + s);
				}
			}

			ret = sg_write(fd, (unsigned char *)buffer, nrblks,
					start, sectsize, &ofp, NULL);
			if (ret) {
				return EIO;
			}

			len -= nrblks;
			start += nrblks;
		}

		return 0;
	}

	return EIO;
}
int
readsector(int fd, DiskInfo &disk, u64_t start, u64_t len, void *buffer)
{
	int sectsize = disk.sector_size;
	ssize_t ret;

	if (sectsize == 512) {
		ret = pread64(fd, buffer, len * sectsize, start * sectsize);
		if (ret > 0 && (u64_t)ret == len * sectsize) {
			return 0;
		}
	}

	if (disk.useSG) {
		struct flags_t ifp;
		int blks_read;

		memset(&ifp, 0, sizeof(ifp));
		ifp.cdbsz = DEF_SCSI_CDBSZ;
		ret = sg_read(fd, (unsigned char *)buffer, len, start,
			      sectsize, &ifp, (int *)NULL, &blks_read);
		if (ret == 0 && (u64_t)blks_read == len) {
			return 0;
		}
	}

	return EIO;
}

#define ATA_OP_IDENTIFY         0xec
#define ATA_OP_PIDENTIFY        0xa1

#define GEN_CONFIG		0   /* general configuration */
#define LCYLS			1   /* number of logical cylinders */
#define CONFIG			2   /* specific configuration */
#define LHEADS			3   /* number of logical heads */
#define TRACK_BYTES		4   /* number of bytes/track (ATA-1) */
#define SECT_BYTES		5   /* number of bytes/sector (ATA-1) */
#define LSECTS			6   /* number of logical sectors/track */
#define START_SERIAL            10  /* ASCII serial number */
#define LENGTH_SERIAL           10  /* 10 words (20 bytes or characters) */
#define BUF_TYPE		20  /* buffer type (ATA-1) */
#define BUF_SIZE		21  /* buffer size (ATA-1) */
#define RW_LONG			22  /* extra bytes in R/W LONG cmd ( < ATA-4)*/
#define START_FW_REV            23  /* ASCII firmware revision */
#define LENGTH_FW_REV		 4  /*  4 words (8 bytes or characters) */
#define START_MODEL    		27  /* ASCII model number */
#define LENGTH_MODEL    	20  /* 20 words (40 bytes or characters) */
#define SECTOR_XFER_MAX		47  /* r/w multiple: max sectors xfered */
#define DWORD_IO		48  /* can do double-word IO (ATA-1 only) */
#define CAPAB_0			49  /* capabilities */
#define CAPAB_1			50
#define PIO_MODE		51  /* max PIO mode supported (obsolete)*/
#define DMA_MODE		52  /* max Singleword DMA mode supported (obs)*/
#define WHATS_VALID		53  /* what fields are valid */
#define LCYLS_CUR		54  /* current logical cylinders */
#define LHEADS_CUR		55  /* current logical heads */
#define LSECTS_CUR		56  /* current logical sectors/track */
#define CAPACITY_LSB		57  /* current capacity in sectors */
#define CAPACITY_MSB		58
#define SECTOR_XFER_CUR		59  /* r/w multiple: current sectors xfered */
#define LBA_SECTS_LSB		60  /* LBA: total number of user */
#define LBA_SECTS_MSB		61  /*      addressable sectors */
#define SINGLE_DMA		62  /* singleword DMA modes */
#define MULTI_DMA		63  /* multiword DMA modes */
#define ADV_PIO_MODES		64  /* advanced PIO modes supported */
				    /* multiword DMA xfer cycle time: */
#define DMA_TIME_MIN		65  /*   - minimum */
#define DMA_TIME_NORM		66  /*   - manufacturer's recommended	*/
				    /* minimum PIO xfer cycle time: */
#define PIO_NO_FLOW		67  /*   - without flow control */
#define PIO_FLOW		68  /*   - with IORDY flow control */
#define PKT_REL			71  /* typical #ns from PKT cmd to bus rel */
#define SVC_NBSY		72  /* typical #ns from SERVICE cmd to !BSY */
#define CDR_MAJOR		73  /* CD ROM: major version number */
#define CDR_MINOR		74  /* CD ROM: minor version number */
#define QUEUE_DEPTH		75  /* queue depth */
#define SATA_CAP_0		76  /* Serial ATA Capabilities */
#define SATA_RESERVED_77	77  /* reserved for future Serial ATA definition */
#define SATA_SUPP_0		78  /* Serial ATA features supported */
#define SATA_EN_0		79  /* Serial ATA features enabled */
#define MAJOR			80  /* major version number */
#define MINOR			81  /* minor version number */
#define CMDS_SUPP_0		82  /* command/feature set(s) supported */
#define CMDS_SUPP_1		83
#define CMDS_SUPP_2		84
#define CMDS_EN_0		85  /* command/feature set(s) enabled */
#define CMDS_EN_1		86
#define CMDS_EN_2		87
#define ULTRA_DMA		88  /* ultra DMA modes */
				    /* time to complete security erase */
#define ERASE_TIME		89  /*   - ordinary */
#define ENH_ERASE_TIME		90  /*   - enhanced */
#define ADV_PWR			91  /* current advanced power management level
				       in low byte, 0x40 in high byte. */  
#define PSWD_CODE		92  /* master password revision code	*/
#define HWRST_RSLT		93  /* hardware reset result */
#define ACOUSTIC  		94  /* acoustic mgmt values ( >= ATA-6) */
#define LBA_LSB			100 /* LBA: maximum.  Currently only 48 */
#define LBA_MID			101 /*      bits are used, but addr 103 */
#define LBA_48_MSB		102 /*      has been reserved for LBA in */
#define LBA_64_MSB		103 /*      the future. */
#define CMDS_SUPP_3		119
#define CMDS_EN_3		120
#define RM_STAT 		127 /* removable media status notification feature set support */
#define SECU_STATUS		128 /* security status */
#define CFA_PWR_MODE		160 /* CFA power mode 1 */
#define START_MEDIA             176 /* media serial number */
#define LENGTH_MEDIA            20  /* 20 words (40 bytes or characters)*/
#define START_MANUF             196 /* media manufacturer I.D. */
#define LENGTH_MANUF            10  /* 10 words (20 bytes or characters) */
#define SCT_SUPP		206 /* SMART command transport (SCT) support */
#define TRANSPORT_MAJOR		222 /* PATA vs. SATA etc.. */
#define TRANSPORT_MINOR		223 /* minor revision number */
#define NMRR			217 /* nominal media rotation rate */
#define INTEGRITY		255 /* integrity word */

/* bit definitions within the words */
/* -------------------------------- */

/* many words are considered valid if bit 15 is 0 and bit 14 is 1 */
#define VALID			0xc000
#define VALID_VAL		0x4000
/* many words are considered invalid if they are either all-0 or all-1 */

/* word 0: gen_config */
#define NOT_ATA			0x8000	
#define NOT_ATAPI		0x4000	/* (check only if bit 15 == 1) */
#define MEDIA_REMOVABLE		0x0080
#define DRIVE_NOT_REMOVABLE	0x0040  /* bit obsoleted in ATA 6 */
#define INCOMPLETE		0x0004
#define DRQ_RESPONSE_TIME	0x0060
#define DRQ_3MS_VAL		0x0000
#define DRQ_INTR_VAL		0x0020
#define DRQ_50US_VAL		0x0040
#define PKT_SIZE_SUPPORTED	0x0003
#define PKT_SIZE_12_VAL		0x0000
#define PKT_SIZE_16_VAL		0x0001
#define EQPT_TYPE		0x1f00
#define SHIFT_EQPT		8

#define CDROM 0x0005
const char *pkt_str[] = {
	"Direct-access device",			/* word 0, bits 12-8 = 00 */
	"Sequential-access device",		/* word 0, bits 12-8 = 01 */
	"Printer",				/* word 0, bits 12-8 = 02 */
	"Processor",				/* word 0, bits 12-8 = 03 */
	"Write-once device",			/* word 0, bits 12-8 = 04 */
	"CD-ROM",				/* word 0, bits 12-8 = 05 */
	"Scanner",				/* word 0, bits 12-8 = 06 */
	"Optical memory",			/* word 0, bits 12-8 = 07 */
	"Medium changer",			/* word 0, bits 12-8 = 08 */
	"Communications device",		/* word 0, bits 12-8 = 09 */
	"ACS-IT8 device",			/* word 0, bits 12-8 = 0a */
	"ACS-IT8 device",			/* word 0, bits 12-8 = 0b */
	"Array controller",			/* word 0, bits 12-8 = 0c */
	"Enclosure services",			/* word 0, bits 12-8 = 0d */
	"Reduced block command device",		/* word 0, bits 12-8 = 0e */
	"Optical card reader/writer",		/* word 0, bits 12-8 = 0f */
	"",					/* word 0, bits 12-8 = 10 */
	"",					/* word 0, bits 12-8 = 11 */
	"",					/* word 0, bits 12-8 = 12 */
	"",					/* word 0, bits 12-8 = 13 */
	"",					/* word 0, bits 12-8 = 14 */
	"",					/* word 0, bits 12-8 = 15 */
	"",					/* word 0, bits 12-8 = 16 */
	"",					/* word 0, bits 12-8 = 17 */
	"",					/* word 0, bits 12-8 = 18 */
	"",					/* word 0, bits 12-8 = 19 */
	"",					/* word 0, bits 12-8 = 1a */
	"",					/* word 0, bits 12-8 = 1b */
	"",					/* word 0, bits 12-8 = 1c */
	"",					/* word 0, bits 12-8 = 1d */
	"",					/* word 0, bits 12-8 = 1e */
	"Unknown",				/* word 0, bits 12-8 = 1f */
};
const char *ata1_cfg_str[] = {			/* word 0 in ATA-1 mode */
	"reserved",				/* bit 0 */
	"hard sectored",			/* bit 1 */
	"soft sectored",			/* bit 2 */
	"not MFM encoded ",			/* bit 3 */
	"head switch time > 15us",		/* bit 4 */
	"spindle motor control option",		/* bit 5 */
	"fixed drive",				/* bit 6 */
	"removable drive",			/* bit 7 */
	"disk xfer rate <= 5Mbs",		/* bit 8 */
	"disk xfer rate > 5Mbs, <= 10Mbs",	/* bit 9 */
	"disk xfer rate > 5Mbs",		/* bit 10 */
	"rotational speed tol.",		/* bit 11 */
	"data strobe offset option",		/* bit 12 */
	"track offset option",			/* bit 13 */
	"format speed tolerance gap reqd",	/* bit 14 */
	"ATAPI"					/* bit 14 */
};

/* word 1: number of logical cylinders */
#define LCYLS_MAX		0x3fff /* maximum allowable value */

/* word 2: specific configureation 
 * (a) require SET FEATURES to spin-up
 * (b) require spin-up to fully reply to IDENTIFY DEVICE
 */
#define STBY_NID_VAL		0x37c8  /*     (a) and     (b) */
#define STBY_ID_VAL		0x738c	/*     (a) and not (b) */
#define PWRD_NID_VAL 		0x8c73	/* not (a) and     (b) */
#define PWRD_ID_VAL		0xc837	/* not (a) and not (b) */

/* words 47 & 59: sector_xfer_max & sector_xfer_cur */
#define SECTOR_XFER		0x00ff  /* sectors xfered on r/w multiple cmds*/
#define MULTIPLE_SETTING_VALID  0x0100  /* 1=multiple sector setting is valid */

/* word 49: capabilities 0 */
#define STD_STBY  		0x2000  /* 1=standard values supported (ATA);
					   0=vendor specific values */
#define IORDY_SUP		0x0800  /* 1=support; 0=may be supported */
#define IORDY_OFF		0x0400  /* 1=may be disabled */
#define LBA_SUP			0x0200  /* 1=Logical Block Address support */
#define DMA_SUP			0x0100  /* 1=Direct Memory Access support */
#define DMA_IL_SUP		0x8000  /* 1=interleaved DMA support (ATAPI) */
#define CMD_Q_SUP		0x4000  /* 1=command queuing support (ATAPI) */
#define OVLP_SUP		0x2000  /* 1=overlap operation support (ATAPI) */
#define SWRST_REQ		0x1000  /* 1=ATA SW reset required (ATAPI, obsolete */

/* word 50: capabilities 1 */
#define MIN_STANDBY_TIMER	0x0001  /* 1=device specific standby timer value minimum */

/* words 51 & 52: PIO & DMA cycle times */
#define MODE			0xff00  /* the mode is in the MSBs */

/* word 53: whats_valid */
#define OK_W88     		0x0004	/* the ultra_dma info is valid */
#define OK_W64_70		0x0002  /* see above for word descriptions */
#define OK_W54_58		0x0001  /* current cyl, head, sector, cap. info valid */

/*word 63,88: dma_mode, ultra_dma_mode*/
#define MODE_MAX		7	/* bit definitions force udma <=7 (when
					 * udma >=8 comes out it'll have to be
					 * defined in a new dma_mode word!) */

/* word 64: PIO transfer modes */
#define PIO_SUP			0x00ff  /* only bits 0 & 1 are used so far,  */
#define PIO_MODE_MAX		8       /* but all 8 bits are defined        */

/* word 75: queue_depth */
#define DEPTH_BITS		0x001f  /* bits used for queue depth */

/* words 80-81: version numbers */
/* 0x0000 or  0xffff means device does not report version */

/* word 81: minor version number */
#define MINOR_MAX		0x22
const char *minor_str[MINOR_MAX+2] = {			/* word 81 value: */
	"Unspecified",					/* 0x0000	*/
	"ATA-1 X3T9.2 781D prior to revision 4",	/* 0x0001	*/
	"ATA-1 published, ANSI X3.221-1994",		/* 0x0002	*/
	"ATA-1 X3T9.2 781D revision 4",			/* 0x0003	*/
	"ATA-2 published, ANSI X3.279-1996",		/* 0x0004	*/
	"ATA-2 X3T10 948D prior to revision 2k",	/* 0x0005	*/
	"ATA-3 X3T10 2008D revision 1",			/* 0x0006	*/
	"ATA-2 X3T10 948D revision 2k",			/* 0x0007	*/
	"ATA-3 X3T10 2008D revision 0",			/* 0x0008	*/
	"ATA-2 X3T10 948D revision 3",			/* 0x0009	*/
	"ATA-3 published, ANSI X3.298-199x",		/* 0x000a	*/
	"ATA-3 X3T10 2008D revision 6",			/* 0x000b	*/
	"ATA-3 X3T13 2008D revision 7 and 7a",		/* 0x000c	*/
	"ATA/ATAPI-4 X3T13 1153D revision 6",		/* 0x000d	*/
	"ATA/ATAPI-4 T13 1153D revision 13",		/* 0x000e	*/
	"ATA/ATAPI-4 X3T13 1153D revision 7",		/* 0x000f	*/
	"ATA/ATAPI-4 T13 1153D revision 18",		/* 0x0010	*/
	"ATA/ATAPI-4 T13 1153D revision 15",		/* 0x0011	*/
	"ATA/ATAPI-4 published, ANSI INCITS 317-1998",	/* 0x0012	*/
	"ATA/ATAPI-5 T13 1321D revision 3",
	"ATA/ATAPI-4 T13 1153D revision 14",		/* 0x0014	*/
	"ATA/ATAPI-5 T13 1321D revision 1",		/* 0x0015	*/
	"ATA/ATAPI-5 published, ANSI INCITS 340-2000",	/* 0x0016	*/
	"ATA/ATAPI-4 T13 1153D revision 17",		/* 0x0017	*/
	"ATA/ATAPI-6 T13 1410D revision 0",		/* 0x0018	*/
	"ATA/ATAPI-6 T13 1410D revision 3a",		/* 0x0019	*/
	"ATA/ATAPI-7 T13 1532D revision 1",		/* 0x001a	*/
	"ATA/ATAPI-6 T13 1410D revision 2",		/* 0x001b	*/
	"ATA/ATAPI-6 T13 1410D revision 1",		/* 0x001c	*/
	"ATA/ATAPI-7 published, ANSI INCITS 397-2005",	/* 0x001d	*/
	"ATA/ATAPI-7 T13 1532D revision 0",		/* 0x001e	*/
	"Reserved",					/* 0x001f	*/
	"Reserved",					/* 0x0020	*/
	"ATA/ATAPI-7 T13 1532D revision 4a",		/* 0x0021	*/
	"ATA/ATAPI-6 published, ANSI INCITS 361-2002",	/* 0x0022	*/
	"Reserved",					/* 0x0023-0xfffe*/
};
const char actual_ver[MINOR_MAX+2] = { 
			/* word 81 value: */
	0,		/* 0x0000	WARNING: 	*/
	1,		/* 0x0001	WARNING: 	*/
	1,		/* 0x0002	WARNING: 	*/
	1,		/* 0x0003	WARNING: 	*/
	2,		/* 0x0004	WARNING:   This array 		*/
	2,		/* 0x0005	WARNING:   corresponds 		*/
	3,		/* 0x0006	WARNING:   *exactly*		*/
	2,		/* 0x0007	WARNING:   to the ATA/		*/
	3,		/* 0x0008	WARNING:   ATAPI version	*/
	2,		/* 0x0009	WARNING:   listed in		*/
	3,		/* 0x000a	WARNING:   the 			*/
	3,		/* 0x000b	WARNING:   minor_str 		*/
	3,		/* 0x000c	WARNING:   array		*/
	4,		/* 0x000d	WARNING:   above.		*/
	4,		/* 0x000e	WARNING:  			*/
	4,		/* 0x000f	WARNING:   if you change 	*/
	4,		/* 0x0010	WARNING:   that one,      	*/
	4,		/* 0x0011	WARNING:   change this one	*/
	4,		/* 0x0012	WARNING:   too!!!        	*/
	5,		/* 0x0013	WARNING:	*/
	4,		/* 0x0014	WARNING:	*/
	5,		/* 0x0015	WARNING:	*/
	5,		/* 0x0016	WARNING:	*/
	4,		/* 0x0017	WARNING:	*/
	6,		/* 0x0018	WARNING:	*/
	6,		/* 0x0019	WARNING:	*/
	7,		/* 0x001a	WARNING:	*/
	6,		/* 0x001b	WARNING:	*/
	6,		/* 0x001c	WARNING:	*/
	7,		/* 0x001d	WARNING:	*/
	7,		/* 0x001e	WARNING:	*/
	0,		/* 0x001f	WARNING:	*/
	0,		/* 0x0020	WARNING:	*/
	7,		/* 0x0021	WARNING:	*/
	6,		/* 0x0022	WARNING:	*/
	0		/* 0x0023-0xfffe    		*/
};

/* words 82-84: cmds/feats supported */
#define CMDS_W82		0x77ff  /* word 82: defined command locations*/
#define CMDS_W83		0x3fff  /* word 83: defined command locations*/
#define CMDS_W84		0x27ff  /* word 84: defined command locations*/
#define SUPPORT_48_BIT		0x0400  
#define NUM_CMD_FEAT_STR	48

//static const char unknown[8] = "obsolete";
//static const char unknown[8] = "unknown";
#define unknown "unknown-"

static const char *feat_word69_str[16] = { 
	"CFast specification support",			/* word 69 bit 15 */
	"Deterministic read data after TRIM",		/* word 69 bit 14 */
	"Long physical sector diagnostics",		/* word 69 bit 13 */
	"DEVICE CONFIGURATION SET/IDENTIFY DMA commands", /* word 69 bit 12 */
	"READ BUFFER DMA command",			/* word 69 bit 11 */
	"WRITE BUFFER DMA command",			/* word 69 bit 10 */
	"SET MAX SETPASSWORD/UNLOCK DMA commands",	/* word 69 bit  9 */
	"DOWNLOAD MICROCODE DMA command",		/* word 69 bit  8 */
	"reserved 69[7]",				/* word 69 bit  7 */
	"reserved 69[6]",				/* word 69 bit  6 */
	"Deterministic read ZEROs after TRIM",		/* word 69 bit  5 */
	"reserved 69[4]",				/* word 69 bit  4 */
	"reserved 69[3]",				/* word 69 bit  3 */
	"reserved 69[2]",				/* word 69 bit  2 */
	"reserved 69[1]",				/* word 69 bit  1 */
	"reserved 69[0]",				/* word 69 bit  0 */
};

static const char *feat_word82_str[16] = { 
	"obsolete 82[15]",				/* word 82 bit 15: obsolete  */
	"NOP cmd",					/* word 82 bit 14 */
	"READ_BUFFER command",				/* word 82 bit 13 */
	"WRITE_BUFFER command",				/* word 82 bit 12 */
	"WRITE_VERIFY command",				/* word 82 bit 11: obsolete  */
	"Host Protected Area feature set",		/* word 82 bit 10 */
	"DEVICE_RESET command",				/* word 82 bit  9 */
	"SERVICE interrupt",				/* word 82 bit  8 */
	"Release interrupt",				/* word 82 bit  7 */
	"Look-ahead",					/* word 82 bit  6 */
	"Write cache",					/* word 82 bit  5 */
	"PACKET command feature set",			/* word 82 bit  4 */
	"Power Management feature set",			/* word 82 bit  3 */
	"Removable Media feature set",			/* word 82 bit  2 */
	"Security Mode feature set",			/* word 82 bit  1 */
	"SMART feature set"				/* word 82 bit  0 */
};
static const char *feat_word83_str[16] = { 
	NULL,						/* word 83 bit 15: !valid bit */
	NULL,						/* word 83 bit 14:  valid bit */
	"FLUSH_CACHE_EXT",				/* word 83 bit 13 */
	"Mandatory FLUSH_CACHE",			/* word 83 bit 12 */
	"Device Configuration Overlay feature set",	/* word 83 bit 11 */
	"48-bit Address feature set",			/* word 83 bit 10 */
	"Automatic Acoustic Management feature set",	/* word 83 bit  9 */
	"SET_MAX security extension",			/* word 83 bit  8 */
	"Address Offset Reserved Area Boot",		/* word 83 bit  7 */
	"SET_FEATURES required to spinup after power up",/* word 83 bit 6 */
	"Power-Up In Standby feature set",		/* word 83 bit  5 */
	"Removable Media Status Notification feature set",/* word 83 bit 4 */
	"Advanced Power Management feature set",	/* word 83 bit  3 */
	"CFA feature set",				/* word 83 bit  2 */
	"READ/WRITE_DMA_QUEUED",			/* word 83 bit  1 */
	"DOWNLOAD_MICROCODE"				/* word 83 bit  0 */
};
static const char *feat_word84_str[16] = { 
	NULL,						/* word 84 bit 15: !valid bit */
	NULL,						/* word 84 bit 14:  valid bit */
	"IDLE_IMMEDIATE with UNLOAD",			/* word 84 bit 13 */
	"Command Completion Time Limit (CCTL)",		/* word 84 bit 12 (ref: dt1696) */
	"Time Limited Commands (TLC) feature set",	/* word 84 bit 11 (ref: dt1696) */
	"URG for WRITE_STREAM[_DMA]_EXT",		/* word 84 bit 10 */
	"URG for READ_STREAM[_DMA]_EXT",		/* word 84 bit  9 */
	"64-bit World wide name",			/* word 84 bit  8 */
	"WRITE_DMA_QUEUED_FUA_EXT",			/* word 84 bit  7 */
	"WRITE_{DMA|MULTIPLE}_FUA_EXT",			/* word 84 bit  6 */
	"General Purpose Logging feature set",		/* word 84 bit  5 */
	"Media Card Pass-Through",			/* word 84 bit  4 */
	"Media Card Pass Through Command feature set",	/* word 84 bit  3 */
	"Media serial number",				/* word 84 bit  2 */
	"SMART self-test",				/* word 84 bit  1 */
	"SMART error logging"				/* word 84 bit  0 */
};
static const char *feat_3_str[16] = { 
	NULL,						/* word 119 bit 15: !valid bit */
	NULL,						/* word 119 bit 14:  valid bit */
	"unknown 119[13]",				/* word 119 bit 13 */
	"unknown 119[12]",				/* word 119 bit 12 */
	"unknown 119[11]",				/* word 119 bit 11 */
	"unknown 119[10]",				/* word 119 bit 10 */
	"unknown 119[9]",				/* word 119 bit  9 */
	"unknown 119[8]",				/* word 119 bit  8 */
	"unknown 119[7]",				/* word 119 bit  7 */
	"unknown 119[6]",				/* word 119 bit  6 */
	"Free-fall Control feature set",		/* word 119 bit  5 */
	"Segmented DOWNLOAD_MICROCODE",			/* word 119 bit  4 */
	"{READ,WRITE}_DMA_EXT_GPL commands",		/* word 119 bit  3 */
	"WRITE_UNCORRECTABLE_EXT command",		/* word 119 bit  2 */
	"Write-Read-Verify feature set",		/* word 119 bit  1 */
	"Disable Data Transfer After Error Detection"	/* word 119 bit  0 (ref: 2014DT)*/
};
static const char *cap_sata0_str[16] = { 
	"unknown 76[15]",				/* word 76 bit 15 */
	"unknown 76[14]",				/* word 76 bit 14 */
	"unknown 76[13]",				/* word 76 bit 13 */
	"NCQ priority information",			/* word 76 bit 12 */
	"Idle-Unload when NCQ is active",		/* word 76 bit 11 */
	"Phy event counters",				/* word 76 bit 10 */
	"Host-initiated interface power management",	/* word 76 bit  9 */
	"Native Command Queueing (NCQ)",		/* word 76 bit  8 */
	"unknown 76[7]",				/* word 76 bit  7 */
	"unknown 76[6]",				/* word 76 bit  6 */
	"unknown 76[5]",				/* word 76 bit  5 */
	"unknown 76[4]",				/* word 76 bit  4 */
	"Gen3 signaling speed (6.0Gb/s)",		/* word 76 bit  3 */
	"Gen2 signaling speed (3.0Gb/s)",		/* word 76 bit  2 */
	"Gen1 signaling speed (1.5Gb/s)",		/* word 76 bit  1 */
	"unknown 76[0]"					/* word 76 bit  0 */
};
static const char *feat_sata0_str[16] = {
	"unknown 78[15]",				/* word 78 bit 15 */
	"unknown 78[14]",				/* word 78 bit 14 */
	"unknown 78[13]",				/* word 78 bit 13 */
	"unknown 78[12]",				/* word 78 bit 12 */
	"unknown 78[11]",				/* word 78 bit 11 */
	"unknown 78[10]",				/* word 78 bit 10 */
	"unknown 78[9]",				/* word 78 bit  9 */
	"unknown 78[8]",				/* word 78 bit  8 */
	"unknown 78[7]",				/* word 78 bit  7 */
	"Software settings preservation",		/* word 78 bit  6 */
	"Asynchronous notification (eg. media change)",	/* word 78 bit  5 */
	"In-order data delivery",			/* word 78 bit  4 */
	"Device-initiated interface power management",	/* word 78 bit  3 */
	"DMA Setup Auto-Activate optimization",		/* word 78 bit  2 */
	"Non-Zero buffer offsets in DMA Setup FIS",	/* word 78 bit  1 */
	"unknown 78[0]"					/* word 78 bit  0 */
};

/* words 85-87: cmds/feats enabled */
/* use cmd_feat_str[] to display what commands and features have
 * been enabled with words 85-87 
 */
#define WWN_SUP         0x100 /* 1=support; 0=not supported */

/* words 89, 90, SECU ERASE TIME */
#define ERASE_BITS		0x00ff

/* word 92: master password revision */
/* 0x0000 or  0xffff means no support for master password revision */

/* word 93: hw reset result */
#define CBLID			0x2000  /* CBLID status */
#define RST0			0x0001  /* 1=reset to device #0 */
#define DEV_DET			0x0006  /* how device num determined */
#define JUMPER_VAL		0x0002  /* device num determined by jumper */
#define CSEL_VAL		0x0004  /* device num determined by CSEL_VAL */

/* word 127: removable media status notification feature set support */
#define RM_STAT_BITS 		0x0003
#define RM_STAT_SUP		0x0001
	
/* word 128: security */
#define SECU_ENABLED		0x0002
#define SECU_LEVEL		0x0100	/* was 0x0010 */
#define NUM_SECU_STR		6
const char *secu_str[] = {
	"supported",			/* word 128, bit 0 */
	"enabled",			/* word 128, bit 1 */
	"locked",			/* word 128, bit 2 */
	"frozen",			/* word 128, bit 3 */
	"expired: security count",	/* word 128, bit 4 */
	"supported: enhanced erase"	/* word 128, bit 5 */
};

/* word 160: CFA power mode */
#define VALID_W160		0x8000  /* 1=word valid */
#define PWR_MODE_REQ		0x2000  /* 1=CFA power level 1 is NOT supported */
#define PWR_MODE_OFF		0x1000  /* 1=CFA power level 1 commands are DISABLED */
#define MAX_AMPS		0x0fff  /* value = max current in milli-amperes (mA) */

/* word 206: SMART command transport (SCT) */
static const char *feat_sct_str[16] = {
	"unknown 206[15] (vendor specific)",		/* word 206 bit 15 */
	"unknown 206[14] (vendor specific)",		/* word 206 bit 14 */
	"unknown 206[13] (vendor specific)",		/* word 206 bit 13 */
	"unknown 206[12] (vendor specific)",		/* word 206 bit 12 */
	"unknown 206[11]",				/* word 206 bit 11 */
	"unknown 206[10]",				/* word 206 bit 10 */
	"unknown 206[9]",				/* word 206 bit  9 */
	"unknown 206[8]",				/* word 206 bit  8 */
	"unknown 206[7]",				/* word 206 bit  7 */
	"unknown 206[6]",				/* word 206 bit  6 */
	"SCT Data Tables (AC5)",			/* word 206 bit  5 */
	"SCT Features Control (AC4)",			/* word 206 bit  4 */
	"SCT Error Recovery Control (AC3)",		/* word 206 bit  3 */
	"SCT Write Same (AC2)",				/* word 206 bit  2 */
	"SCT Read/Write Long (AC1), obsolete",		/* word 206 bit  1: obsolete per T13/e08153r1 */
	"SMART Command Transport (SCT) feature set"	/* word 206 bit  0 */
};

/* word 255: integrity */
#define SIG			0x00ff  /* signature location */
#define SIG_VAL			0x00A5  /* signature value */

const char *BuffType[4]		= {"unknown", "1Sect", "DualPort", "DualPortCache"};

void
get_hw_info(DiskInfo &newdisk)
{
	int fd;
	unsigned char args[4 + 512], *cp;
	uint16_t *id, jj;
	uint32_t ll;
	uint64_t bbbig;

	fprintf(stderr, "Obtaining hw info for %s\n", newdisk.dev_file.c_str());
	fd = open(newdisk.dev_file.c_str(), O_RDONLY);
	if (fd < 0) {
		log_error("Could not open device file %s:%s\n",
			  newdisk.dev_file.c_str(), strerror(errno));
		return;
	}

	memset(args, 0, sizeof(args));
	int last_identify_op = ATA_OP_IDENTIFY;
	args[0] = last_identify_op;
	args[3] = 1;	/* sector count */
	if (ioctl(fd, HDIO_DRIVE_CMD, args)) {
		memset(args, 0, sizeof(args));
		last_identify_op = ATA_OP_PIDENTIFY;
		args[0] = last_identify_op;
		args[3] = 1;	/* sector count */
		if (ioctl(fd, HDIO_DRIVE_CMD, args)) {
			perror("HDIO_DRIVE_CMD(identify) failed");
			return;
		}
	}
	close(fd);

	/* byte-swap the little-endian IDENTIFY data
	   to match byte-order on host CPU */
	cp = (args + 4);
	id = (uint16_t *)cp;
	for (int i = 0; i < 0x100; ++i) {
		unsigned char *b = (unsigned char *)&id[i];
		id[i] = b[0] | (b[1] << 8);	/* le16_to_cpu() */
	}

	/*
	fprintf(stderr, "Model number:%.*s\n",
		LENGTH_MODEL, cp + START_MODEL);
	*/
	if(id[NMRR] == 1)
		newdisk.rpm = 1;
	else if(id[NMRR] > 0x401)
		newdisk.rpm = id[NMRR];
	fprintf(stderr, "RPM = %d\n", newdisk.rpm);

	if(id[168] && (id[168] & 0xfff8) == 0) {
		switch(id[168]) {
		case 1:
			newdisk.form_factor = "5.25 inch";
			break;
		case 2:
			newdisk.form_factor = "3.5 inch";
			break;
		case 3:
			newdisk.form_factor = "2.5 inch";
			break;
		case 4:
			newdisk.form_factor = "1.8 inch";
			break;
		case 5:
			newdisk.form_factor = "less than 1.8 inch";
			break;
		}
	}

	/* CHS */
	ll = 0;
	if (id[CAPAB_0] & LBA_SUP)
		ll = (__u32)id[LBA_SECTS_MSB] << 16 | id[LBA_SECTS_LSB];
	if ( (ll > 0x00FBFC10) && (!id[LCYLS])) {
		printf("\tCHS addressing not supported\n");
	} else {
		jj = id[WHATS_VALID] & OK_W54_58;
		printf("\tLogical\t\tmax\tcurrent\n");
		printf("\tcylinders\t%u\t%u\n",id[LCYLS],jj?id[LCYLS_CUR]:0);
		newdisk.cylinders = id[LCYLS];
		printf("\theads\t\t%u\t%u\n",id[LHEADS],jj?id[LHEADS_CUR]:0);
		newdisk.heads = id[LHEADS];
		printf("\tsectors/track\t%u\t%u\n",
			id[LSECTS],jj?id[LSECTS_CUR]:0);
		newdisk.sectors_per_track = id[LSECTS];
	}

	/* Number of sectors */
	if (id[CAPAB_0] & LBA_SUP) {
	/* LBA addressing */
		printf("\tLBA    user addressable sectors:%11u\n",ll);
		newdisk.number_of_sectors = ll;
		if( ((id[CMDS_SUPP_1] & VALID) == VALID_VAL) &&
		     (id[CMDS_SUPP_1] & SUPPORT_48_BIT) ) {
			bbbig = (__u64)id[LBA_64_MSB] << 48 | 
				(__u64)id[LBA_48_MSB] << 32 |
				(__u64)id[LBA_MID] << 16 | 
				id[LBA_LSB] ;
			printf("\tLBA48  user addressable sectors:%11llu\n",
				(unsigned long long)bbbig);
			newdisk.number_of_sectors = bbbig;
		}
	}

	/* Sector size */
	if((id[106] & 0xc000) != 0x4000) {
		int sector_bytes = 512;
		printf("\t%-31s %11u bytes\n","Logical/Physical Sector size:", sector_bytes);
		newdisk.physical_sector_size = sector_bytes;
	} else {
		unsigned int lsize = 256, pfactor = 1, sector_bytes;
		if (id[106] & (1<<13))
			pfactor = (1 << (id[106] & 0xf));
		if (id[106] & (1<<12))
			lsize = (id[118] << 16) | id[117];
		sector_bytes = 2 * lsize;
		printf("\t%-31s %11u bytes\n","Logical  Sector size:",
			sector_bytes);
		printf("\t%-31s %11u bytes\n","Physical Sector size:",
			sector_bytes * pfactor);
		newdisk.physical_sector_size = sector_bytes * pfactor;
	}

	/* device cache/buffer size */
	printf("\tcache/buffer size  = ");
	if (id[20] <= 3 && id[BUF_SIZE] && id[BUF_SIZE] != 0xffff) {
		printf("%u KBytes", id[BUF_SIZE] / 2);
		newdisk.cache_size = id[BUF_SIZE];
		if (id[20])
			printf(" (type=%s)", BuffType[id[20]]);
	} else {
		printf("unknown");
	}
}

bool
check_disk(DiskInfo &newdisk)
{
	int fd, ssz, ret;
	unsigned long size32;
	unsigned long long size64;
	DiskInfo::wipestate wiped_state;

	fd = open(newdisk.dev_file.c_str(), O_RDONLY);
	if (fd < 0) {
		log_error("Could not open device file %s:%s\n",
			  newdisk.dev_file.c_str(), strerror(errno));
		return false;
	}

	if (newdisk.useSG) {
		char cmd[512];
		sprintf(cmd, "sg_readcap %s", newdisk.dev_file.c_str());
		FILE *fp = popen(cmd, "r");
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		map<string, string> caps;
		while ((read = getline(&line, &len, fp)) != -1) {
			extractcap(line, caps);
		}
		if (line)
			free(line);
		fclose(fp);

		if (caps.find("sectsize") != caps.end()) {
			int sectsize = atoi((char *)caps["sectsize"].c_str());
			if (sectsize < 512 || sectsize > 1024*1024) {
				fprintf(stderr, "Bad sector size %d "
					       "on disk %s, trying nonSG\n",
					sectsize, newdisk.dev_file.c_str());
				goto via_kernel;
			}
			newdisk.sector_size = sectsize;
		} else {
			goto via_kernel;
		}

		if (caps.find("devsize") != caps.end()) {
			u64_t devsize = strtoull(caps["devsize"].c_str(),
						 NULL, 10);
			newdisk.dev_size = devsize;
		} else {
			goto via_kernel;
		}

		fprintf(stderr, "Got sectsize %d, devsize %llu\n",
			newdisk.sector_size, newdisk.dev_size);
		goto readsect0;
	}

via_kernel:
	ret = ioctl(fd, BLKSSZGET, &ssz);
	if (ret < 0) {
		log_error("Could not get sector size for device file %s:%s\n",
			  newdisk.dev_file.c_str(), strerror(errno));
		goto badout;
	}
	newdisk.sector_size = ssz;

	ret = ioctl(fd, BLKGETSIZE64, &size64);
	if (ret < 0) {
		ret = ioctl(fd, BLKGETSIZE, &size32);
		if (ret < 0) {
			log_error("Could not get device size for %s:%s\n",
				  newdisk.dev_file.c_str(), strerror(errno));
			goto badout;
		}
		newdisk.dev_size = size32 * newdisk.sector_size;
	} else {
		newdisk.dev_size = size64;
	}

readsect0:
	/*
	 * Check to make sure device size is a multiple of sector size.
	 */
	if (newdisk.dev_size % newdisk.sector_size != 0) {
		log_error("%s: disk size or sector size is incorrect\n",
			  newdisk.dev_file.c_str());
		goto badout;
	}

	/*
	 * Get hardware info from the disk.
	 */
	get_hw_info(newdisk);

	/*
	 * Check if disk has already been wiped.
	 */
	char *buf;
	buf = (char *)malloc(newdisk.sector_size);
	if (!buf) {
		log_error("Not enough memory\n");
		goto badout;
	}
	ret = readsector(fd, newdisk, 0, 1, buf);
	if (ret) {
		log_error("%s: could not read first sector\n",
			  newdisk.dev_file.c_str());
		goto badout2;
	}
	char *sig, *cp;
	sig = WIPEIT_SIG;
	newdisk.wiped_state = DiskInfo::UNWIPED;
	if (strncmp(buf, sig, strlen(WIPEIT_SIG)) == 0) {
		/*
		 * Has the following fields
		 * WIPEIT_SIG:<serialnr>:<wipe state>:<wipe sectnr>:
		 *		<wiping user>:<start date>:<end date>:<passes>
		 */
		cp = buf + strlen(WIPEIT_SIG) + 1;
		cp = strtok2(cp, ":");
		if (strlen(cp)) {
			/*
			 * We have a valid serialnr on disk sig read it in.
			 */
			newdisk.serialnr = string(cp);
		}

		cp = strtok2(NULL, ":");
		if (!cp) {
			goto out;
		}
		wiped_state = (DiskInfo::wipestate)atoi(cp);

		cp = strtok2(NULL, ":");
		if (!cp) {
			goto out;
		}
		newdisk.wiped_sectnr = atoll(cp);
		
		cp = strtok2(NULL, ":");
		if (!cp) {
			goto out;
		}
		newdisk.wiped_user = string(cp);

		cp = strtok2(NULL, ":");
		if (!cp) {
			goto out;
		}
		newdisk.wiped_start = atoll(cp);

		cp = strtok2(NULL, ":");
		if (!cp) {
			goto out;
		}
		newdisk.wiped_date = atoll(cp);

		cp = strtok2(NULL, ":");
		if (!cp) {
			goto out;
		}
		newdisk.wiped_mode = atoi(cp);
		newdisk.wiped_state = wiped_state;
	}

out:
	free(buf);
	close(fd);
	fprintf(stderr, "Good disk %s useSG %d size %lluMB sectsize %d\n",
	       newdisk.dev_file.c_str(),
	       newdisk.useSG,
	       newdisk.dev_size / 1048576,
	       newdisk.sector_size);
	return true;

badout2:
	free(buf);
badout:
	close(fd);
	return false;
}
	

void
get_disks(hwNode *node, hwNode *parent, SysInfo *sys)
{
	if (node->getId().compare(0, 4, "disk") == 0 &&
	    node->getClass() == hw::disk) {
		DiskInfo newdisk;

		newdisk.model = node->getProduct();
		newdisk.vendor = node->getVendor();
		newdisk.hwserialnr = node->getSerial();
		newdisk.dev_file = node->getLogicalName();
		newdisk.type = "HARDDISK";
		newdisk.wipenow = true;
		if (parent && parent->getConfig("driver") == "usb-storage") {
			newdisk.type = "USBDISK";
		}
		string bus = node->getBusInfo();
		if (bus.compare(0, 5, "scsi@") == 0 &&
		    newdisk.vendor != "ATA") {
			newdisk.useSG = true;
		}
		if (check_disk(newdisk)) {
			sys->disks.push_back(newdisk);
		}
		return;
	}

	for (unsigned int i = 0; i < node->countChildren(); i++) {
		hwNode *chNode = node->getChild(i);
		get_disks(chNode, node, sys);
	}
}

static void (*UpdateFn)(string item, void *pData);
static void *UpdateFn_pData;

void
status(const char *message)
{
	::UpdateFn(string(message), UpdateFn_pData);
}

char *
strupr(char *str)
{
	char *cp = str;

	while (*cp) {
		*cp = toupper(*cp);
		cp++;
	}

	return str;
}

u64_t
getFromMemInfo()
{
	FILE *fp;
	char *buf, *word, *units;
	size_t bufsize;
	ssize_t read;
	u64_t memsize, scaling;

	fp = fopen("/proc/meminfo", "r");
	if (!fp) {
		return 0;
	}
	buf = NULL;
	memsize = 0;
	while ((read = getline(&buf, &bufsize, fp)) != -1) {
		word = strtok(buf, ":");
		if (strcmp(word, "MemTotal") == 0) {
			word = strtok(NULL, " \n");
			units = strtok(NULL, " \n");
			memsize = atoll(word);
			strupr(units);
			if (strcmp(units, "KB") == 0) {
				scaling = 1024;
			} else if (strcmp(units, "MB") == 0) {
				scaling = 1024*1024;
			} else if (strcmp(units, "GB") == 0) {
				scaling = 1024LL*1024*1024;
			} else {
				scaling = 1;
			}
			memsize *= scaling;
			break;
		}
	}
	if (buf) {
		free(buf);
	}
	fclose(fp);
	return memsize;
}

string
get_procmodel_cpuinfo()
{
	FILE *fp;

	fp = fopen("/proc/cpuinfo", "r");
	if (!fp) {
		return string();
	}

	ssize_t read;
	char *line = NULL, *cp;
	size_t len = 0;
	int nrprocessors = 0;
	char *modelname = NULL;
	while ((read = getline(&line, &len, fp)) != -1) {
		while (line[read - 1] == '\n') {
			line[read - 1] = '\0';
			read--;
		}
		if (strncmp(line, "processor", 5) == 0) {
			nrprocessors++;
			continue;
		}
		if (modelname == NULL && strncmp(line, "model name", 10) == 0) {
			cp = strchr(line, ':');
			if (cp && cp[1]) {
				modelname = strdup(cp + 2);
			}
			continue;
		}
	}
	if (line) {
		free(line);
	}
	fclose(fp);

	string retname = modelname;
	free(modelname);
	return retname;
}

SysInfo *
get_system(void (*updateFn)(string item, void *pData), void *pData)
{
	hwNode root("computer", hw::system), *core, *memory;
	SysInfo *sys;

	disable("output:sanitize");
	::UpdateFn = updateFn;
	::UpdateFn_pData = pData;
	if (!scan_system(root)) {
		return NULL;
	}
	//cout << root.asXML() << endl; 

	sys = new SysInfo;
	sys->hostname = root.getId();
	sys->model = root.getProduct();
	sys->vendor = root.getVendor();
	sys->hwserialnr = root.getSerial();

	core = root.getChild("core");
	if (!core) {
		goto badout;
	}
	sys->processor_count = core->countChildren(hw::processor);
	if (sys->processor_count) {
		hwNode *cpu = core->getChild("cpu");
		if (!cpu) {
			cpu = core->getChild("cpu:0");
		}
		assert(cpu);
		string procmodel = get_procmodel_cpuinfo();
		if (procmodel.length() != 0) {
			sys->processor = procmodel;
		} else {
			sys->processor = cpu->getProduct();
		}
		sys->processor_cores = atoi(cpu->getConfig("cores").c_str());
		sys->processor_threads = atoi(cpu->getConfig("threads").c_str());
	} else {
		goto badout;
	}
	memory = core->getChild("memory");
	if (memory) {
		sys->memory = memory->getSize();
	} else {
		/*
		 * Get memory value from /proc/meminfo.
		 */
		sys->memory = getFromMemInfo();
	}

	/*
	 * Get local disk info
	 */
	get_disks(core, NULL, sys);
	
#if 0
	/*
	 * Get RAID info
	 */
	int ctlrcnt;
	updateFn("MegaRAID controllers", pData);
	if ((ctlrcnt = MegaRAID::getCtlrCnt()) > 0) {
		for (int i = 0; i < ctlrcnt; i++) {
			RAIDCtlr *ctlr = MegaRAID::getCtlr(i);
			sys->RAIDCtlrs.push_back(ctlr);

			/*
			 * Get virtual drives and delete them from the
			 * local system disk list if they appear in it since
			 * they are exposed to the host. We check for this
			 * with the device name.
			 */
			unsigned nrVirt = ctlr->getVirtDrvCnt();
			for (unsigned j = 0; j < nrVirt; j++) {
				VirtDrv *vd = ctlr->getVirtDrv(j);
				string devname = vd->getDevName();
				if (devname.length() == 0) {
					continue;
				}
				unsigned nrSysDisks = sys->disks.size();
				for (unsigned k = 0; k < nrSysDisks; k++) {
					if (sys->disks[k].dev_file == devname) {
						sys->disks.erase(
							sys->disks.begin() + k);
						break;
					}
				}
			}

					
		}
	}
#endif

	/*
	 * Print out.
	 */
#ifdef TEST
	cout << sys->model << endl;
	cout << sys->vendor << endl;
	cout << sys->hwserialnr << endl;
	cout << sys->processor << endl;
	cout << sys->processor_count << endl;
	cout << sys->processor_cores << endl;
	cout << sys->processor_threads  << endl;
	cout << kilobytes(sys->memory);
	for (unsigned int i = 0; i < sys->disks.size(); i++) {
		cout << sys->disks[i].model << endl;
		cout << sys->disks[i].vendor << endl;
		cout << sys->disks[i].serialnr << endl;
		cout << sys->disks[i].dev_file << endl;
		cout << decimalkilos(sys->disks[i].dev_size) << endl;
		cout << "Wiped state " << sys->disks[i].wiped_state << endl;
		if (sys->disks[i].wiped_state == DiskInfo::WIPED) {
			cout << sys->disks[i].wiped_user << endl;
			cout << sys->disks[i].wiped_date << endl;
			cout << sys->disks[i].wiped_mode << endl;
		}
	}
#endif
	return sys;

badout:
	delete sys;
	return NULL;
}

unsigned long
code32(int nr)
{
	unsigned long code;
	int count;

	nr &= 0xF;
	code = 0;
	count = 0;
	while (count < 8) {
		code = (code << 4 | nr);
		count++;
	}
	return code;
}

char *
timeunits(float time_in_secs)
{
	static char buf[128];
	char *prefixes[] = { "ns", "us", "ms", "secs", "mins",
			     "hrs", "days", "years"};
	float scale[] = { 1000, 1000, 1000, 60, 60, 24, 365.2422, 0};
	float time;
	int i;

	i = 0;
	time = time_in_secs * 1E9;
	for (;;) {
		if (!scale[i] || time < scale[i]) {
			break;
		}
		time /= scale[i];
		i++;
	}

	sprintf(buf, "%3.*f %s", i < 4 ? 0 : 1, time, prefixes[i]);
	return buf;
}

#define MEMALIGN(p, sz) (((unsigned long)(p) + sz - 1) & ~(sz - 1))

bool
iofails(u64_t start, u64_t end, DiskInfo *disk)
{
	static char buf[512*1024 + 512], *bufp;
	ssize_t wrsize, ret;

	bufp = buf;
	bufp = (char *)MEMALIGN(bufp, 512);
	fsync(disk->diskfd);
	ret = ioctl(disk->diskfd, BLKFLSBUF, 0);
	if (ret != 0) {
		perror("iofails:ioctl");
	}
	while (start <= end) {
		wrsize = (end - start + 1) * disk->sector_size;
		wrsize = MIN((size_t)wrsize, sizeof(buf));
		ret = pwrite64(disk->diskfd, bufp, wrsize,
			       start * disk->sector_size);
		if (ret != wrsize) {
			return true;
		}
		start += wrsize / disk->sector_size;
	}

	return false;
}

unsigned countbadsectors(u64_t start, u64_t end, DiskInfo *disk)
{
	unsigned badsectors;
	u64_t mid;

	//log_error("countbadsectors(%llu, %llu)\n", start, end);
	if (start > end) {
		return 0;
	} else if (start == end) {
		return 1;
	}

	badsectors = 0;
	mid = (start + end) / 2;
	if (start <= mid) {
		if (iofails(start, mid, disk)) {
			//log_error("IO failed for %llu, %llu\n", start, mid);
			badsectors += countbadsectors(start, mid, disk);
		}
	}
	if (mid + 1 <= end) {
		if (iofails(mid + 1, end, disk)) {
			//log_error("IO failed for %llu, %llu\n", mid + 1, end);
			badsectors += countbadsectors(mid + 1, end, disk);
		}
	}
	return badsectors;
}

bool
wipeit(DiskInfo *disk, int nrpasses, string username,
       bool (*statusfn)(void *privData, DiskInfo *, float throughput,
       			int passnr, u64_t written, u64_t sectorsdone),
	void *privData)
{
	char *zeros_alloc, *zeros_buf;
	char *ones_alloc, *ones_buf;
	char *modulo_alloc, *modulo_buf;
	int passnr, nrwr, fd;
	unsigned sectsize = disk->sector_size;
	u64_t nrsects, sectnr, totalwritten, startwritten;
	unsigned nrbadsects;
	char *buf;
	ssize_t written;
	const u64_t quanta = 1024*1024*1LL;
	struct timeval starttime, endtime;
	bool seen_ioerrors, canceled = false;
	u64_t first_badsector;
	int first_nrsects, first_errorno;
	const int writesize = sectsize * 1024*32, memalignsz = 512;

	if (nrpasses == 0) {
		return false;
	}

	disk->errorno = 0;
	zeros_alloc = (char *)malloc(writesize + memalignsz);
	if (!zeros_alloc) {
		log_error("Not enough memory\n");
		goto badout;
	}
	zeros_buf = (char *)MEMALIGN(zeros_alloc, memalignsz);
	memset(zeros_buf, 0, writesize);

	ones_alloc = (char *)malloc(writesize + memalignsz);
	if (!ones_alloc) {
		log_error("Not enough memory\n");
		goto badout2;
	}
	ones_buf = (char *)MEMALIGN(ones_alloc, memalignsz);
	memset(ones_buf, 255, writesize);

	modulo_alloc = (char *)malloc(writesize + memalignsz);
	if (!modulo_alloc) {
		log_error("Not enough memory\n");
		goto badout3;
	}
	modulo_buf = (char *)MEMALIGN(modulo_alloc, memalignsz);
	/*
	log_error("%p %p\t%p %p\t%p %p\n", zeros_alloc, zeros_buf,
		  ones_alloc, ones_buf,
		  modulo_alloc, modulo_buf);
	*/

	nrsects = writesize / sectsize;
	sectnr = 0;
	while (nrsects) {
		u32_t code = code32(sectnr);
		u32_t *sptr = (u32_t *) (modulo_buf + sectnr * sectsize);
		for (unsigned i = 0; i < sectsize / 4; i++) {
			sptr[i] = code;
		}
		time_t now;
		time(&now);
		sprintf((char *)sptr, "Wiped clean by Data Annihilator "
				      "from ALLGREEN on %s.\n", ctime(&now));
		sectnr++;
		nrsects--;
	}

#ifdef TEST
	assert(disk->dev_file == "/dev/sda5");
#endif
	fd = open(disk->dev_file.c_str(), O_RDWR|O_LARGEFILE|O_DIRECT);
	if (fd < 0) {
		log_error("Could not open device file %s:%s\n",
			  disk->dev_file.c_str(), strerror(errno));
		goto badout4;
	}
	disk->diskfd = fd;

	struct timeval tv_start;
	gettimeofday(&tv_start, NULL);

	passnr = 0;
	totalwritten = 0;
	gettimeofday(&starttime, NULL);
	startwritten = 0;
	seen_ioerrors = false;
	nrbadsects = 0;
	while (passnr < nrpasses) {
		nrsects = disk->dev_size / sectsize;
		sectnr = 0;
		if (passnr % 2 == 0) {
			buf = zeros_buf;
		} else {
			buf = ones_buf;
		}
		if (passnr == nrpasses - 1) {
			buf = modulo_buf;
		}
		while (nrsects) {
			nrwr = MIN(nrsects, writesize / sectsize);
			written = writesector(fd, *disk, sectnr, nrwr, buf);
			if (written) {
				disk->errorno = errno ? errno : EIO;
				if (!seen_ioerrors) {
					seen_ioerrors = true;
					first_badsector = sectnr;
					first_nrsects = nrwr;
					first_errorno = errno;
				}

				unsigned badsectors;
				if (disk->useSG) {
					badsectors = 128;
				} else {
					badsectors = countbadsectors(sectnr,
							     sectnr + nrwr - 1,
							     disk);
				}
				nrbadsects += badsectors;
				if (nrbadsects > max_nr_badsectors) {
					goto writesig;
				}
				written = badsectors * sectsize;
			} else {
				written = nrwr * sectsize;
			}

			startwritten += written;
			totalwritten += written;
			sectnr += nrwr;
			nrsects -= nrwr;
			if (!nrsects || startwritten >= quanta) {
				float timetaken, throughput;
				gettimeofday(&endtime, NULL);
				timetaken = endtime.tv_sec - starttime.tv_sec;
				timetaken *= 1E6;
				timetaken += endtime.tv_usec -starttime.tv_usec;
				throughput = totalwritten / timetaken * 1E6;
				if (!statusfn(privData, disk, throughput,
						passnr, startwritten,
						sectnr)) {
					canceled = true;
					goto cancel;
				}
				startwritten = 0;
			}
		}
		passnr++;
	}

writesig:
	if (seen_ioerrors) {
		if (nrbadsects > max_nr_badsectors) {
			disk->wiped_state = DiskInfo::BADDISK;
			log_error("%s:Disk found DEFECTIVE! during wiping. "
				  "Wiping NOT COMPLETED!",
				  getdiskname(disk).c_str());
		} else {
			disk->wiped_state = DiskInfo::IOERROR;
			log_error("%s:IO error during WIPING: "
				  "first IO Error at sector %llu, "
				  "length %u, error message [%s]. "
				  "Total bad sectors %u.",
				  getdiskname(disk).c_str(),
				  first_badsector, first_nrsects,
				  strerror(first_errorno),
				  nrbadsects);
		}
	} else {
		disk->wiped_state = DiskInfo::WIPED;
	}
	/*
	 * Write out wiped signature.
	 * Note that the serial nr is updated both when we succeed in wiping
	 * and when we hit an IOERROR. It is not updated only when the wiping
	 * is cancelled.
	 */
	struct timeval tv;
	gettimeofday(&tv, NULL);
	sprintf(zeros_buf, "%s:%s:%d:%d:%s:%llu:%llu:%d",
		WIPEIT_SIG, disk->serialnr.c_str(), disk->wiped_state, 0,
		username.c_str(), (u64_t)tv_start.tv_sec, (u64_t)tv.tv_sec,
		nrpasses);
	written = writesector(fd, *disk, 0, 1, zeros_buf);
	if (written) {
		disk->wiped_state = DiskInfo::BADDISK;
		log_error("%s:Disk found DEFECTIVE while writing "
			  "signature:%s. Please DESTROY the hard disk",
			  getdiskname(disk).c_str(), strerror(errno));
	}
	disk->wiped_user = username;
	disk->wiped_start = tv_start.tv_sec;
	disk->wiped_date = tv.tv_sec;
	disk->wiped_mode = nrpasses;

cancel:
	fdatasync(fd);
	close(fd);
	disk->diskfd = -1;
	free(modulo_alloc);
	free(ones_alloc);
	free(zeros_alloc);
	return !canceled;

badout4:
	free(modulo_alloc);
badout3:
	free(ones_alloc);
badout2:
	free(zeros_alloc);
badout:
	return false;
}
	
string
toDateStr(u64_t time)
{
	struct tm timecomp;
	char buf[512];
	time_t giventime = (time_t)time;

	localtime_r(&giventime, &timecomp);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H.%M.%S %z", &timecomp);
	return string(buf);
}

string
makeReport(SysInfo *sys, bool machInfo)
{
	stringstream report;
	unsigned int i;

	if (!machInfo) {
		goto disks;
	}
	report << "====================" << endl;
	report << " System information" << endl;
	report << "====================" << endl;
	report << "Hostname:" << sys->hostname << endl;
	report << "Vendor:" << sys->vendor << endl;
	report << "Model:" << sys->model << endl;
	report << "Hardware SN:" << sys->hwserialnr << endl;
	report << "All Green SN:" << sys->serialnr << endl;
	report << "Processor type:" << sys->processor << endl;
	report << "Processor count:" << sys->processor_count << endl;
	report << "Processor core:" << sys->processor_cores << endl;
	report << "Processor threads:" << sys->processor_threads << endl;
	report << "System RAM:" << kilobytes(sys->memory) << endl;
disks:
	report << "====================" << endl;
	report << " Disk information" << endl;
	report << "====================" << endl;
	for (i = 0; i < sys->disks.size(); i++) {
		DiskInfo *disk = &sys->disks[i];

		report << "Disk " << i + 1 << ":" << endl;
		report << '\t' << "Size:" << decimalkilos(disk->dev_size)
		       << endl;
		report << '\t' << "Vendor:" << disk->vendor << endl
		       << '\t' << "Model:" << disk->model << endl
		       << '\t' << "Hardware SN:" << disk->hwserialnr << endl
		       << '\t' << "All Green SN:" << disk->serialnr << endl;
		if (disk->wiped_state == DiskInfo::UNWIPED) {
			report << '\t' << "Wipe state:Ready For Wipe"
				<< endl;
		} else if (disk->wiped_state == DiskInfo::WIPED) {
			report << '\t' << "Wipe state:Data Annihilated" << endl;
			report << '\t' << "Wipe user:" << disk->wiped_user
				<< endl;
			report << '\t' << "Wipe date:"
				<< toDateStr(disk->wiped_date) << endl;
			report << '\t' << "Wipe passes:"
				<< disk->wiped_mode << endl;
		} else if (disk->wiped_state == DiskInfo::IOERROR) {
			report << '\t' << "Wipe state:IO ERROR during wipe!"
				<< endl;
			report << '\t' << "Wipe user:" << disk->wiped_user
				<< endl;
			report << '\t' << "Wipe date:"
				<< toDateStr(disk->wiped_date) << endl;
			report << '\t' << "Wipe passes:"
				<< disk->wiped_mode << endl;
		} else if (disk->wiped_state == DiskInfo::BADDISK) {
			report << '\t' << "Wipe state:Disk found defective "
					  "during wipe!"
				<< endl;
			report << '\t' << "Wipe user:" << disk->wiped_user
				<< endl;
			report << '\t' << "Wipe date:"
				<< toDateStr(disk->wiped_date) << endl;
			report << '\t' << "Wipe passes:"
				<< disk->wiped_mode << endl;
		} else {
			assert(0);
		}
	}

	return report.str();
}



#if 0
bool
updater(void *privData, DiskInfo *disk, float throughput, int passnr,
	u64_t datawritten)
{
	cout << disk->dev_file << ' '
		<< kilobytes(throughput) << ' '
		<< datawritten << endl;
	return true;
}
int
main(int ac, char *av[])
{
	get_system(&sys_info);

	DiskInfo newdisk;

	newdisk.dev_file = "/dev/sda5";
	if (check_disk(newdisk)) {
		cout << "Device file: " << newdisk.dev_file << endl;
		cout << "Size: " << decimalkilos(newdisk.dev_size) << endl;
		cout << "Sector size: " << kilobytes(newdisk.sector_size) << endl;
		cout << "Wiped ? " << newdisk.wiped << endl;
		if (newdisk.wiped) {
			cout << newdisk.wiped_user << endl;
			cout << newdisk.wiped_date << endl;
			cout << newdisk.wiped_mode << endl;
		}

		if (wipeit(&newdisk, 1, updater)) {
			cout << "Wiping finished!" << endl;
		} else {
			cout << "Wiping failed!" << endl;
		}
	}
	return 0;
}
#endif

#if 0
int
get_system(SysInfo *sys)
{
	FILE *fp;

	fp = fopen("/proc/cpuinfo", "r");
	if (!fp) {
		log_error("Could not open /proc/cpuinfo:%s\n",
			  (const char *)strerror(errno));
		return 1;
	}

	ssize_t read;
	char *line = NULL, *cp;
	size_t len = 0;
	int nrprocessors = 0;
	char *modelname = NULL;
	while ((read = getline(&line, &len, fp)) != -1) {
		while (line[read - 1] == '\n') {
			line[read - 1] = '\0';
			read--;
		}
		printf("Retrieved line [%s]\n", line);
		if (strncmp(line, "processor", 5) == 0) {
			nrprocessors++;
			continue;
		}
		if (strncmp(line, "model name", 10) == 0) {
			cp = strchr(line, ':');
			if (cp && cp[1] && !modelname) {
				modelname = strdup(cp + 2);
			}
			continue;
		}
	}
	if (line) {
		free(line);
	}
	fclose(fp);

	printf("Nr of processors = %d, model = %s\n", nrprocessors, modelname);
	if (modelname) {
		sys->processor = new string(modelname);
		free(modelname);
	}
	sys->processor_count = nrprocessors;

	fp = fopen("/proc/meminfo", "r");
	if (!fp) {
		log_error("Could not open /proc/meminfo:%s\n",
			  (const char *)strerror(errno));
		return 1;
	}
}
	cout << timeunits(1) << ' ';
	cout << timeunits(1E-6) << ' ';
	cout << timeunits(1E-9) ;
	cout << ' ' << timeunits(1E-10);
	cout << ' ' << timeunits(1E-11);
	cout << ' ' << timeunits(100);
	cout << ' ' << timeunits(60*60);
	cout << ' ' << timeunits(60*60*24);
	cout << ' ' << timeunits(60*60*24*367) << endl;
#endif
