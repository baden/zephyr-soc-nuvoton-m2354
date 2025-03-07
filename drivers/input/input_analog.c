#define DT_DRV_COMPAT analog_input

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/input/input.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/util.h>
#include "input_analog.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(analog_input, CONFIG_INPUT_LOG_LEVEL);

#if 0
#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};
// /* Get the number of channels defined on the DTS. */
#define CHANNEL_COUNT ARRAY_SIZE(adc_channels)
#endif

struct analog_input_channel_config {
	struct adc_dt_spec adc;
    uint16_t id;
	int16_t r_top;
	int16_t r_bottom;
};

struct analog_input_channel_data {
    int index;
	int last_out;
};

struct analog_input_config {
	uint32_t poll_period_ms;
	const struct analog_input_channel_config *channel_cfg;
	struct analog_input_channel_data *channel_data;
	// struct analog_axis_calibration *calibration;
	const uint8_t num_channels;
};

struct analog_input_data {
	// struct k_sem cal_lock;
	analog_input_raw_data_t raw_data_cb;
	struct k_timer timer;
	struct k_thread thread;

	K_KERNEL_STACK_MEMBER(thread_stack,
			      CONFIG_INPUT_ANALOG_THREAD_STACK_SIZE);

#ifdef CONFIG_PM_DEVICE
	atomic_t suspended;
	struct k_sem wakeup;
#endif
};


void analog_input_set_raw_data_cb(const struct device *dev, analog_input_raw_data_t cb)
{
    // LOG_ERR("analog_input_set_raw_data_cb. TODO: Must be implemented. (%s)", dev->name);
	struct analog_input_data *data = dev->data;

	data->raw_data_cb = cb;
}

// int16_t bufs[32];   // TODO: Треба визначитись, чи треба робити буфер глобальним, чи локальним.

static void analog_input_loop(const struct device *dev)
{
	const struct analog_input_config *cfg = dev->config;
	struct analog_input_data *data = dev->data;

    // WTF? Як це буде працювати? Я не розумію.
    // Дуже не хочеться шоб це було реалізовано через mem*alloc
    int16_t bufs[cfg->num_channels];
    // int32_t out;
	struct adc_sequence sequence = {
		.buffer = bufs,
		// .buffer_size = cfg->num_channels * sizeof(uint16_t) //sizeof(bufs),
		.buffer_size = sizeof(bufs),
	};
    const struct analog_input_channel_config *cfg_0 = &cfg->channel_cfg[0];
	int err;
	int i;

    // INFO: Це буде працювати виключно якшо задіян загальний для всіх каналів АЦП
    adc_sequence_init_dt(&cfg_0->adc, &sequence);

	for (i = 0; i < cfg->num_channels; i++) {
		const struct analog_input_channel_config *ch_cfg = &cfg->channel_cfg[i];
        // struct analog_input_channel_data *ch_data = &cfg->channel_data[i];
        sequence.channels |= BIT(ch_cfg->adc.channel_id);
	}

    // LOG_ERR("analog_input_loop: sequence.channels: 0x%08x", sequence.channels);

	err = adc_read(cfg_0->adc.dev, &sequence);
	if (err < 0) {
		LOG_ERR("Could not read (%d)", err);
		return;
	}

    // LOG_ERR("analog_input_loop: sequence.channels: 0x%08x", sequence.channels);

    // LOG_ERR("analog_input_loop:"
    //     " [0]:%d [1]:%d [2]:%d [3]:%d [4]:%d [5]:%d [6]:%d [7]:%d"
    //     , bufs[0], bufs[1], bufs[2], bufs[3], bufs[4], bufs[5], bufs[6], bufs[7]
    // );

    for (i = 0; i < cfg->num_channels; i++) {
		const struct analog_input_channel_config *ch_cfg = &cfg->channel_cfg[i];
        // const struct analog_input_channel_config *cfg_0 = &cfg->channel_cfg[0];
		struct analog_input_channel_data *ch_data = &cfg->channel_data[i];
        int index = ch_data->index;
		int32_t raw_val = bufs[index];

        int r_top = ch_cfg->r_top;
        int r_bottom = ch_cfg->r_bottom;

        // LOG_ERR("analog_input_loop: i=%d  index=%d raw_val: %d", i, index, raw_val);

        adc_raw_to_millivolts_dt(&ch_cfg->adc, &raw_val);

        // LOG_ERR("analog_input_loop: i=%d  index=%d mv_val: %d", i, index, raw_val);

        /* voltage scaled by voltage divider values using DT binding */
	    raw_val = /*(int64_t)*/raw_val * (r_top + r_bottom) / r_bottom;

        // LOG_ERR("analog_input_loop: i=%d  index=%d raw_val: %d", i, index, raw_val);

		if (data->raw_data_cb != NULL) {
			data->raw_data_cb(dev, ch_cfg->id, raw_val);
		}

    }

    // LOG_ERR("analog_input_loop.  TODO: Must be implemented.");
}

