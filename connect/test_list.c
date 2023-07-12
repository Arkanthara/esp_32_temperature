#include <stdio.h>

#include "list.h"



void main(void)
{
	Head * head = list_init();

	list_add(head, "Gnyahaha", "Blurps");
	list_add(head, "PKLK", "Yooo");
	list_print(head);
}
