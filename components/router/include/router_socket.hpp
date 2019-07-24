#pragma once
#include "esp_err.h"
#include "router.hpp"
namespace ero {
	namespace router {
		namespace socket {
			constexpr const char HOSTNAME[] = "network.dev.tapjac.tech";
			constexpr uint16_t PORT = 17737;
			error_t connect();
			error_t disconnect();
			error_t send(const uint8_t* bytes, uint16_t amount);
			error_t read(uint8_t* bytes, uint16_t amount, bool timeout=true);
		}
	}
}