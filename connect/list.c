#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"


// Function for initialize our chain list
Head * list_init(void)
{
	printf("Sizeof head: %d\n", sizeof(Head));
	Head * head = malloc(sizeof(Head));
	head->head = NULL;
	return head;
}

// Function for create new data
Data * create_data(char *ssid, char * password)
{
	// We verify that the sizes are respected
	if (strlen(ssid) > 32)
	{
		fprintf(stderr, "Error: ssid is too long because it must have 32 char and it has: %d char\n", strlen(ssid));
		return NULL;
	}
	if (strlen(password) > 64)
	{
		fprintf(stderr, "Error: password is too long because it must have 64 char and it has: %d char\n", strlen(password));
		return NULL;
	}

	// We create new data and allocate necessary space
	Data * data = malloc(sizeof(Data));

	// Allocution resources
	data->ssid = malloc(sizeof(char) * (strlen(ssid) + 1));
	data->password = malloc(sizeof(char) * (strlen(password) + 1));

	memset(data->ssid, '\0', strlen(data->ssid) * sizeof(char));
	memset(data->password, '\0', strlen(data->password) * sizeof(char));

	// Copy data in our structure
	strncpy(data->ssid, ssid, strlen(ssid));
	strncpy(data->password, password, strlen(password));

	printf("SSID_INT: %s\n", data->ssid);
	return data;

}


// Function for add data to our chain list. Data will be placed at the begining of the list
void list_add(Head * head, char * ssid, char * password)
{
	// Initialization of our data
	Data * data = create_data(ssid, password);
	if (data == NULL)
	{
		fprintf(stderr, "We don't add this to our chain list\n");
		return;
	}

	// Initialisation of our item
	Item * item = NULL;
	item = malloc(sizeof(Item));

	// We put our item at the begining of our chain list
	item->data = data;
	item->next = head->head;
	head->head = item;

}

// Function for print ssid
void list_print(Head * head)
{
//	// Control if the chained list is empty
//	if (head->head == NULL)
//	{
//		fprintf(stderr, "Error: the chained list is empty");
//		return;
//	}

	// Create a pointer for browse the chained list
	Item * item = head->head;

	// Go to the end of the list and display the ssid
	while (item != NULL)
	{
//		write(1, item->data->ssid, strlen(item->data->ssid));
		printf("SSID: %s\n", item->data->ssid);
		item = item->next;
	}
}