static void analog_input_thread(void *arg1, void *arg2, void *arg3)
{
	const struct device *dev = arg1;
	const struct analog_input_config *cfg = dev->config;
	struct analog_input_data *data = dev->data;
	int err;
	int i;

    // LOG_ERR("analog_input_thread/num_channels: %d", cfg->num_channels);
    for (i = 0; i < cfg->num_channels; i++) {
        const struct analog_input_channel_config *input_cfg = &cfg->channel_cfg[i];
		if (!adc_is_ready_dt(&input_cfg->adc)) {
			LOG_ERR("ADC controller device not ready");
			return;
}
		err = adc_channel_setup_dt(&input_cfg->adc);
		if (err < 0) {
			LOG_ERR("Could not setup channel #%d (%d)", i, err);
			return;
		}
    }

    // Заповнимо поле index в struct analog_input_channel_data в послідовносі зростання
    // ch_cfg->adc.channel_id
    // Шось це виглядає дуже костильно, але я не знаю як це зробити краще.
    uint32_t module_mask = 0;

	for (i = 0; i < cfg->num_channels; i++) {
		const struct analog_input_channel_config *ch_cfg = &cfg->channel_cfg[i];
        // struct analog_input_channel_data *ch_data = &cfg->channel_data[i];
		
        module_mask |= BIT(ch_cfg->adc.channel_id);
	}
    int j = 0;
    /* Get conversion data of each sample module for selected channel */
    while (module_mask) {
        uint16_t module_id = find_lsb_set(module_mask) - 1;
        for (i = 0; i < cfg->num_channels; i++) {
		    const struct analog_input_channel_config *ch_cfg = &cfg->channel_cfg[i];
            struct analog_input_channel_data *ch_data = &cfg->channel_data[i];
            if (ch_cfg->adc.channel_id == module_id) {
                ch_data->index = j;
                break;
            }
        }
        module_mask &= ~BIT(module_id);
        j++;
    }

    while(true) {
        #ifdef CONFIG_PM_DEVICE
            if (atomic_get(&data->suspended) == 1) {
                k_sem_take(&data->wakeup, K_FOREVER);
            }
        #endif

		analog_input_loop(dev);
		k_timer_status_sync(&data->timer);
    }
}

static int analog_input_init(const struct device *dev)
{
    struct analog_input_data *data = dev->data;
    k_tid_t tid;

    // k_sem_init(&data->cal_lock, 1, 1);
    k_timer_init(&data->timer, NULL, NULL);

#ifdef CONFIG_PM_DEVICE
	k_sem_init(&data->wakeup, 0, 1);
#endif

	tid = k_thread_create(&data->thread, data->thread_stack,
			      K_KERNEL_STACK_SIZEOF(data->thread_stack),
			      analog_input_thread, (void *)dev, NULL, NULL,
			      CONFIG_INPUT_ANALOG_THREAD_PRIORITY,
			      0, K_NO_WAIT);
	if (!tid) {
		LOG_ERR("thread creation failed");
		return -ENODEV;
	}

	k_thread_name_set(&data->thread, dev->name);

#ifndef CONFIG_PM_DEVICE_RUNTIME
	const struct analog_input_config *cfg = dev->config;

    // LOG_ERR("analog_input_init: k_timer_start (poll_period_ms: %d)", cfg->poll_period_ms);
	k_timer_start(&data->timer,
		      K_MSEC(cfg->poll_period_ms), K_MSEC(cfg->poll_period_ms));
#else
	int ret;

	atomic_set(&data->suspended, 1);

	pm_device_init_suspended(dev);
	ret = pm_device_runtime_enable(dev);
	if (ret < 0) {
		LOG_ERR("Failed to enable runtime power management");
		return ret;
	}
#endif

    // LOG_ERR("analog_input_init. TODO: Must be implemented. (%s)", dev->name);
    return 0;
}


#ifdef CONFIG_PM_DEVICE
static int analog_input_pm_action(const struct device *dev, enum pm_device_action action)
{
	const struct analog_input_config *cfg = dev->config;
	struct analog_input_data *data = dev->data;

	switch (action) {
	case PM_DEVICE_ACTION_SUSPEND:
		atomic_set(&data->suspended, 1);
		k_timer_stop(&data->timer);
		break;
	case PM_DEVICE_ACTION_RESUME:
		k_timer_start(&data->timer,
			      K_MSEC(cfg->poll_period_ms),
			      K_MSEC(cfg->poll_period_ms));
		atomic_set(&data->suspended, 0);
		k_sem_give(&data->wakeup);
		break;
	default:
		return -ENOTSUP;
	}
    
    return 0;
}
#endif


#define ANALOG_INPUT_CHANNEL_CFG_DEF(node_id) \
	{ \
		.adc = ADC_DT_SPEC_GET(node_id), \
        .id = (uint16_t)DT_PROP(node_id, id), \
		.r_top = (int16_t)DT_PROP(node_id, r_top), \
		.r_bottom = (int16_t)DT_PROP(node_id, r_bottom), \
	}

#define ANALOG_INPUT_INIT(inst)									\
	static const struct analog_input_channel_config analog_input_channel_cfg_##inst[] = {	\
		DT_INST_FOREACH_CHILD_STATUS_OKAY_SEP(inst, ANALOG_INPUT_CHANNEL_CFG_DEF, (,))	\
	};											\
												\
	static struct analog_input_channel_data							\
		analog_input_channel_data_##inst[ARRAY_SIZE(analog_input_channel_cfg_##inst)];	\
										\
												\
	static const struct analog_input_config analog_input_cfg_##inst = {			\
		.poll_period_ms = DT_INST_PROP(inst, poll_period_ms),				\
		.channel_cfg = analog_input_channel_cfg_##inst,					\
		.channel_data = analog_input_channel_data_##inst,				\
		.num_channels = ARRAY_SIZE(analog_input_channel_cfg_##inst),			\
	};											\
												\
	static struct analog_input_data analog_input_data_##inst;					\
												\
	PM_DEVICE_DT_INST_DEFINE(inst, analog_input_pm_action);					\
												\
	DEVICE_DT_INST_DEFINE(inst, analog_input_init, PM_DEVICE_DT_INST_GET(inst),		\
			      &analog_input_data_##inst, &analog_input_cfg_##inst,		\
			      POST_KERNEL, CONFIG_INPUT_INIT_PRIORITY, NULL);

DT_INST_FOREACH_STATUS_OKAY(ANALOG_INPUT_INIT)
