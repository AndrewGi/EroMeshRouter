#include "router.hpp"
#include "router_socket.hpp"
#include "router_internet.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
static const char* TAG = "ero_router";

using namespace ero;
using router::error_t;
#define ERROR_CHECK { if (error!=error_t::OK) return error;};
error_t router::send(const ero::router::message_t &message) {
	auto error = router::socket::send(reinterpret_cast<const uint8_t*>(&message.opcode), sizeof(message.opcode));
	ERROR_CHECK
	error = router::socket::send(reinterpret_cast<const uint8_t*>(message.size), sizeof(message.size));
	ERROR_CHECK
	error = router::socket::send(message.buffer, message.size);
	ERROR_CHECK
	return error_t::OK;
}
static constexpr uint8_t SEQ[] = {0xFE, 0x02};
static error_t _wait_for_seq() {
	uint8_t buf = 0;
	error_t error = error_t::OK;
	while (buf!=SEQ[1]) {
		while(buf!=SEQ[0]) {
			error = router::socket::read(&buf, sizeof(buf), false);
			ERROR_CHECK
		}
		error = router::socket::read(&buf, sizeof(buf));
		ERROR_CHECK
	}
	return error_t::OK;
}
error_t router::read(ero::router::message_t& message) {
	auto error = _wait_for_seq();
	ERROR_CHECK
	error = router::socket::read(reinterpret_cast<uint8_t*>(&message.opcode), sizeof(message.opcode));
	ERROR_CHECK
	error = router::socket::read(reinterpret_cast<uint8_t*>(&message.size), sizeof(message.size));
	ERROR_CHECK
	error = router::socket::read(message.buffer, message.size);
	ERROR_CHECK
	return error_t::OK;
}
static xTaskHandle handle = 0;
void _router_task_func(void* p_args) {
	ESP_LOGI(TAG, "router task started...");
	{
		uint8_t mac[6];
		esp_efuse_mac_get_default(mac);
		ESP_LOGI(TAG, "MAC: %02x:%02x:%02x:%02x:%02x:%02x",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
	error_t error = error_t::OK;
	router::internet::connect();
	while(!router::internet::is_connected())
		vTaskDelay(pdMS_TO_TICKS(250));
	for (;;) {
		ESP_LOGI(TAG, "connecting to cloud...");
		do {
			error = router::socket::connect();
			if (error!=error_t::OK) {
				ESP_LOGW(TAG, "unable to connect socket. error: %d", static_cast<int>(error));
				vTaskDelay(pdMS_TO_TICKS(5000));
			}
		} while(error!=error_t::OK);
		ESP_LOGI(TAG, "connected to cloud!");
	}
}
router::error_t router::start() {
	xTaskCreate(_router_task_func, "router_task", 4096, nullptr, tskIDLE_PRIORITY+1, &handle);
	return error_t::OK;
}