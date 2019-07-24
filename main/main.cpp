/* ethernet Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "router_internet.hpp"



static const char *TAG = "ero_router";

void cpp_main()
{
	ESP_LOGI(TAG, "starting...");
   	ero::router::start();
	ESP_LOGI(TAG, "ready!");

}
extern "C" {
	void app_main() {
		cpp_main();
	}
}
