/*
 * $Id$
 * $Author$
 */

#include <stdlib.h>
#include <string.h>
#include <glob.h>

char *glob_pattern ( char *wildcard, char *sep )
{
	char *gfilename;
	size_t cnt = 0;
	size_t length = 0;
	glob_t glob_results;
	char **p;

	glob(wildcard, GLOB_NOCHECK, 0, &glob_results);

	length = 0;
	for (p = glob_results.gl_pathv, cnt = glob_results.gl_pathc; cnt; p++, cnt--) {
		length += strlen(*p) + 1;
	}

	gfilename = (char *) calloc (length, sizeof(char));
	for (p = glob_results.gl_pathv, cnt = glob_results.gl_pathc; cnt; p++, cnt--) {
		strcat(gfilename, *p);
		if (cnt > 1) strcat(gfilename, sep);
	}

	globfree(&glob_results);
	return gfilename;
}
