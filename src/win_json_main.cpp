#include <QtCore/QCoreApplication>

#include "frontends.h"

int main(int argc, char** argv) {
	QCoreApplication::setApplicationName("Stellaris Stat Viewer");
	QCoreApplication::setOrganizationName("ArdiMaster");
	QCoreApplication::setOrganizationDomain("diepixelecke.de");

	if (argc == 2 && strncmp(argv[1], "--frontend=", 11) != 0) {
		// called as "ssv_json.exe <file>"
		int new_argc = 3;
		char* new_argv[] = { argv[0], 0, argv[1] };
		return frontend_json_begin(new_argc, new_argv);
	}

	if (argc >= 2) {
		if (strncmp(argv[1], "--frontend=", 11) == 0) {
			char* frontendstr = &argv[1][11];
			if (strcmp(frontendstr, "json") != 0) {
				fprintf(stderr, "Error: %s is a special executable and must be used with `--frontend=json'.\n", argv[0]);
				return 1;
			}
		} else {
			fprintf(stderr, "USAGE: %s [--frontend=json] <file>\n", argv[0]);
			return 1;
		}
		return frontend_json_begin(argc, argv);
	}
	fprintf(stderr, "USAGE: %s [--frontend=json] <file>\n", argv[0]);
	return 1;
}