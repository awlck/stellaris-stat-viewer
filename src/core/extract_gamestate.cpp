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
	*destsize = ((unsigned long) fileUncompressedSize) + 1;
	auto inputSize = (unsigned long) fileCompressedSize;
	return puff(*dest, destsize, compr, &inputSize);
}