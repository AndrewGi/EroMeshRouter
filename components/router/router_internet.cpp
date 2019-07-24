#include "router_internet.hpp"
#include "esp_eth.h"
#include "esp_log.h"
#include "esp_event.h"
#include "tcpip_adapter.h"

static const char* TAG = "ero_eth";

static bool _connected = false;

using namespace ero::router;
static void eth_event_handler(void *arg, esp_event_base_t event_base,
							  int32_t event_id, void *event_data)
{
	uint8_t mac_addr[6] = {0};
	/* we can get the ethernet driver handle from event data */
	esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

	switch (event_id) {
		case ETHERNET_EVENT_CONNECTED:
			esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
			ESP_LOGI(TAG, "Ethernet Link Up");
			ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
					 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
			break;
		case ETHERNET_EVENT_DISCONNECTED:
			_connected = false;
			ESP_LOGI(TAG, "Ethernet Link Down");
			break;
		case ETHERNET_EVENT_START:
			ESP_LOGI(TAG, "Ethernet Started");
			break;
		case ETHERNET_EVENT_STOP:
			_connected = false;
			ESP_LOGI(TAG, "Ethernet Stopped");
			break;
		default:
			break;
	}
}
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
								 int32_t event_id, void *event_data)
{
	ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
	const tcpip_adapter_ip_info_t *ip_info = &event->ip_info;

	ESP_LOGI(TAG, "Ethernet Got IP Address");
	ESP_LOGI(TAG, "~~~~~~~~~~~");
	ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
	ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
	ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
	ESP_LOGI(TAG, "~~~~~~~~~~~");
	_connected = true;
}
bool ero::router::internet::is_connected() {
	return _connected;
}
ero::router::error_t ero::router::internet::connect() {
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(tcpip_adapter_set_default_eth_handlers());
	ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, nullptr));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, nullptr));

	eth_mac_config_t mac_config;
	mac_config.sw_reset_timeout_ms = 100;
	mac_config.rx_task_stack_size = 4096;
    mac_config.rx_task_prio = 15;
    mac_config.queue_len = 100;

	eth_phy_config_t phy_config;
	phy_config.phy_addr = 0;
	phy_config.reset_timeout_ms = 1000;
	phy_config.autonego_timeout_ms = 4000;

	esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
	esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);
	esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
	esp_eth_handle_t eth_handle = NULL;
	ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));
	return error_t::OK;
}