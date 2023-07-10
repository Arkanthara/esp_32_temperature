#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "freertos/semphr.h"

// #define WIFI_SSID "Livebox-4130"
// #define WIFI_PASSWORD "LrKkE5HeSixXowpGgb"
#define WIFI_SSID "WIFI_Mobile"
#define WIFI_PASSWORD "428fdcf3d44d5e92a54d1ca5579d21416be03291895184d724abf652f24a"
#define NUMBER_OF_TRY_FOR_RECONNECTION 5
#define WAIT_TIME_FOR_CONNECTION 10000
int current_try_for_reconnection = 0;

SemaphoreHandle_t semaphore = NULL;

void event_handler(void * event_handler_arg, esp_event_base_t event_base, int32_t event_id, void * event_data)
{
	if (event_base == WIFI_EVENT)
	{
		switch(event_id)
		{
			case WIFI_EVENT_STA_DISCONNECTED:
				ESP_LOGE("Connection Status", "Disconnected");
				if (current_try_for_reconnection < NUMBER_OF_TRY_FOR_RECONNECTION)
				{
					int error = esp_wifi_connect();
					if (error == ESP_ERR_WIFI_NOT_STARTED)
					{
						ESP_LOGE("Wifi Status", "Wifi stop");
					}
					current_try_for_reconnection ++;
				}
				else
				{
					ESP_LOGE("Connection Status", "Connection failed");
				}
				break;
			case WIFI_EVENT_STA_START:
				ESP_LOGI("Wifi Status", "Wifi start");
				ESP_ERROR_CHECK(esp_wifi_connect());
				break;
			case WIFI_EVENT_STA_CONNECTED:
				ESP_LOGI("Wifi Status", "Connected");
				current_try_for_reconnection = 0;
				break;
			default:
				break;
		}
	}
	else
	{
		ESP_LOGI("IP", "We have an ip address");
		if (xSemaphoreGive(semaphore) != pdTRUE)
		{
			ESP_LOGE("Semaphore", "We can't give semaphore up");
		}
	}

}


esp_netif_t * connect_wifi(void)
{
	// Create binary semaphore
	semaphore = xSemaphoreCreateBinary();
	if (semaphore == NULL)
	{
		ESP_LOGE("Semaphore", "Creation of semaphore failed");
		return NULL;
	}

	// Initialize stack for tcp
	ESP_ERROR_CHECK(esp_netif_init());


	// Create event loop for handler's management.
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	// Create stack for wifi station... It initialize netif and register event handlers for default interfaces...
	esp_netif_t * netif = esp_netif_create_default_wifi_sta();

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
			.ssid = WIFI_SSID,
			.password = WIFI_PASSWORD,
		},
	};

	// Set the configuration to wifi
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));

	// Start wifi
	ESP_ERROR_CHECK(esp_wifi_start());

	// Wait semaphore which indicate that we have ip address (add TickType_t cast ???)
	if (xSemaphoreTake(semaphore, WAIT_TIME_FOR_CONNECTION / portTICK_PERIOD_MS) != pdTRUE)
	{
		ESP_LOGE("Semaphore", "Semaphore don't giving up");
		return netif;
	}

	return netif;


}

void disconnect_wifi(esp_netif_t * netif)
{
	ESP_ERROR_CHECK(esp_wifi_stop());
	esp_netif_destroy_default_wifi(netif);
	ESP_ERROR_CHECK(esp_wifi_deinit());
	ESP_ERROR_CHECK(esp_event_loop_delete_default());
}