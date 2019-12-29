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

#include <QtCore/QCoreApplication>
#include <string.h>

#include "frontends.h"

int main(int argc, char *argv[]) {
	QCoreApplication::setApplicationName("Stellaris Stat Viewer");
	QCoreApplication::setOrganizationName("ArdiMaster");
	QCoreApplication::setOrganizationDomain("diepixelecke.de");

	Frontend frontend = (Frontend) 0;  // default to the first (available) entry in the frontends enum

	if (frontend == FRONTEND_NONE) {  // if NONE is the first enum entry, no frontends have been compiled
		fprintf(stderr, "Error: SSV has been built with no frontends enabled.\n");
		return 1;
	}

	if (argc >= 2) {
		if (strncmp(argv[1], "--frontend=", 11) == 0) {
			char *frontendstr = &argv[1][11];
			if (strcmp(frontendstr, "widgets") == 0) {
				#ifdef SSV_BUILD_WIDGETS
				frontend = FRONTEND_WIDGETS;
				#else
				fprintf(stderr, "Error: SSV has not been built with the selected frontend '%s'.\n", frontendstr);
				return 1;
				#endif
			} else if (strcmp(frontendstr, "json") == 0) {
				#ifdef SSV_BUILD_JSON
				frontend = FRONTEND_JSON;
				#else
				fprintf(stderr, "Error: SSV has not been built with the selected frontend '%s'.\n", frontendstr);
				return 1;
				#endif
			} else {
				fprintf(stderr, "Unknown frontend: '%s'.\nThe following frontends are recognized:\n\n"
					"\twidgets\n\tjson\n\n(Note: not all of the above may be available in the specific "
					"build you are running.)\n", frontendstr);
				return 1;
			}
		}
	}

	switch (frontend) {
		#ifdef SSV_BUILD_WIDGETS
		case FRONTEND_WIDGETS:
			return frontend_widgets_begin(argc, argv);
		#endif
		#ifdef SSV_BUILD_JSON
		case FRONTEND_JSON:
			return frontend_json_begin(argc, argv);
		#endif
		default:
			Q_UNREACHABLE();
	}
}
