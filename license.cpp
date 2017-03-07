#define _LARGEFILE64_SOURCE
#include <QApplication>
#include <QDateTime>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "license.h"
#include "simplecrypt.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

QString
encodeNibbles(uint64_t val, int NrNibbles)
{
	QString str;

	for (int i = 0; i < NrNibbles; i++) {
		str += 'A' + (val & 0xF);
		val >>= 4;
	}

	return str;
}

QString
hyphenate(QString LicCode)
{
	QString out;

	int i = 0;
	while (i < LicCode.length()) {
		if (out.length()) {
			out += "-" + LicCode.mid(i, 4);
		} else {
			out = LicCode.mid(i, 4);
		}
		i += 4;
	}
	return out;
}

/*
 * Base32 encoding. We encode 5 bits into a symbol A-Z (excluding 'O', so 25 of them),
 * 1-7 (excluding '0', so 7 of them). Symbols 8 & 9 are special markers, they indicate that the
 * next symbol is a partial symbol. The next symbol has to be converted to 5 bits and then
 * the MSB along with the markers decide the length of the word as follows:
 * Marker MSB
 * '8'	  0	1 bit length
 * '8'	  1	2 bit length
 * '9'	  0	3 bit length
 * '9'	  1	4 bit length
 */

QString
encodeBase32(QByteArray input)
{
	int		bytesLeft, bitPos, i, left;
	unsigned char	currentByte, nextByte, fragment;
	QString		output;

	bytesLeft = input.length();
	bitPos = 0;
	i = 0;
	while (bytesLeft) {
		currentByte = input.at(i);
		assert(bitPos <= 8);
		if (bitPos == 8) {
			bytesLeft--;
			i++;
			bitPos = 0;
			continue;
		}
		if (8 - bitPos < 5) {
			if (bytesLeft > 1) {
				i++;
				bytesLeft--;
				nextByte = input.at(i);
				fragment = (currentByte >> bitPos) & 0x1F;
				left = 5 - (8 - bitPos);
				assert(left > 0);
				fragment = fragment | (nextByte & ((1 << left) - 1)) << (8 - bitPos);
				bitPos = left;
				currentByte = nextByte;
			} else {
				left = 8 - bitPos;
				assert(left && left < 5);
				fragment = currentByte >> bitPos;
				switch (left) {
					case 1:
					output += '8';
					fragment |= (0 << 4);
					break;

					case 2:
					output += '8';
					fragment |= (1 << 4);
					break;

					case 3:
					output += '9';
					fragment |= (0 << 4);
					break;

					case 4:
					output += '9';
					fragment |= (1 << 4);
					break;
				}
				bytesLeft--;
				bitPos = 0;
			}
		} else {
			fragment = (currentByte >> bitPos) & 0x1F;
			bitPos += 5;
		}
		assert(fragment < 32);
		if (fragment < 25) {
			char ch = 'A' + fragment;
			if (ch >= 'O') {
				ch++;
			}
			output += ch;
		} else {
			output += '1' + fragment - 25;
		}
	}
	
	return output;
}

QByteArray
decodeBase32(QString input)
{
	QByteArray	output;
	int		i, len, bitPos, fraglen, fragment, used, marker;
	unsigned char	currentByte;

	len = input.length();
	i = 0;
	bitPos = 0;
	currentByte = 0;
	fraglen = 5;
	marker = 0;
	while (i < len) {
		fragment = input.at(i++).toAscii();
		if (fragment >= 'A' && fragment <= 'Z') {
			if (fragment < 'O') {
				fragment = fragment - 'A';
			} else {
				fragment--;
				fragment = fragment - 'A';
			}
		} else if (fragment >= '1' && fragment <= '7') {
			fragment = fragment - '1' + 25;
		} else if (fragment == '8' || fragment == '9') {
			marker = fragment;
			continue;
		} else {
			assert(0);
		}
		if (marker) {
			if (marker == '8' && !(fragment & (1 << 4))) {
				fraglen = 1;
			} else if (marker == '8' && (fragment & (1 << 4))) {
				fraglen = 2;
			} else if (marker == '9' && !(fragment & (1 << 4))) {
				fraglen = 3;
			} else if (marker == '9' && (fragment & (1 << 4))) {
				fraglen = 4;
			}
			marker = 0;
		}


		if (8 - bitPos < fraglen) {
			currentByte = currentByte | ((fragment << bitPos) & 0xFF);
			output.append(currentByte);
			used = 8 - bitPos;
			assert(used > 0 && used < fraglen);
			currentByte = fragment >> used;
			bitPos = fraglen - used;
		} else {
			currentByte = currentByte | ((fragment & ((1 << fraglen) - 1)) << bitPos);
			bitPos += fraglen;
			if (bitPos == 8) {
				output.append(currentByte);
				bitPos = 0;
				currentByte = 0;
			}
		}
		fraglen = 5;
	}
	
	return output;
}

QString
dehyphenate(QString LicCode)
{
	QString out;

	out = LicCode.replace("-", "");
	return out;
}

