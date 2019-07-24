#include "router_socket.hpp"
#include "esp_tls.h"
#include "esp_log.h"
//#include "ca_cert.h"
static const char* TAG = "router_socket";

static esp_tls* _tls = nullptr;

extern const uint8_t _ca_cert_start[] asm("_binary_dev_ca_certificate_crt_start");
extern const uint8_t _ca_cert_end[] asm("_binary_dev_ca_certificate_crt_end");

ero::router::error_t ero::router::socket::connect() {
	disconnect();
	_tls = esp_tls_init();
	esp_tls_cfg_t cfg {};
	cfg.non_block = false;
	cfg.use_global_ca_store = false;
	cfg.cacert_pem_buf = _ca_cert_start;
	cfg.cacert_pem_bytes = _ca_cert_end - _ca_cert_start;
	esp_err_t err = esp_tls_conn_new_sync(HOSTNAME, sizeof(HOSTNAME), PORT, &cfg, _tls);
	if (err!=1) {
		ESP_LOGE(TAG, "new tls connection error %d", err);
		return error_t::CONNECTION_ERROR;
	}
	return error_t::OK;
}
ero::router::error_t ero::router::socket::send(const uint8_t *bytes, uint16_t amount) {
	esp_err_t err = esp_tls_conn_write(_tls, bytes, amount);
	if (err!=ESP_OK) {
		ESP_LOGE(TAG, "new tls send error %d", err);
		return error_t::TLS_ERROR;
	}
	return error_t::OK;
}
ero::router::error_t ero::router::socket::read(uint8_t *bytes, uint16_t amount, bool timeout) {
	TickType_t expire_tick = timeout?xTaskGetTickCount():0;
	while(esp_tls_get_bytes_avail(_tls)<amount) {
		vTaskDelay(pdMS_TO_TICKS(25));
		if (timeout && xTaskGetTickCount()>expire_tick) {
			ESP_LOGW(TAG, "read timeout");
			return error_t::TIMEOUT;
		}
	}
	auto amount_read = esp_tls_conn_read(_tls, bytes, amount);
	if (amount_read!=amount) {
		ESP_LOGW(TAG, "read error %d", amount_read);
		return error_t::CONNECTION_ERROR;
	}
	return error_t::OK;
}
ero::router::error_t ero::router::socket::disconnect() {
	if (_tls==nullptr)
		return error_t::OK;
	esp_tls_conn_delete(_tls);
	_tls = nullptr;
	return error_t::OK;
}