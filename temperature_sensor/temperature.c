#include "driver/temp_sensor.h"


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

void read_temp_sensor(float * result)
{
	ESP_ERROR_CHECK(temp_sensor_read_celsius(result));
}

void stop_temp_sensor(void)
{
	ESP_ERROR_CHECK(temp_sensor_stop());
}
