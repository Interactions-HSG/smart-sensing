#include "thread_helper.h"

LOG_MODULE_REGISTER(thread_helper, CONFIG_COAP_CLIENT_UTILS_LOG_LEVEL);

thread_mode_change_cb_t mode_change_cb;

otLinkModeConfig getCurrentMode(void)
{
	struct otInstance *instance = openthread_get_default_instance();
	otLinkModeConfig mode = otThreadGetLinkMode(instance);
	return mode;
}


/**************************************************************************************************
  Switches from minimal (MED) to sleepy end device (SED) and vice versa.
**************************************************************************************************/

//typedef struct otLinkModeConfig
//{
//    bool mRxOnWhenIdle : 1; ///< 1, if the sender has its receiver on when not transmitting. 0, otherwise.
//    bool mDeviceType : 1;   ///< 1, if the sender is an FTD. 0, otherwise.
//    bool mNetworkData : 1;  ///< 1, if the sender requires the full Network Data. 0, otherwise.
//}

void setCurrentMode(otLinkModeConfig mode)
{
	otError error;
    struct openthread_context *context = openthread_get_default_context();

	openthread_api_mutex_lock(context);
	error = otThreadSetLinkMode(context->instance, mode);
	error = otLinkSetPollPeriod(context->instance, 10000);
	openthread_api_mutex_unlock(context);

	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to set MLE link mode configuration");
	} else {
		otLinkModeConfig mode = getCurrentMode();
		LOG_INF("Current mode is %d", mode.mRxOnWhenIdle);
		mode_change_cb(mode);
	}
}


void on_thread_state_changed(uint32_t flags, void *context, void *user_data)
{
    struct openthread_context *ot_context = context;
	if (flags & OT_CHANGED_THREAD_ROLE) {
		switch (otThreadGetDeviceRole(ot_context->instance)) {
		case OT_DEVICE_ROLE_CHILD:
		case OT_DEVICE_ROLE_ROUTER:
		case OT_DEVICE_ROLE_LEADER:
            LOG_INF("Thread Connected");
			break;

		case OT_DEVICE_ROLE_DISABLED:
		case OT_DEVICE_ROLE_DETACHED:
		default:
            LOG_INF("Thread Disconnected");
			break;
		}
		otLinkModeConfig mode = getCurrentMode();
		mode_change_cb(mode);
	}
}

static struct openthread_state_changed_cb ot_state_chaged_cb = { .state_changed_cb = on_thread_state_changed };

void startThread(thread_mode_change_cb_t callback)
{
	mode_change_cb = callback;
    //openthread_set_state_changed_cb(on_thread_state_changed);
	openthread_state_changed_cb_register(openthread_get_default_context(), &ot_state_chaged_cb);
    LOG_INF("Calling openthread_start..");
	openthread_start(openthread_get_default_context());
    LOG_INF("Calling openthread_start..done");
	otLinkModeConfig mode = getCurrentMode();
	mode_change_cb(mode);
}