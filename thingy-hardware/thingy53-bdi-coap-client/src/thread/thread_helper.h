#ifndef __THREAD_HELPER_H__
#define __THREAD_HELPER_H__

#include <stdio.h>
#include <zephyr/kernel.h>
#include <net/coap_utils.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>


typedef void (*thread_mode_change_cb_t)(/*uint32_t val*/ otLinkModeConfig mode);

otLinkModeConfig getCurrentMode(void);
void setCurrentMode(otLinkModeConfig mode);
void on_thread_state_changed(uint32_t flags, void *context, void *user_data);
void startThread(thread_mode_change_cb_t callback);


#endif