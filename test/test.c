#include <stdio.h>
#include <string.h>

int main (int argv, char **argc) {
	char *filename = "e.log";

	char *buffer;

	printf("before read_file\n");
	(char *)buffer = read_file((char *)filename, (int)100);
	printf("after read file\n");

	printf("length of buffer = %d\n", strlen(buffer));
}
