FILE(GLOB_RECURSE sources ./src/*.*)
idf_component_register(SRCS ${sources} 
INCLUDE_DIRS "." "src"
REQUIRES NMEA2000_esp32
)
