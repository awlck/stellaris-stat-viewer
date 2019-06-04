/* main.cpp: Entry point for stellaris_stat_viewer.
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

#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#ifdef Q_OS_WIN
#include <windows.h>
int __stdcall WinMain(int argc, char *argv[])
#else
#include <dlfcn.h>
#include <stdlib.h>
int main(int argc, char *argv[])
#endif
{
	QCoreApplication::setApplicationName("Stellaris Stat Viewer");
	QCoreApplication::setOrganizationName("ArdiMaster");
	QCoreApplication::setOrganizationDomain("diepixelecke.de");

	char *frontendstr;
	if (argc == 1) {
		frontendstr = (char *) calloc(sizeof(char), 9);
		strcpy(frontendstr, "widgets");
	} else if (argc == 2) {
		if (strncmp(argv[1], "--frontend=", 11) == 0) {
			frontendstr = &argv[1][11];
		} else {
			frontendstr = (char *) calloc(sizeof(char), 9);
			strcpy(frontendstr, "widgets");
		}
	} else if (argc == 3) {
		if (strncmp(argv[1], "--frontend=", 11) == 0) {
			frontendstr = &argv[1][11];
		} else {
			fprintf(stderr, "Usage: %s [--frontend=FRONTEND] [FILE]\n", argv[0]);
			return 1;
		}
	} else {
		fprintf(stderr, "Usage: %s [--frontend=FRONTEND] [FILE]\n", argv[0]);
		return 1;
	}

	#ifdef Q_OS_WIN
	typedef int (__cdecl *frontend_ptr_t)(int, char **);
	QString felib = "ssv_frontend_";
	felib += frontendstr;
	HMODULE frontend = LoadLibraryA(felib.toLocal8Bit().data());
	auto fb = (frontend_ptr_t) GetProcAddress(frontend, "frontend_begin");
	#else
	typedef int (*frontend_ptr_t)(int, char **);
	QString felib = "libssv_frontend_";
	felib += frontendstr;
	#ifdef Q_OS_MAC
	felib += ".dylib";
	#else
	felib += ".so";
	#endif
	void *frontend = dlopen(felib.toLocal8Bit().data(), RTLD_NOW);
	if (!frontend) {
		fprintf(stderr, "[ERROR] Error loading frontend %s: %s\n", frontendstr, dlerror());
		return 1;
	}
	auto fb = (frontend_ptr_t) dlsym(frontend, "frontend_begin");
	#endif
	return fb(argc, argv);
}