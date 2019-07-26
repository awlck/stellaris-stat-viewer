//
// Created by Adrian Welcker on 2019-07-26.
//

#include "extract_gamestate.h"

extern "C" {
#include "puff/puff.h"
}

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
// #warning "Compiling on Big-Endian platforms is experimental and untested."
#error "Big-Endian platforms are unsupported."
template <typename T>
inline T fromLE(T in) {
	char *inArr = reinterpret_cast<char *>(&in);
	char resultArr[sizeof(T)];
	for (unsigned long i = 0; i < sizeof(T); i++) {
		resultArr[i] = inArr[sizeof(T)-1 - i];
	}
	return *reinterpret_cast<T *>(&resultArr);
}
/* #else
template <typename T>
inline T fromLE(T in) {
	return in;
} */
#endif

/*
 * Possible return codes:
 * 2, 1, -1, -2, -3, ..., -11: see puff/puff.c
 *   0: success
 * 3: input file has invalid ZIP header
 * 4: file is not zlib (deflate) compressed.
 * 5: 'gamestate' is not first in ZIP file
 * 6: input file is too short
 */
int extractGamestate(QFile &f, unsigned char **dest, unsigned long *destsize) {
	QByteArray arr(f.readAll());
	if (arr.size() < 39) return -6;
	const char *data = arr.data();
	quint32 fileHeader = (*reinterpret_cast<const quint32 *>(&data[0]));
	if (fileHeader != 0x04034b50) return 3;
	quint16 fileCompressionMethod = (*reinterpret_cast<const quint16 *>(&data[8]));
	if (fileCompressionMethod != 8) return 4;
	quint32 fileCompressedSize = (*reinterpret_cast<const quint32 *>(&data[18]));
	quint32 fileUncompressedSize = (*reinterpret_cast<const quint32 *>(&data[22]));
	quint16 fileNameLength = (*reinterpret_cast<const quint16 *>(&data[26]));
	if (fileNameLength != 9 || qstrncmp(&data[30], "gamestate", 9) != 0) return 5;
	quint16 fileExtraLength = (*reinterpret_cast<const quint16 *>(&data[28]));
	const unsigned char *compr = (const unsigned char *) &data[30+fileNameLength+fileExtraLength];
	*dest = (unsigned char *) calloc(sizeof(unsigned char), fileUncompressedSize+1);
	*destsize = ((unsigned long) fileUncompressedSize) + 1;
	auto inputSize = (unsigned long) fileCompressedSize;
	return puff(*dest, destsize, compr, &inputSize);
}