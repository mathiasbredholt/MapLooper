idf_component_register(SRCS "Clock.cpp"
                       INCLUDE_DIRS include ../../external/link/include 
                       REQUIRES asio libmapper nvs_flash)
target_compile_options(${COMPONENT_LIB} PUBLIC -Wno-multichar)
