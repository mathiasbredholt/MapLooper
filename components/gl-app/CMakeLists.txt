idf_component_register(SRCS GestureRecorder.cpp Mapper.cpp TrackParameters.cpp Sequencer.cpp ParameterLayout.cpp Pattern.cpp Track.cpp GestureLooper.cpp Clock.cpp
                       INCLUDE_DIRS include ${external_dir}/link/include 
                       REQUIRES gl-util gl-hal asio libmapper)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-multichar -fexceptions)
