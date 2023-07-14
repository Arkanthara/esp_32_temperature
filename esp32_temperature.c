#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "connect/global.h"
#include "connect/list.h"
#include "connect/connect.h"
#include "temperature_sensor/temperature.h"
#include "http/http.h"
#include <unistd.h>
#include "freertos/task.h"

#define TIME_PERIOD 5000
#define STACK_SIZE 4096
// #define configSTACK_DEPTH_TYPE int

// Our global variables
Head * head;
Item * item;

TaskHandle_t Task_1 = NULL;
TaskHandle_t Task_2 = NULL;

// Function for create our task
void vTask_1(void * parameters)
{

	// Initialize time
	// It's a variable that holds the time at which the task was last unblocked
	// The variable is automatically updated within vTaskDelayUntil().
	TickType_t time = xTaskGetTickCount();

	// Frequency
	const TickType_t freq = TIME_PERIOD / portTICK_PERIOD_MS;


	// Loop for send each five seconds the sensor's temperature
	while (1)
	{
	// Use minimal space for this task
	UBaseType_t size = uxTaskGetStackHighWaterMark(NULL);

	// Print this minimal space
	ESP_LOGI("Task Stack", "Size: %d", size);

		// Initialize variables
		float temp;
		char buffer[6];
		int buffer_len = 6;

		// Start temperature sensor
		start_temp_sensor();

		// Read temerature
		read_temp_sensor(&temp);

		// Format float to string
		int error = snprintf(buffer, buffer_len, "%f", temp);
		if (error < 1)
		{
			ESP_LOGE("Convert", "Failed to convert float to string");
			stop_temp_sensor();
//			http_cleanup(client);
//			disconnect_wifi(netif);
			return;
		}

		// Send temperature to server
//		http_post(client, buffer, buffer_len);
		
		// Print temp_sensor
		printf("Temperature's sensor: %s\n", buffer);

		// Stop sensor
		stop_temp_sensor();

		// Wait the time indicated by macro TIME_PERIOD
		vTaskDelayUntil(&time, freq);
	}
	// Free resources of http
//	http_cleanup(client);

}


void vTask_2(void * parameters)
{
	while (1)
	{
		connect_wifi_no_init();
	}
}



void app_main(void)
{
	// Initialize non volatile storage (nvs)
	int error = nvs_flash_init();
	if (error != ESP_OK)
	{
		ESP_LOGE("NVS Initialisation", "Failed to initialize non volatile storage");
		return;
	}

	// Initialize our list of password and ssid
	head = list_init();

	// Initialize our current item
	item = head->head;

	// Add our network
	list_add(head, "Livebox-4130", "LrKkE5HeSixXowpGgb", 1);
	list_add(head, "Test", "JbAeJdA!",5);
	list_add(head, "WIFI_Mobile", "428fdcf3d44d5e92a54d1ca5579d21416be03291895184d724abf652f24a",4);
	list_add(head, "Nolan", "JbNdIlY!", 6);
	list_print(head);

	// Initialize wifi and connect wifi
	esp_netif_t * netif = init_wifi();

	// Scan networks
	scan_wifi(NULL, false);

	// Init http connection
	esp_http_client_handle_t client = http_init();

	// Indicate to server that we send text data type
	ESP_ERROR_CHECK(esp_http_client_set_header(client, "content-type", "text/plain"));

/*	// Change stack size for optimize space... Not working and no idea...
	// Use minimal space for this task
	UBaseType_t size = uxTaskGetStackHighWaterMark(xHandle);

	// Print this minimal space
	ESP_LOGI("Task Stack", "Size: %d", size);

	// Change stack size of the task
	error = xTaskCreate(vTaskCode, "LOOP", size, NULL, tskIDLE_PRIORITY, xHandle);
	if (error != pdPASS)
	{
		ESP_LOGE("Task Creation", "Error when trying to create a task: %s", esp_err_to_name(error));
		return;
	}
*/
	// Create task
	error = xTaskCreate(vTask_2, "CONNECT", STACK_SIZE, NULL, tskIDLE_PRIORITY, &Task_2);
	if (error != pdPASS)
	{
		ESP_LOGE("Task Creation", "Error when trying to create a task. %s", esp_err_to_name(error));
		return;
	}

	error = xTaskCreate(vTask_1, "LOOP", STACK_SIZE, NULL, tskIDLE_PRIORITY, &Task_1);
	if (error != pdPASS)
	{
		ESP_LOGE("Task Creation", "Error when trying to create a task: %s", esp_err_to_name(error));
		return;
	}

	// vTaskSuspend(Task_2);

	// Delete our task
//	vTaskDelete(xHandle);

	// Destroy list of networks
//	list_destroy(head);

	// Disconnect and free resources of wifi
//	disconnect_wifi(netif);

}
