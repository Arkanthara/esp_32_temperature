#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"

#define URL2 "http://20.103.43.247/cmp/api/v1/Sim"
#define URL "https://df20e270-e352-4e0e-9d9d-7594db0f3c6e.mock.pstmn.io/tempSensor"

#define USERNAME "midonnet@ems-ch.com"
#define PASSWORD "JbVeEdlCh32dV1!"

char * buffer = NULL;
int buffer_len = 0;


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
			ESP_LOGI("HTTP Data", "Http transfert is finished.\nThis is the data received:");
			write(1, buffer, buffer_len);
			free(buffer);
			buffer = NULL;
			buffer_len = 0;
			break;

		case HTTP_EVENT_DISCONNECTED:
			if (buffer_len != 0)
			{
				ESP_LOGE("HTTP Status", "Disconnected. We don't read this data:");
				write(1, buffer, buffer_len);
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

// Function for read server response, and print then
void http_read(esp_http_client_handle_t client)
{
	ESP_ERROR_CHECK(esp_http_client_flush_response(client, NULL));
	if (esp_http_client_is_complete_data_received(client))
	{
		ESP_LOGI("HTTP Data", "Data have been received successfully.\nThis is the data received:");
		write(1, buffer, buffer_len);
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

void http_post(esp_http_client_handle_t client, char * data, int data_len)
{
	ESP_ERROR_CHECK(esp_http_client_set_post_field(client, data, data_len));
}


esp_http_client_handle_t http_init(void)
{
	// We create the configuration for http connection
	esp_http_client_config_t config = {
		.url = URL,
		.event_handler = http_event,
		.transport_type = HTTP_TRANSPORT_OVER_SSL,
		.crt_bundle_attach = esp_crt_bundle_attach,
		.username = USERNAME,
		.password = PASSWORD,
	};

	// We attach an enable use of a bundle for certificate verification
	//ESP_ERROR_CHECK(esp_crt_bundle_attach(&config));

	// We initialize the connection
	esp_http_client_handle_t client = esp_http_client_init(&config);
	if (client == NULL)
	{
		ESP_LOGE("HTTP Initialization", "Failed");
		return NULL;
	}

	// Set method to POST
	ESP_ERROR_CHECK(esp_http_client_set_method(client, HTTP_METHOD_POST));

	// We open the connection
	ESP_ERROR_CHECK(esp_http_client_open(client, 0));

	// Fetch headers
	int error = esp_http_client_fetch_headers(client);
	if (error == -1)
	{
		ESP_LOGE("HTTP Headers", "Can't fetch headers");
		return client;
	}

	// Read all response of server
	http_read(client);



	return client;
}

void http_cleanup(esp_http_client_handle_t client)
{
	ESP_ERROR_CHECK(esp_http_client_close(client));
	ESP_ERROR_CHECK(esp_http_client_cleanup(client));
}


