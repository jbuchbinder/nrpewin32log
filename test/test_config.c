
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main ( int argc, char *argv[] ) {
	FILE *fp;
	char *line, *ptr, *key, *value;

	line = (char *)malloc(sizeof(char)*4096);
	key = (char *)malloc(sizeof(char *));
	value = (char *)malloc(sizeof(char *));
	ptr = (char *)malloc(sizeof(char *));

	fp = fopen("test.conf", "r");
	if (!fp) {
		printf("Could not open file\n");
		exit(1);
	}

	while (fgets(line, 4096, fp)) {
		if (*line == '#') {
			printf("Skipping comment line: %s\n", line);
			continue;
		} else if (*line == '-' && *(line+1) == '-' && *(line+2) == '-' && *(line+3) == '-') {
			printf("RECORD SEPARATOR\n");
			printf("----------------------------------------------\n");
			continue;
		} else if (ptr = strstr(line, ": ")) {
			memcpy(key, line, strlen(line)-strlen(ptr));
			ptr += 2;
			printf(" key : '%s'\n", key);
			value = strncpy(value, ptr, strlen(ptr)-1);
			value[strlen(ptr)-1] = '\0';
			/*
			memcpy(value, (char *)ptr+2, strlen(ptr)-4);
			printf(" line : '%s'\n", value);
			key[0]='\0';
			value[0]='\0';
			*/
			if (strcmp(key, "PATTERN") == 0) {
				printf("PATTERN = '%s'\n", value);
			} else if (strcmp(key, "COMMENT") == 0) {
				printf("COMMENT = '%s'\n", value);
			} else if (strcmp(key, "MINIMUM") == 0) {
				printf("MINIMUM = '%s'\n", value);
			} else {
				printf("'%s'\n", line);
			}
			
		}
	}

	fclose(fp);

	return 0;
}
