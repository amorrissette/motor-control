PLUGIN_NAME = motor_control

HEADERS = motor.h

SOURCES = motor.cpp\
					moc_motor.cpp\

LIBS = -lphidget21

### Do not edit below this line ###

include $(shell rtxi_plugin_config --pkgdata-dir)/Makefile.plugin_compile
