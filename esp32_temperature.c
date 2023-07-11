#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "connect/connect.h"
#include "temperature_sensor/temperature.h"
#include "http/http.h"
#include <unistd.h>
#include "freertos/task.h"

#define TIME_PERIOD 2000

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

	// Start temperature sensor
	start_temp_sensor();
	float temp;

	// Read the value of the temperature sensor an convert it into string for sending to a server
	

	ESP_ERROR_CHECK(esp_http_client_set_header(client, "content-type", "text/plain"));

	// Initialize time
	TickType_t time = xTaskGetTickCount();

	// Frequency
	const TickType_t freq = TIME_PERIOD / portTICK_PERIOD_MS;


// Loop for send each five seconds the sensor's temperature
	while (1)
	{
		char buffer[6];
		int buffer_len = 6;
		read_temp_sensor(&temp);
		int error = snprintf(buffer, buffer_len, "%f", temp);
		if (error < 1)
		{
			ESP_LOGE("Convert", "Failed to convert float to string");
			stop_temp_sensor();
			http_cleanup(client);
			disconnect_wifi(netif);
			return;
		}
		http_post(client, buffer, buffer_len);
		// vTaskDelay((TickType_t) TIME_PERIOD/portTICK_PERIOD_MS);
		// vTaskSuspendAll();
		vTaskDelayUntil(&time, freq);
		//sleep(2);
		//xTaskResumeAll();
	}

	// Stop sensor
	stop_temp_sensor();

	// Free resources of http
	http_cleanup(client);

	// Disconnect and free resources of wifi
	disconnect_wifi(netif);
}
