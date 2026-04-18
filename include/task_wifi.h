#ifndef __TASK_WIFI_H__
#define __TASK_WIFI_H__

#include <WiFi.h>
#include <task_check_info.h>
#include <task_webserver.h>
#include <Preferences.h>

// Hàm khởi tạo Wi-Fi ban đầu
void init_wifi(SystemHandles* handles);

// Khai báo task Wi-Fi
void wifi_task(void *pvParameters);

#endif