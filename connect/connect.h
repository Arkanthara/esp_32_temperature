#include "esp_netif.h"

esp_netif_t * connect_wifi(void);
void disconnect_wifi(esp_netif_t * netif);
void scan_wifi(void);
