#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "esp_log.h"


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

	data->ssid_len = strlen(ssid);
	data->password_len = strlen(password);

	// Allocution resources
	data->ssid = malloc(sizeof(char) * data->ssid_len);
	data->password = malloc(sizeof(char) * data->password_len);

	// Copy data in our structure
	strncpy(data->ssid, ssid, data->ssid_len);
	strncpy(data->password, password, data->password_len);

	// Add \0 at the end of chain
	data->ssid = realloc(data->ssid, (data->ssid_len + 1) * sizeof(char));
	data->ssid[data->ssid_len] = '\0';
	data->password = realloc(data->password, (data->password_len + 1) * sizeof(char));
	data->password[data->password_len] = '\0';

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
	Item * item = malloc(sizeof(Item));

	// We put our item at the begining of our chain list
	item->data = data;
	item->next = head->head;
	head->head = item;

}

// Function for print ssid
void list_print(Head * head)
{
	// Create a pointer for browse the chained list
	Item * item = head->head;

	// Go to the end of the list and display the ssid
	while (item != NULL)
	{
		write(1, "SSID: ", 6);
		write(1, item->data->ssid, item->data->ssid_len);
		write(1, "\n", 2);
		item = item->next;
	}
}

// Function for print item
void item_print(Item * item)
{
	// We test if item isn't null
	if (item == NULL)
	{
		ESP_LOGE("Item", "Item is null");
		return;
	}
	write(1, "SSID: ", 6);
	write(1, item->data->ssid, item->data->ssid_len);
	write(1, "\n", 2);
}

// Function that return password for an ssid given
Item * list_find(Head * head, char * ssid)
{
	// Create a pointer for browse our element
	Item * item = head->head;

	// Search for our element by browsing our list
	while (item != NULL)
	{
		if (strncmp(item->data->ssid, ssid, item->data->ssid_len) == 0)
		{
			return item;
		}
		item = item->next;
	}

	printf("Error");

	fprintf(stderr, "We don't find the item of ssid: %s\n", ssid);

	// Return NULL if we don't find our element
	return NULL;
}



















