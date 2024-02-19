# Minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Set the project name
project(Arduino_Project)

# Set the Arduino board and its properties
set(ARDUINO_BOARD "arduino:avr:uno")
set(ARDUINO_PORT "/dev/ttyACM0")
set(ARDUINO_PROGRAMMER "arduino")

# Set the path to the Arduino toolchain
set(ARDUINO_TOOLCHAIN_PATH "/path/to/arduino/toolchain")

# Set the path to the Arduino libraries
set(ARDUINO_LIBRARIES_PATH "/path/to/arduino/libraries")

# Set the source files for the Arduino project
set(SOURCE_FILES
    src/main.cpp
)

# Include the Arduino CMake module
include(${ARDUINO_TOOLCHAIN_PATH}/Arduino.cmake)

# Create the Arduino executable
arduino_executable(${PROJECT_NAME} ${ARDUINO_BOARD} ${SOURCE_FILES})

# Set the upload options
set(UPLOAD_OPTIONS
    -P${ARDUINO_PORT}
    -C${ARDUINO_TOOLCHAIN_PATH}/avrdude.conf
    -carduino
    -P${ARDUINO_PORT}
    -b115200
    -D
    -Uflash:w:${PROJECT_NAME}.hex:i
)

# Create the upload target
add_custom_target(upload
    COMMAND ${ARDUINO_TOOLCHAIN_PATH}/avrdude ${UPLOAD_OPTIONS}
    DEPENDS ${PROJECT_NAME}
    COMMENT "Uploading ${PROJECT_NAME} to ${ARDUINO_BOARD} on ${ARDUINO_PORT}"
)

# Set the default target to be the upload target
set_directory_properties(PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_NAME}.hex
)
