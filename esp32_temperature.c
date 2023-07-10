#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "connect/connect.h"
#include "temperature_sensor/temperature.h"

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
	start_temp_sensor();
	float temp;
	read_temp_sensor(&temp);
	printf("Temperature read: %f\n", temp);
	stop_temp_sensor();
	disconnect_wifi(netif);
}
