/*
 * $Id$
 * $Author$
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/fcntl.h>

int file_size(char *filename)
{
	struct stat statbuf;
	stat((const char *)filename, &statbuf);
	return statbuf.st_size;
}

char *read_file(char *filename, int offset)
{
	extern int errno;
	int fp, err;
	char *buffer;

	fp = open((const char *)filename, O_RDONLY);
	if (!fp) { return (char *) NULL; }

	if (offset >= file_size(filename)) {
		return "\0";
	}

	if (offset > 0) {
		err = lseek(fp, offset, SEEK_SET);
		/* printf("seeking to %s, err = %d\n", offset, err); */
	}

	(char *)buffer = malloc(file_size(filename) + 1);

	err = read (fp, buffer, file_size(filename) - offset);

	close(fp);

	return strdup(buffer);
}
