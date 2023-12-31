// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_rom_sys.h"

#if CONFIG_COMPILER_STACK_CHECK

#include "esp_log.h"
const static char *TAG = "stack_chk";

void *__stack_chk_guard = NULL;

static void __attribute__ ((constructor))
__esp_stack_guard_setup (void)
{
    ESP_LOGD(TAG, "Intialize random stack guard");
    __stack_chk_guard = (void *)esp_random();
}

IRAM_ATTR void __stack_chk_fail (void)
{
    esp_system_abort(DRAM_STR("Stack smashing protect failure!"));
}

#endif
