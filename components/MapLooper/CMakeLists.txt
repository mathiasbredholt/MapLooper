idf_component_register(SRCS "GestureRecorder.cpp" "GestureVoice.cpp" "Mapper.cpp" "TrackParameters.cpp" "Sequencer.cpp" "ParameterLayout.cpp" "Pattern.cpp" "Track.cpp" "MapLooper.cpp" "Clock.cpp" "MIDI.cpp"
                       INCLUDE_DIRS include ../../external/link/include 
                       REQUIRES asio libmapper nvs_flash blemidi)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-multichar -fexceptions)
