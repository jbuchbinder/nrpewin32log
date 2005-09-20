/*
 *	$Id$
 *	$Author$
 *
 *	win32log
 *	Win32 Nagios/NRPE log checker
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PROGRAM "win32log"
#define VERSION "0.1"

#define OPTIONS "vf:r:"

int main (int argc, char **argv)
{
	int c, l, i, m, o, f, p;
	extern char *optarg;
	extern int optind;
	char *inputglob, *tmp, *t, *filetext;
	FILE *config;
	int VERBOSE = 0;

	char *line, *ptr, *key, *value;

	int EXIT_STATUS;
	char *EXIT_MESSAGE;

	char *CONFIGFILE;
	char *REGEX;
	int MINIMUM;

	/* Initialize and memory allocation */

	line = (char *)malloc(sizeof(char)*4096);
	key = (char *)malloc(sizeof(char *));
	value = (char *)malloc(sizeof(char *));
	ptr = (char *)malloc(sizeof(char *));

	EXIT_STATUS = 0;
	EXIT_MESSAGE = (char *)malloc(sizeof(char *));
	EXIT_MESSAGE[0] = '\0';

	inputglob = (char *)malloc(sizeof(char *));
	REGEX = (char *)malloc(sizeof(char *));
	CONFIGFILE = (char *)malloc(sizeof(char *));
	MINIMUM = 0;

	while ((c = getopt(argc, argv, OPTIONS)) != -1) {
		switch (c) {
			case 'v':
			/* verbose */
			VERBOSE = 1;
			break; /* v */

			case 'f':
			l += strlen((char *)glob_pattern(optarg, "|")) + 1;
			inputglob = (char *)calloc(l, sizeof(char *));
			inputglob[0] = '\0';
			if (strlen(inputglob) > 1) { strcat(inputglob, "|"); }
			tmp = (char *)malloc(sizeof(char *));
			tmp = (char *)glob_pattern(optarg, "|");
			strcat(inputglob, tmp);
			if (VERBOSE) { printf("inputglob = %s\n", inputglob); }
			free(tmp);
			break;

			case 'r':
			CONFIGFILE = optarg;
			break;

			case ':':
			printf("Missing argument %c\n", optopt);	
			usage();
			exit(-1);
			break;

			default:
			printf("c = %d\n", c);
			break;
		}	
	}

	/* Sanity clause ... */
	if (strlen(CONFIGFILE) < 3 || strlen(inputglob) < 3) {
		usage();
		exit(-1);
	}

	f = 0;
	for (t = strtok(inputglob, "|"); t; t = strtok(0, "|")) {
		f++; /* Increment file counter */
		p = 0; /* Reset pattern counter */
		if (VERBOSE) { printf("file = %s, offset = %s\n", t, make_offset_name(t)); }
		/* Load file */
		filetext = (char *)calloc(file_size(t)+1, sizeof(char));
		/* filetext = read_file(t, 0); */
		o = tail(t, make_offset_name(t));
		if (VERBOSE) { printf(" -- file_size = %d, offset = %d\n", file_size(t), o); }
		filetext = (char *)read_file(t, o);
		if (VERBOSE) { printf("read_file done for %s, length = %d\n", t, strlen(filetext)); }


		/*********************************************************/
		/***** PERFORM MATCHING **********************************/
		config = fopen(CONFIGFILE, "r");
		if (!config) {
			printf("Could not open '%s'\n", CONFIGFILE);
			exit(-1);
		}

		while (fgets(line, 4096, config)) {
 			if (*line == '#' || *line == ';') {
				if (VERBOSE) { printf("Skipping comment line: %s\n", line); }
				continue;
			} else if (*line == '-' && *(line+1) == '-' && *(line+2) == '-' && *(line+3) == '-') {
				/* At end of rule, process regex */
				p++; /* Increment pattern counter */
				if (VERBOSE) { printf("----- end rule\n"); }

				if (VERBOSE) { printf("Attempting to match regex '%s'\n", REGEX); }
				m = match_count(REGEX, filetext);
				if (VERBOSE) { printf("'%d' matches for '%s' in %s\n", m, REGEX, t); }
				if ((strlen(filetext)>1) && (m >= MINIMUM) && (strlen(REGEX) > 3)) {
					/* Reallocate error message length */
					tmp = (char *)malloc(sizeof(char) * 200);
					sprintf(tmp, "ERROR: Pattern %s matched in %s for %d times", REGEX, t, m);
					EXIT_MESSAGE = (char *)realloc(EXIT_MESSAGE, sizeof(char) * (strlen(EXIT_MESSAGE) + strlen(tmp) + 3));
					if (strlen(EXIT_MESSAGE) > 5) {
						strcat(EXIT_MESSAGE, "; ");
					}
					strcat(EXIT_MESSAGE, tmp);

					free(tmp);
					EXIT_STATUS = 2;
				}

				/* Reset */
				MINIMUM = 1;
				continue;
			} else if (ptr = strstr(line, ": ")) {
				memcpy(key, line, strlen(line)-strlen(ptr));
				key[strlen(line)-strlen(ptr)] = '\0';
				if (key[strlen(line)-strlen(ptr)-1] == '\r' || key[strlen(line)-strlen(ptr)-1] == '\n') {
					key[strlen(line)-strlen(ptr)-1] = '\0';
				}
				ptr += 2;
				value = strncpy(value, ptr, strlen(ptr)-1);
				value[strlen(ptr)-1] = '\0';
				if (value[strlen(ptr)-2] == '\r' || value[strlen(ptr)-2] == '\n') {
					value[strlen(ptr)-2] = '\0';
				}
				if (VERBOSE) {
					printf(" config key : '%s'\n", key);
					printf(" config val : '%s'\n", value);
				}
				if (strcmp(key, "PATTERN") == 0) {
					REGEX = strcpy(REGEX, value);
					REGEX[strlen(value)] = '\0';
				} else if (strcmp(key, "COMMENT") == 0) {
					if (VERBOSE) { printf("COMMENT = '%s'\n", value); }
				} else if (strcmp(key, "MINIMUM") == 0) {
					MINIMUM = atoi(value);
				} else {
					if (VERBOSE) { printf("Skipped line: %s", line); }
				}
			}
		}

		fclose(config);


		/*--------------------------------------------------------*/

		/* Clean up this filename */
		if (strlen(filetext) > 1) { free(filetext); }

		if (VERBOSE) { printf("-------------------------------------------------\n"); }
	}	

	if (strlen(EXIT_MESSAGE) < 5) {
		sprintf(EXIT_MESSAGE, "OK; %d files processed with %d patterns", f, p);
	}

	/* Cleanup */
	free(inputglob);

	printf("%s\n", EXIT_MESSAGE);

	return EXIT_STATUS;
}

int usage ( void ) {
	printf(
		"usage:\n"
		"\twin32log -r <file> -f <fileglob> [ -f <fileglob> ...] [-v]\n"
		"\n"
		"\t-r <config>\n"
		"\t\tConfig file with regular expressions to search for\n"
		"\t-f <fileglob>\n"
		"\t\tWildcard expression(s) containing files to search\n"
		"\t\tExample:\t -f '*.log'\n"
		"\t-v\n"
		"\t\tVerbose mode (for debugging)\n"
		"\n"
		"%s v%s (GPL) - jbuchbinder@adeptra.com\n",
		PROGRAM, VERSION
	);
	return 0;
}



