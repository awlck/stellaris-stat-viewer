/* extract_gamestate.c: rudimentarily interpret ZIP files to get at
 *                      the gamestate file inside.
 *
 * Copyright 2019 Adrian "ArdiMaster" Welcker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "extract_gamestate.h"

#include <QtCore/QTextStream>

extern "C" {
#include "puff/puff.h"
}

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
#warning "Compiling on Big-Endian platforms is experimental and untested."

template <typename T>
inline T endianSwap(T in) {
	char *inArr = reinterpret_cast<char *>(&in);
	char resultArr[sizeof(T)];
	for (unsigned long i = 0; i < sizeof(T); i++) {
		resultArr[i] = inArr[sizeof(T)-1 - i];
	}
	return *reinterpret_cast<T *>(&resultArr);
}
#define LEtoSystem(x) endianSwap(x)
#else
#define LEtoSystem(x) (x)
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
	if (arr.size() < 39) return 6;
	const char *data = arr.data();
	quint32 fileHeader = LEtoSystem(*reinterpret_cast<const quint32 *>(&data[0]));
	if (fileHeader != 0x04034b50) return 3;
	quint16 fileCompressionMethod = LEtoSystem(*reinterpret_cast<const quint16 *>(&data[8]));
	if (fileCompressionMethod != 8) return 4;
	quint32 fileCompressedSize = LEtoSystem(*reinterpret_cast<const quint32 *>(&data[18]));
	quint32 fileUncompressedSize = LEtoSystem(*reinterpret_cast<const quint32 *>(&data[22]));
	quint16 fileNameLength = LEtoSystem(*reinterpret_cast<const quint16 *>(&data[26]));
	if (fileNameLength != 9 || qstrncmp(&data[30], "gamestate", 9) != 0) return 5;
	quint16 fileExtraLength = LEtoSystem(*reinterpret_cast<const quint16 *>(&data[28]));
	const unsigned char *compr = (const unsigned char *) &data[30+fileNameLength+fileExtraLength];
	*dest = (unsigned char *) calloc(sizeof(unsigned char), fileUncompressedSize+1);
	unsigned char *puffdest = *dest;
	*destsize = ((unsigned long) fileUncompressedSize) + 1;
	auto inputSize = (unsigned long) fileCompressedSize;
	return puff(puffdest, destsize, compr, &inputSize);
}

QString getInflateErrmsg(int result) {
	switch (result) {
		case 6:
			return QObject::tr("Input file too short.");
		case 5:
			return QObject::tr("'gamestate' file not first in save.");
		case 4:
			return QObject::tr("File is not deflate (zlib) compressed.");
		case 3:
			return QObject::tr("File has invalid header.");
		case 2:
			return QStringLiteral("puff internal error (2): Available inflate data did not terminate.");
		case 1:
			return QStringLiteral("puff internal error (1): Output space exhausted before completing inflate.");
		case -1:
			return QStringLiteral("puff internal error (-1): invalid block type (type == 3)");
		case -2:
			return QStringLiteral("puff internal error (-2): stored block length did not match one's complement");
		case -3:
			return QStringLiteral("puff internal error (-3): dynamic block code description: too many length or distance codes");
		case -4:
			return QStringLiteral("puff internal error (-4): dynamic block code description: code lengths codes incomplete");
		case -5:
			return QStringLiteral("puff internal error (-5): dynamic block code description: repeat lengths with no first length");
		case -6:
			return QStringLiteral("puff internal error (-6): dynamic block code description: repeat more than specified lengths");
		case -7:
			return QStringLiteral("puff internal error (-7): dynamic block code description: invalid literal/length code lengths");
		case -8:
			return QStringLiteral("puff internal error (-8): dynamic block code description: invalid distance code lengths");
		case -9:
			return QStringLiteral("puff internal error (-9): dynamic block code description: missing end-of-block code");
		case -10:
			return QStringLiteral("puff internal error (-10): invalid literal/length or distance code in fixed or dynamic block");
		case -11:
			return QStringLiteral("puff internal error (-11): distance is too far back in fixed or dynamic block");
		default:
			return QStringLiteral("puff internal error (%1): Unknown error while inflating file.").arg(result);
	}
}