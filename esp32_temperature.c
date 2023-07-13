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

// Our global variables
Head * head;
Item * item;

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
	// scan_wifi();
	scan_wifi(NULL, false);

	// Init http connection
	esp_http_client_handle_t client = http_init();

	// Read the value of the temperature sensor an convert it into string for sending to a server
	

	ESP_ERROR_CHECK(esp_http_client_set_header(client, "content-type", "text/plain"));

	// Initialize time
	// It's a variable that holds the time at which the task was last unblocked
	// The variable is automatically updated within vTaskDelayUntil().
	TickType_t time = xTaskGetTickCount();

	// Frequency
	const TickType_t freq = TIME_PERIOD / portTICK_PERIOD_MS;
/*

	// Loop for send each five seconds the sensor's temperature
//	while (1)
//	{
		// Initialize variables
		float temp;
		char buffer[6];
		int buffer_len = 6;

		// Start temperature sensor
		start_temp_sensor();

		// Read temerature
		read_temp_sensor(&temp);

		// Format float to string
		error = snprintf(buffer, buffer_len, "%f", temp);
		if (error < 1)
		{
			ESP_LOGE("Convert", "Failed to convert float to string");
			stop_temp_sensor();
			http_cleanup(client);
			disconnect_wifi(netif);
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
//	}

	// Free resources of http
	http_cleanup(client);
*/
	// Disconnect and free resources of wifi
	disconnect_wifi(netif);

	// Destroy list of networks
	list_destroy(head);
}
