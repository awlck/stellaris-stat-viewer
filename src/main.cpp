/* main.cpp: Entry point for stellaris_stat_viewer. Kicks off execution
 *           by loading the frontend specified on the command line.
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

#define _CRT_SECURE_NO_WARNINGS

#include <QtCore/QDir>
#include <QtCore/QFileInfo>


#ifdef Q_OS_WIN
#include <windows.h>
#include <strsafe.h>

// From https://docs.microsoft.com/en-us/windows/desktop/debug/retrieving-the-last-error-code
void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}
#else
#include <dlfcn.h>
#include <stdlib.h>
#endif

int main(int argc, char *argv[]) {
	QCoreApplication::setApplicationName("Stellaris Stat Viewer");
	QCoreApplication::setOrganizationName("ArdiMaster");
	QCoreApplication::setOrganizationDomain("diepixelecke.de");

	char *frontendstr;
	if (argc == 1) {
		frontendstr = (char *) calloc(sizeof(char), 9);
		strcpy(frontendstr, "widgets");
	} else if (argc >= 2) {
		if (strncmp(argv[1], "--frontend=", 11) == 0) {
			frontendstr = &argv[1][11];
		} else {
			frontendstr = (char *) calloc(sizeof(char), 9);
			strcpy(frontendstr, "widgets");
		}
	}

	#ifdef Q_OS_WIN
	typedef int (__cdecl *frontend_ptr_t)(int, char **);
	QString felib = "ssv_frontend_";
	felib += frontendstr;
	HMODULE frontend = LoadLibraryA(felib.toLocal8Bit().data());
	if (frontend == NULL) {
		QString errstr("Loading frontend ");
		errstr += frontendstr;
		errstr += " (LoadModuleA)";
		ErrorExit(errstr.toLocal8Bit().data());
	}
	auto fb = (frontend_ptr_t) GetProcAddress(frontend, "frontend_begin");
	if (fb == NULL) {
		QString errstr("Loading frontend ");
		errstr += frontendstr;
		errstr += " (GetProcAddress)";
		ErrorExit(errstr.toLocal8Bit().data());
	}
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
