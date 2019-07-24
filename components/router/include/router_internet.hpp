#pragma once
#include "router.hpp"

namespace ero {
	namespace router {
		namespace internet {
			router::error_t connect();
			bool is_connected();
		}
	}
}