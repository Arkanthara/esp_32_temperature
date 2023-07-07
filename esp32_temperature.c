#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "connect.h"

void app_main(void)
{
	// Initialise non volatile storage (nvs)
	int error = nvs_flash_init();
	if (error != ESP_OK)
	{
		ESP_LOGE("NVS Initialisation", "Failed to initialize non volatile storage");
		return;
	}
	connect_wifi();
}
