#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "connect/connect.h"
#include "temperature_sensor/temperature.h"
#include "http/http.h"
#include <unistd.h>
#include "freertos/task.h"

#define TIME_PERIOD 5000

void app_main(void)
{
	// Initialize non volatile storage (nvs)
	int error = nvs_flash_init();
	if (error != ESP_OK)
	{
		ESP_LOGE("NVS Initialisation", "Failed to initialize non volatile storage");
		return;
	}

	// Initialize wifi and connect wifi
	esp_netif_t * netif = connect_wifi();

	// Scan networks
	scan_wifi();

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


	// Loop for send each five seconds the sensor's temperature
	while (1)
	{
		// Start temperature sensor
		start_temp_sensor();

		// Initialize variables
		float temp;
		char buffer[6];
		int buffer_len = 6;

		// Read temerature
		read_temp_sensor(&temp);

		// Format float to string
		int error = snprintf(buffer, buffer_len, "%f", temp);
		if (error < 1)
		{
			ESP_LOGE("Convert", "Failed to convert float to string");
			stop_temp_sensor();
			http_cleanup(client);
			disconnect_wifi(netif);
			return;
		}

		// Send temperature to server
		http_post(client, buffer, buffer_len);

		// Stop sensor
		stop_temp_sensor();

		// Wait the time indicated by macro TIME_PERIOD
		vTaskDelayUntil(&time, freq);
	}

	// Free resources of http
	http_cleanup(client);

	// Disconnect and free resources of wifi
	disconnect_wifi(netif);
}
