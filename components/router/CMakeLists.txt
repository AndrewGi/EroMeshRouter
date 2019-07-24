idf_component_register(SRCS "router_socket.cpp" "router.cpp" "router_internet.cpp"
		INCLUDE_DIRS "include"
		PRIVATE_INCLUDE_DIRS "include"
		EMBED_TXTFILES dev_ca_certificate.crt
		REQUIRES esp-tls esp_eth)
