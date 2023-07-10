#include "esp_log.h"
#include "esp_http_client.h"

#define URL "http://20.103.43.247/cmp/api/v1/Sim"

char * buffer = NULL;
int buffer_len = 0;


// Function for add \n at end of buffer
void prepare_buffer_for_print(char * buffer, buffer_len)
{
	if (buffer[buffer_len - 1] != '\n')
	{
		buffer_len ++;
		buffer = realloc(buffer, buffer_len * sizeof(char));
		buffer[buffer_len - 1] = '\n';
	}
}

// Function for treat event caused by http
esp_err_t http_event(esp_http_client_event_t * event)
{
	switch(event->event_id)
	{
		case HTTP_EVENT_ERROR:
			ESP_LOGE("HTTP Error", "Error: %s", strerror(esp_http_client_get_errno(event->client)));
			break;

		case HTTP_EVENT_ON_CONNECTED:
			ESP_LOGI("HTTP Status", "Connected");
			break;

		case HTTP_EVENT_ON_DATA:
			buffer_len += event->data_len;
			buffer = realloc(buffer, sizeof(char) * buffer_len);
			for (int i = 0; i < event->data_len; i ++)
			{
				buffer[buffer_len - event->data_len + i ] = ((char * ) event->data)[i];
			}
			break;

		case HTTP_EVENT_ON_FINISH:
			ESP_LOGI("HTTP Data", "Http transfert is finished.\nThis is the data received:\n%s", prepare_buffer_for_print(buffer, buffer_len));
			free(buffer);
			buffer = NULL;
			buffer_len = 0;
			break;

		case HTTP_EVENT_DISCONNECTED:
			if (buffer_len != 0)
			{
				ESP_LOGE("HTTP Status", "Disconnected. We don't read this data:\n%s", prepare_buffer_for_print(buffer, buffer_len));
				free(buffer);
				buffer = NULL;
				buffer_len = 0;
			}
			else
			{
				ESP_LOGE("HTTP Status", "Disconnected");
			}
			break;

		default:
			break;
	}
	return ESP_OK;
}


void http_read(esp_http_client_handle_t client)
{
	ESP_ERROR_CHECK(esp_http_client_flush_response(client, NULL));
	if (esp_http_client_is_complete_data_received(client))
	{
		ESP_LOGI("HTTP Data", "Data have been received successfully.\nThis is the data received:\n%s", prepare_buffer_for_print(buffer, buffer_len));
		free(buffer);
		buffer = NULL;
		buffer_len = 0;
	}
	else
	{
		ESP_LOGE("HTTP Data", "Error: we didn't receive all data");
	}
}

void http_write(esp_http_client_handle_t client, char * buffer, int buffer_len)
{
	int length_written = esp_http_client_write(client, buffer, buffer_len);
	if (length_written == -1)
	{
		ESP_LOGE("HTTP Data", "Error when trying write some data to the server");
	}
	else if (length_written < buffer_len)
	{
		ESP_LOGE("HTTP Data", "Error: we don't send %d data but %d data to the server", buffer_len, length_written);
	}
	else
	{
		ESP_LOGI("HTTP Data", "Message has been successfully send to server");
	}
}


esp_http_client_handle_t http_init(void)
{
	// We create the configuration for http connection
	esp_http_client_config_t config = {
		.url = URL,
		.event_handler = http_event,
	};

	// We initialize the connection
	esp_http_client_handle_t client = esp_http_client_init(&config);
	if (client == NULL)
	{
		ESP_LOGE("HTTP Initialization", "Failed");
		return NULL;
	}

	// We open the connection
	ESP_ERROR_CHECK(esp_http_client_open(client, 0));

	// Fetch headers
	ESP_ERROR_CHECK(esp_http_client_fetch_headers(client));

	// Read all response of server
	http_read(client);

	return client;
}

void http_cleanup(esp_http_client_handle_t client)
{
	ESP_ERROR_CHECK(esp_http_client_close(client));
	ESP_ERROR_CHECK(esp_http_client_cleanup(client));
}

