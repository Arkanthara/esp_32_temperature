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

// Our global variables
Head * head;
Item * item;

// Our task
TaskHandle_t Task_1 = NULL;
TaskHandle_t Task_2 = NULL;

// Our global variables
bool task_2 = false;
bool quit = false;


// We must look after the void * client
void vTask_1(void * client)
{

	// Initialize wifi and connect wifi
	esp_netif_t * netif = init_wifi();

	// Scan networks
	// scan_wifi();
	// scan_wifi(NULL, false);

	// Init http connection
//	esp_http_client_handle_t client = http_init();

	// Initialize time
	// It's a variable that holds the time at which the task was last unblocked
	// The variable is automatically updated within vTaskDelayUntil().
	TickType_t time = xTaskGetTickCount();

	// Frequency
	const TickType_t freq = TIME_PERIOD / portTICK_PERIOD_MS;

	// Loop for send each five seconds the sensor's temperature
	while (1)
	{
		if (quit)
		{
			break;
		}
		else if (task_2)
		{
			vTaskResume(Task_2);
			vTaskSuspend(NULL);
		}
		else
		{
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
//				http_cleanup(client);
				disconnect_wifi(netif);
				return;
			}

			// Send temperature to server
//			http_post(client, buffer, buffer_len);
			
			// Print temp_sensor
			printf("Temperature's sensor: %s\n", buffer);

			// Stop sensor
			stop_temp_sensor();

			// Wait the time indicated by macro TIME_PERIOD
			vTaskDelayUntil(&time, freq);
		}
	}

	// Free resources of http
//	http_cleanup(client);

	// Disconnect and free resources of wifi
	disconnect_wifi(netif);

	// Destroy list of networks
	list_destroy(head);

	// Destroy the task
	vTaskDelete(NULL);
}

void vTask_2(void * parameters)
{
	while (1)
	{
		if (quit)
		{
			break;
		}
		else if (!task_2)
		{
			vTaskSuspend(NULL);
		}
		else
		{
//			vTaskSuspend(Task_1);
			connect_wifi_no_init();
			task_2 = false;
			vTaskResume(Task_1);
			vTaskSuspend(NULL);
		}
	}
	vTaskDelete(NULL);
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


//	ESP_ERROR_CHECK(esp_http_client_set_header(client, "content-type", "text/plain"));


	error = xTaskCreate(vTask_2, "CONNECT", STACK_SIZE, NULL, tskIDLE_PRIORITY, &Task_2);
	configASSERT(Task_2);
	error = xTaskCreate(vTask_1, "LOOP", STACK_SIZE, NULL, tskIDLE_PRIORITY, &Task_1);
	configASSERT(Task_1);

}
