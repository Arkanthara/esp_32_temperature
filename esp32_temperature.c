#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "connect/connect.h"
#include "temperature_sensor/temperature.h"
#include "http/http.h"

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
	read_temp_sensor(&temp);
	printf("Temperature read: %f\n", temp);
	char buffer[6];
	error = snprintf(buffer, sizeof(buffer), "%f", temp);
	if (error < 0)
	{
		ESP_LOGE("Convert", "Failed to convert float to string");
		stop_temp_sensor();
		http_cleanup(client);
		disconnect_wifi(netif);
		return;
	}
	printf("Converted value: %s\n", buffer);

	// Send the value to the server
	http_post(client, buffer, sizeof(buffer));
	esp_http_client_perform(client);

/* Loop for send each five seconds the sensor's temperature
	while (1)
	{
		char temp_sensor[6];
		vTaskDelay(5000 / portTICK_PERIOD_MS);
		read_temp_sensor(&temp);
		error = snprintf(temp_sensor, sizeof(temp_sensor), "%f", temp);
		if (error < 1)
		{
			ESP_LOGE("Convert", "Failed to convert float to string");
			stop_temp_sensor();
			http_cleanup(client);
			disconnect_wifi(netif);
			return;
		}
		http_write(client, temp_sensor, sizeof(temp_sensor));
	}
*/

	// Stop sensor
	stop_temp_sensor();

	// Free resources of http
	http_cleanup(client);

	// Disconnect and free resources of wifi
	disconnect_wifi(netif);
}
