#include "../highc/vector.h"
#include <stdio.h>
#include <string.h>


int ivec[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10
};

#define LEN (sizeof(ivec) / sizeof(ivec[0]))

char *svec[] = {
	"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"
};


int vadd1(unsigned index, int v)
{
	return v + 1;
}

#define vadd3(index, v) ((v) + 3)

int veq5(int index, char *a)
{
	return !strcmp(a, "5");
}

int test_vector()
{
	int errors = 0;
	
	vector_apply(ivec, LEN, vadd1);	
	vector_apply(ivec, LEN, vadd3);
	if(!vector_contains(5, ivec, LEN)) {
		printf("5 not found in vector\n");
		errors++;
	}
	if(vector_contains(4, ivec, LEN)) {
		printf("4 was in vector\n");
		errors++;
	}
	char *tmparr[sizeof(svec) / sizeof(svec[0])] = {0};
	int count = 0;
	vector_filter(svec, tmparr, LEN, count, veq5);
	if(count != 1) {
		printf("vector filter failed\n");
		errors++;
	}
	printf("using vector print\n");
	vector_print(tmparr, LEN, "%s", "\t", 1);
	for_vector(txt, index, tmparr, LEN) {
		if(*txt) {
			printf("%s\n", *txt);
		}
	}
	return errors;
}
