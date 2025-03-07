#pragma once

#include <stdint.h>
#include <zephyr/device.h>

/**
 * @brief Analog input raw data callback.
 *
 * @param dev Analog input device.
 * @param channel Channel number.
 * @param raw_val Raw value for the channel.
 */
typedef void (*analog_input_raw_data_t)(const struct device *dev,
				       int channel, int16_t raw_val);


/**
 * @brief Set a raw data callback.
 *
 * Set a callback to receive raw data for the specified analog input device.
 * This is meant to be use in the application to acquire the data to use for
 * calibration. Set cb to NULL to disable the callback.
 *
 * @param dev Analog input device.
 * @param cb An analog_input_raw_data_t callback to use, NULL disable.
 */
void analog_input_set_raw_data_cb(const struct device *dev, analog_input_raw_data_t cb);


/**
 * @brief Get the number of defined inputs.
 *
 * @retval n The number of defined axes for dev.
 */
int analog_input_num_inputs(const struct device *dev);
