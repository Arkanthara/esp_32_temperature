#include <stdio.h>

#include "list.h"

#include <string.h>

void test(void)
{

	char * salut = "coucou";
	printf("size_salut: %d\n", strlen(salut));
	char * yo = "salut";
	strncpy(salut, yo, strlen(yo));
	printf("Result = %s\n", salut);
}

void main(void)
{
	Head * head = list_init();

	list_add(head, "Gnyahaha", "Blurps");
	list_add(head, "PKLK", "Yooo");
	list_print(head);
	char * result = list_find(head, "Gnyahaha");
	printf("%s\n", result);

}
