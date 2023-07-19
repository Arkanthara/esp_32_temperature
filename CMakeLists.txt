set (INCLUDE_COMPONENT
	.
	../config
	)

idf_component_register(SRCS "esp32_temperature.c"
	INCLUDE_DIRS ${INCLUDE_COMPONENT}
	REQUIRES http temperature chained_list connect nvs_flash freertos)
