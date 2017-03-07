#ifndef __LICENSE_H__
#define __LICENSE_H__

QString genSerialNr();
bool getSerialNr(QString devFile, QString *serialNr);
bool setSerialNr(QString devFile, const QString &serialNr);
QString makeLicense(QString UserName, int NrWipes);
bool decodeLicense(QString LicCode, QString UserName, int *pNrWipes);
void testBase32(void);

#define SERIALNRSZ	8
#define SERIALSIGNATURE	"ALLGREENANNIHILATORSN"
#define SERIALSIGNATURESZ (sizeof(SERIALSIGNATURE) - 1)
extern QString theSerialNr;
#endif