void
dumpbytes(char *data, int len)
{
	for (int i = 0 ; i < len; i++) {
		printf("%02x ", (unsigned char)data[i]);
		printf("%c ", (unsigned char)data[i]);
	}
	printf("\n");
}

void
fill(char *data, int length)
{
	for (int i = 0; i < length; i++) {
		data[i] = 'A' + (random() % 26);
	}
}

void
testBase32(void)
{
	char		data[256];
	int		length;
	QByteArray	input;

	for (int i = 0; i < 1000000; i++) {
		length = random() % (sizeof(data) + 1);
		fill(data, length);
		input.clear();
		input.append(data, length);
		QString encoded = encodeBase32(input);
		QByteArray decoded = decodeBase32(encoded);
		//dumpbytes(decoded.data(), decoded.length());
		assert(decoded.length() == length);
		assert(memcmp(decoded.data(), data, decoded.length()) == 0);
	}
}

#define CRYPTKEY	0xCAFEF00D
QString
makeLicense(QString UserName, int NrWipes)
{
	SimpleCrypt	crypt;
	uint32_t	hash, int32;
	uint16_t	int16;
	QByteArray	input;
	
	hash = 0;
	for (int i = 0; i < UserName.length(); i++) {
		hash = UserName.at(i).toAscii() << 3 ^ hash << 7;
	}
	for (int i = UserName.length() - 1; i >= 0; i--) {
		hash = UserName.at(i).toAscii() << 3 ^ hash << 7;
	}
	input.append(char(hash & 0xFF));
	input.append(char((hash >> 8*1) & 0xFF));
	input.append(char((hash >> 8*2) & 0xFF));
	input.append(char((hash >> 8*3) & 0xFF));

	int16 = NrWipes;
	input.append((char *)&int16, sizeof(int16));

	time_t now = time(NULL);
	int32 = now;
	input.append((char *)&int32, sizeof(int32));

	//dumpbytes(input.data(), input.length());
	crypt.setKey(CRYPTKEY);
	QByteArray output = crypt.encryptToByteArray(input);
	
	QString lic = encodeBase32(output);
	return hyphenate(lic);
}
	
bool
decodeLicense(QString LicCode, QString UserName, int *pNrWipes)
{
	SimpleCrypt	crypt;
	int		i;
	uint32_t	hash;

	LicCode = LicCode.trimmed();
	QString lic = dehyphenate(LicCode);
	if (lic.length() != 24) {
		return false;
	}
	QByteArray input = decodeBase32(lic);

	crypt.setKey(CRYPTKEY);
	QByteArray output = crypt.decryptToByteArray(input);
	char *decoded = output.data();
	//dumpbytes(decoded, output.length());

	hash = 0;
	for (i = 0; i < UserName.length(); i++) {
		hash = UserName.at(i).toAscii() << 3 ^ hash << 7;
	}
	for (int i = UserName.length() - 1; i >= 0; i--) {
		hash = UserName.at(i).toAscii() << 3 ^ hash << 7;
	}
	if ((hash & 0xFF) != *(unsigned char *)decoded++) {
		return false;
	}
	if (((hash >> (8*1)) & 0xFF) != *(unsigned char *)decoded++) {
		return false;
	}
	if (((hash >> (8*2)) & 0xFF) != *(unsigned char *)decoded++) {
		return false;
	}
	if (((hash >> (8*3)) & 0xFF) != *(unsigned char *)decoded++) {
		return false;
	}
	
	int NrWipes = *(uint16_t *)&decoded[0];
	*pNrWipes = NrWipes;
	return true;
}

#if 0
QString
makeLicense(QString UserName, int NrWipes)
{
	uint32_t	UserHash = 0, hash;

	for (int i = 0; i < UserName.length(); i++) {
		UserHash = UserName.at(i).toAscii() ^ UserHash << 7;
	}
	UserHash %= 256;
	QString UserCode = encodeNibbles(UserHash, 2);
	
	time_t now = time(NULL);
	QString TimeCode = encodeNibbles((uint32_t)now, 32 / 4);

	QString WipeCode = encodeNibbles((uint32_t)NrWipes, 32 / 4);

	QString LicCode = UserCode;
	for (int i = 0; i < 32 / 4; i++) {
		LicCode += TimeCode.at(i);
		LicCode += WipeCode.at(i);
	}

	hash = 0;
	for (int i = 0; i < LicCode.length(); i++) {
		hash = LicCode.at(i).toAscii() ^ hash << 7;
		hash = hash * 16777619;
	}
	hash %= 1 << 24;
	QString HashCode = encodeNibbles(hash, 6);
	LicCode += HashCode;

	return hyphenate(LicCode);
}

