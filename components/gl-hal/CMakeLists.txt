idf_component_register(SRCS "./MIDI.cpp" "./WiFi.cpp"
                       INCLUDE_DIRS include
                       REQUIRES gl-util nvs_flash blemidi)
target_compile_options(${COMPONENT_LIB} PRIVATE -DI2C_MIDI)
