#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"


void event_handler(void * event_handler_arg, esp_event_base_t event_base, int32_t event_id, void * event_data)
{
	if (event_base == WIFI_EVENT)
	{
		switch(event_id)
		{
			case WIFI_EVENT_STA_DISCONNECTED:
				ESP_LOGE("Connection Status", "Disconnected");
				break;
			case WIFI_EVENT_STA_START:
				ESP_LOGI("Wifi Status", "Wifi start");
				break;
			default:
				break;
		}
	}

}


void connect_wifi(char * ssid, char * password)
{
	// Initialize stack for tcp
	ESP_ERROR_CHECK(esp_netif_init());

	// Create stack for wifi station
	esp_netif_create_default_wifi_sta();


	// Create event loop for handler's management.
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	// Are this lines usefull for the rest of the programm ??? I try to comment and I will see if it run...
	// Create handlers for wifi
	// esp_event_handler_instance_t wifi_event;
	// esp_event_handler_instance_t got_ip;

	// Attach handlers to an action. Is it necessary to create this handlers ???
	esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL, NULL);
	esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL, NULL);

	// Initialize wifi
	wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&init));

	// Define that it's a wifi station
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	// Create a config for wifi
	wifi_config_t config = {
		.sta = {
			.ssid = ssid,
			.password = password,
		},
	};

	// Set the configuration to wifi
	ESP_ERROR_CHECK(esp_wifi_set_config(&config));

	// Start wifi
	ESP_ERROR_CHECK(esp_wifi_start());

	// Wait semaphore which indicate that we have ip address
	// xSemaphoreTake(semaphore, WAIT_TIME);

	// Destroy event loop
	ESP_ERROR_CHECK(esp_event_loop_delete_default());



}
