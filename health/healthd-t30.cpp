/*
 * Copyright (C) 2013 The Android Open Source Project
 * Copyright (C) 2013 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "healthd-t30"
#include <healthd.h>
#include <fcntl.h>
#include <cutils/klog.h>

// Update touch screen "plugged" status
#define PLUGGED_PATH    "/sys/Touch/plugged"
#define PLUGGED_OFF     "2"
#define PLUGGED_ON      "3"

static int plugged_fd;
static int old_plugged = -1;

void
healthd_board_init(struct healthd_config *config)
{
    plugged_fd = open(PLUGGED_PATH, O_WRONLY);
    if (plugged_fd == -1) {
        KLOG_ERROR(LOG_TAG, "open %s failed; errno=%d\n", PLUGGED_PATH, errno);
    }
}

int
healthd_board_battery_update(struct android::BatteryProperties *props)
{
    int plugged;

    if (plugged_fd < 0) {
        goto out;
    }

    plugged = props->chargerAcOnline;
    if (plugged != old_plugged) {
        old_plugged = plugged;
        if (write(plugged_fd, plugged ? PLUGGED_ON : PLUGGED_OFF, 1) < 0) {
            KLOG_ERROR(LOG_TAG, "write plugged status failed; errno=%d\n", errno);
        }
    }

out:
    // don't log to kernel
    return 1;
}
