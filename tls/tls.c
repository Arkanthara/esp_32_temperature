#include "esp_tls.h"

#define URL "https://df20e270-e352-4e0e-9d9d-7594db0f3c6e.mock.pstmn.io/tempSensor"

esp_tls_t * init_tls(void)
{
	esp_tls_t * tls = esp_tls_init(void);
	if (tls == NULL)
	{
		ESP_LOGE("TLS Initialization", "Failed");
		return NULL;
	}
	
	esp_tls_cfg_t config = {
		.alpn_protos = {"h2", NULL},
		.non_block = true,
		.crt_bundle_attach = esp_crt_bundle_attach,
	};

	int error = esp_tls_conn_http_new_async(URL, &config, tls);
	while (error == 0)
	{
		if (error == -1)
		{
			ESP_LOGE("HTTPS Status", "Connection failed");
			return NULL;
		}
		else if (error == 1)
		{
			ESP_LOGI("HTTPS Status", "Connected");
		}
	}
	return tls;
}


void write_tls(esp_tls_t * tls, const void * data, int datalen)
{
	int size = esp_tls_conn_write(tls, data, datalen);
	if (size < 0)
	{
		ESP_LOGE("HTTPS Data", "Failed to write data");
	}
	else
	{
		ESP_LOGI("HTTPS Data", "Success to write data");
	}
	if (size == ESP_TLS_ERR_SSL_WANT_READ)
	{
		ESP_LOGE("HTTPS Data", "Want to read...");
		write_tls(tls, data, datalen);
	}
	if (size == ESP_TLS_ERR_SSL_WANT_WRITE)
	{
		ESP_LOGE("HTTPS Data", "Want to write");
		write_tls(tls, data, datalen);
	}
}


void read_tls(esp_tls_t * tls, balbla);