bool
decodeLicense(QString LicCode, QString UserName, int *pNrWipes)
{
	if (!pNrWipes) {
		return false;
	}

	LicCode = dehyphenate(LicCode);
	if (LicCode.length() != 24) {
		return false;
	}

	// Check integrity hash
	uint32_t hash = 0;
	for (int i = 0; i < LicCode.length() - 6; i++) {
		hash = LicCode.at(i).toAscii() ^ hash << 7;
		hash = hash * 16777619;
	}
	hash %= 1 << 24;
	printf("Hash = %u\n", hash);
	QString HashCode = encodeNibbles(hash, 6);
	if (HashCode != LicCode.mid(LicCode.length() - 6)) {
		return false;
	}

	// Check user hash
	uint32_t	UserHash = 0;
	for (int i = 0; i < UserName.length(); i++) {
		UserHash = UserName.at(i).toAscii() ^ UserHash << 7;
	}
	UserHash %= 256;
	QString UserCode = encodeNibbles(UserHash, 2);
	if (UserCode != LicCode.left(2)) {
		return false;
	}

	uint32_t	NrWipes = 0, j = 0;
	LicCode = LicCode.mid(2);
	for (int i = 1; i < LicCode.length(); i+=2) {
		int nibble = LicCode.at(i).toAscii() - 'A';
		NrWipes |= (nibble << (j * 4));
		j++;
	}
	*pNrWipes = NrWipes;
	return true;
}
#endif

bool
getDevSize(int fd, uint64_t *pSize)
{
	struct stat	filestat;
	int		ret;

	ret = fstat(fd, &filestat);
	if (ret < 0) {
		perror("Could not do stat on device");
		return false;
	}
	if (S_ISREG(filestat.st_mode)) {
		*pSize = filestat.st_size;
		return true;
	} else if (S_ISCHR(filestat.st_mode) || S_ISBLK(filestat.st_mode)) {
		uint64_t size64 = 0;
		ret = ioctl(fd, BLKGETSIZE64, &size64);
		if (ret < 0 || !size64) {
			perror("Could not get size of device");
			return false;
		}
		*pSize = size64;
		return true;
	}

	fprintf(stderr, "Specified file is not a regular file or a device\n");
	return false;
}

bool
getSerialNr(QString devFile, QString *serialNr)
{
	int fd;

	fd = open(qPrintable(devFile), O_RDONLY);
	if (fd < 0) {
		perror("Could not open device file");
		return false;
	}
	
	uint64_t size64 = 0;
	if (!getDevSize(fd, &size64)) {
		return false;
	}

	uint64_t off = 0, left = size64;
	size_t iolen, readlen;
	static char buffer[1024 * 256];
	char signature[] = SERIALSIGNATURE, serial[SERIALNRSZ + 1];
	char *ploc;

	while (left) {
		iolen = MIN(left, sizeof(buffer));
		readlen = read(fd, buffer, iolen);
		if (readlen <= 0) {
			perror("Error in reading from device");
			return false;
		}
		ploc = (char *)memmem(buffer, readlen, signature, sizeof(signature) - 1);
		if (!ploc) {
			left -= readlen;
			off += readlen;
			continue;
		}

		ploc += sizeof(signature) - 1;
		strncpy(serial, ploc, sizeof(serial));
		serial[SERIALNRSZ] = '\0';
		break;
	}

	if (!left) {
		fprintf(stderr, "Could not find serial!\n");
		return false;
	}

	*serialNr = QString(serial);
	return true;
}

bool
setSerialNr(QString devFile, const QString &serialNr)
{
	int fd;

	fd = open(qPrintable(devFile), O_RDWR);
	if (fd < 0) {
		perror("Could not open device file");
		return false;
	}
	
	uint64_t size64 = 0;
	if (!getDevSize(fd, &size64)) {
		return false;
	}

	uint64_t off = 0, left = size64;
	size_t iolen, readlen, writelen;
	static char buffer[1024 * 256];
	char signature[] = SERIALSIGNATURE, serial[SERIALNRSZ + 1];
	char *ploc;

	while (left) {
		iolen = MIN(left, sizeof(buffer));
		readlen = read(fd, buffer, iolen);
		if (readlen <= 0) {
			perror("Error in reading from device");
			return false;
		}
		ploc = (char *)memmem(buffer, readlen, signature, sizeof(signature) - 1);
		if (!ploc) {
			left -= readlen;
			off += readlen;
			continue;
		}

		ploc += sizeof(signature) - 1;
		strncpy(ploc, qPrintable(serialNr), sizeof(serial));
		ploc[SERIALNRSZ] = '\0';
		writelen = pwrite64(fd, buffer, readlen, off);
		if (writelen != readlen) {
			perror("Error in writing to device");
			return false;
		}
		break;
	}

	if (!left) {
		fprintf(stderr, "Could not find serial!\n");
		return false;
	}

	return true;
}

QString
genSerialNr()
{
	QString serial;

	srandom((unsigned int)time(NULL));
	
	for (int i = 0; i < SERIALNRSZ; i++) {
		int nr = random() % (26 + 9);
		if (nr < 9) {
			serial += '1' + nr;
		} else {
			char ch = 'A' + nr - 9;
			if (ch == 'O') {
				ch = 'P';
			}
			serial += ch;
		}
	}
	return serial;
}
