#include <stdio.h>
#include <string.h>

int main() {
	char pattern[] = "*.c";
	char pattern2[] = "e.log";

	char *test;

	test = glob_pattern(pattern, "|");
	strcat(test,"|");
	strcat(test,glob_pattern(pattern2, "|"));
	printf("%s\n", test);
/*
	printf("%s\n", glob_pattern(pattern, "|"));
	printf("%s\n", glob_pattern(pattern2, "|"));
*/
}
