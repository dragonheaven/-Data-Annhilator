#include <QApplication>
#include <QtSql>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include "../license.h"

using namespace std;

// From libmtools.a
extern "C" int set_label(char *device, char *label);

void
usage()
{
	printf("Usage\n");
	printf("lictool generate <serial nr> <nr of wipes>\n");
	printf("lictool decode <serialnr> <license code>\n");
	printf("lictool setserial <device file> <default nr of wipes> [<label>]\n");
	printf("lictool getserial <device file>\n");
}

bool
addWipesToDb(QString LicCode, int NrWipes, QString DBName)
{
    do {
	QSqlDatabase db;
	db = QSqlDatabase::addDatabase("QSQLITE", "WIPEDB");
	db.setDatabaseName(DBName);
	if (!db.open()) {
		fprintf(stderr, "Could not open credits database");
		return false;
	}
	QSqlQuery qy(db);

	// Clear all rows from credits table
	qy.prepare("delete from credits");
	qy.exec();
	if (!qy.isActive()) {
		fprintf(stderr, "Could not clear credits database");
		db.close();
		return false;
	}

	qy.prepare("insert into credits(DTS, LicCode, WipeDelta) values(:dts, :lic, :delta)");
	qy.bindValue(":dts", QDateTime::currentDateTime());
	qy.bindValue(":lic", LicCode);
	qy.bindValue(":delta", NrWipes);
	qy.exec();
	if (!qy.isActive()) {
		fprintf(stderr,"SQL error:%s\n", qPrintable(qy.lastError().text()));
		db.close();
		return false;
	}

	db.close();
    } while(0);

	QSqlDatabase::removeDatabase("WIPEDB");
	return true;
}

void
updateSysLinux(const char *mntpath)
{
	FILE *fp;
	char filename[128];

	sprintf(filename, "%s/syslinux.cfg", mntpath);
	if (access(filename, R_OK) != 0) {
		return;
	}

	fp = fopen(filename, "w");
	if (!fp) {
		fprintf(stderr, "Could not truncate and open %s to update SysLinux boot loader\n",
			filename);
		return;
	}

	const char *content =
		"DEFAULT tc\n"
		"PROMPT 0\n"
		"TIMEOUT 0\n"
		"ONTIMEOUT tc\n"
		"\n"
		"LABEL tc\n"
		"MENU LABEL Load Data Annihilator\n"
		"TEXT HELP\n"
		"Load up Data annihilator\n"
		"Data annihilator is a tool meant for secure data destruction.\n"
		"It wipes hard disks clean off any data!\n"
		"ENDTEXT\n"
		"KERNEL /boot/vmlinuz\n"
		"INITRD /boot/core.gz\n"
		"APPEND loglevel=3 cde\n";
	fputs(content, fp);
	fclose(fp);
}

int
main(int ac, char *av[])
{
	int DecodedNrWipes;
	bool status;

	if (ac < 3) {
		usage();
		exit(1);
	}

	if (strcmp(av[1], "generate") == 0) {
		if (ac != 4) {
			usage();
			exit(1);
		}
		int GivenNrWipes = atoi(av[3]);
		QString lic = makeLicense(av[2], GivenNrWipes);
		printf("License: %s\n", qPrintable(lic));
		status = decodeLicense(lic, av[2], &DecodedNrWipes);
		if (status) {
			printf("Successfully decodes, nr of wipes: %d\n", DecodedNrWipes);
		} else {
			printf("Failed to decode\n");
		}
	} else if (strcmp(av[1], "decode") == 0) {
		if (ac != 4) {
			usage();
			exit(1);
		}
		status = decodeLicense(av[3], av[2], &DecodedNrWipes);
		if (status) {
			printf("Successfully decodes, nr of wipes: %d\n", DecodedNrWipes);
		} else {
			printf("Failed to decode\n");
		}
	} else if (strcmp(av[1], "setserial") == 0) {
		if (ac < 4) {
			usage();
			exit(1);
		}
        char *device_file = av[2];
		QString serial = genSerialNr();
		printf("Serial nr = %s\n", qPrintable(serial));
		if (!setSerialNr(av[2], serial)) {
			fprintf(stderr, "Failed to set serial on %s\n", av[2]);
			exit(1);
		}
		const char *mntpt = "/mnt1";
		char cmd[256];
		int status, defWipes = atoi(av[3]);
		sprintf(cmd, "mkdir -p %s; mount %s %s", mntpt, av[2], mntpt);
		status = system(cmd);
		if (WEXITSTATUS(status) != 0) {
			fprintf(stderr, "mount failed. Check if /mnt1 is already mounted on.\n"
					"if it is then umount it and make /mnt1 free");
			exit(2);
		}
		if (!addWipesToDb(QString(), defWipes, (mntpt + string("/WipedData.db")).c_str())) {
			fprintf(stderr, "Could not add default wipes to database\n");
			sprintf(cmd, "umount %s", mntpt); system(cmd);
			exit(1);
		}
		updateSysLinux(mntpt);
		sprintf(cmd, "umount %s", mntpt); system(cmd);

        // Set the volume label on the disk
        char label[64];
        if (ac > 4) {
            strcpy(label, av[4]);
        } else {
            strcpy(label, "Annihilator");
        }
        if (set_label(device_file, label)) {
            fprintf(stderr, "Could not set label on disk %s\n", device_file);
            exit(1);
        }

		printf("Successfully set serial and default wipes of %d on %s\n", defWipes, av[2]);
	} else if (strcmp(av[1], "getserial") == 0) {
		if (ac != 3) {
			usage();
			exit(1);
		}
		
		QString serial;
		
		if (!getSerialNr(av[2], &serial)) {
			fprintf(stderr, "Failed to get serial on %s\n", av[2]);
			exit(1);
		}
		printf("Serial nr = %s\n", qPrintable(serial));
	} else if (strcmp(av[1], "test") == 0) {
		if (strcmp(av[2], "base32") == 0) {
			testBase32();
		}
	} else {
		usage();
		exit(1);
	}

	return 0;
}
