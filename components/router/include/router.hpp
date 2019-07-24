#pragma once
#include <cstdint>
namespace ero {
	namespace router {
		enum class error_t {
			OK,
			CONNECTION_ERROR,
			TLS_ERROR,
			TIMEOUT,
		};
		enum class opcode_t : uint16_t {
			Ping = 0,
			Pong = 1,
			Status = 2,
			MeshMessage = 3,
			MeshProvisioning = 4,
			Administration = 5,
		};
		struct message_t {
			opcode_t opcode;
			uint16_t size;
			uint8_t* buffer;
		};
		error_t start();
		error_t send(const message_t& message);
		error_t read(message_t& message);
	}
}