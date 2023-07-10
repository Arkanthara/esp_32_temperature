#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "connect/connect.h"
#include "temperature_sensor/temperature.h"
#include "http/http.h"

void app_main(void)
{
	// Initialise non volatile storage (nvs)
	int error = nvs_flash_init();
	if (error != ESP_OK)
	{
		ESP_LOGE("NVS Initialisation", "Failed to initialize non volatile storage");
		return;
	}
	esp_netif_t * netif = connect_wifi();
	esp_http_client_handle_t client = http_init();
	start_temp_sensor();
	float temp;
	read_temp_sensor(&temp);
	printf("Temperature read: %f\n", temp);
	stop_temp_sensor();
	http_cleanup(client);
	disconnect_wifi(netif);
}
