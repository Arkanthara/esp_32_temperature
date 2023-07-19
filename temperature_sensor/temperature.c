#include "driver/temp_sensor.h"


// Start the sensor
void start_temp_sensor(void)
{
//	temp_sensor_config_t temp_sensor =  {
//		.dac_offset = TSENS_DAC_L2,
//		.clk_div = 1,
//	};
	temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(temp_sensor_set_config(temp_sensor));
	ESP_ERROR_CHECK(temp_sensor_start());
}

// Read the temperature of the sensor
void read_temp_sensor(float * result)
{
	// Attention: for avoid watchdog error, I had to modify the function static inline uint32_t temperature_sensor_ll_get_raw_value(void)
	// of the file temperature_sensor_ll.h like described here: https://github.com/espressif/esp-idf/issues/8088
	ESP_ERROR_CHECK(temp_sensor_read_celsius(result));
}

// Stop the sensor
void stop_temp_sensor(void)
{
	ESP_ERROR_CHECK(temp_sensor_stop());
}
