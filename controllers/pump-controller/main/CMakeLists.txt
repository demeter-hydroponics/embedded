set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components/esp_websocket_client")

# create a cmake variable for the ESP_HAL sources
set(ESP32HAL_DIR "../../../ESP_HAL")
set(APPLICATION_DIR "../../../c_source")
set(HAL_DIR "${APPLICATION_DIR}/HAL")
set(PRIVATE_DIR "../../../private")
set(MESSAGES_DIR "../../../generated/c")
set(PROTOBUF_DIR "../../../libs/protobuf")
set(NANOPB_DIR "${PROTOBUF_DIR}/submodules/nanopb")


set(DRIVER_INC_DIR "${APPLICATION_DIR}/inc/driver")
set(DEVICE_INC_DIR "${APPLICATION_DIR}/inc/device")
set(APP_INC_DIR "${APPLICATION_DIR}/inc/app")

file(GLOB HAL_SOURCES "${ESP32HAL_DIR}/src/*.cpp")
file(GLOB_RECURSE HIGH_LEVEL_SOURCES "${APPLICATION_DIR}/**/*.cpp")
file(GLOB PB_SOURCES "${NANOPB_DIR}/*.c")
file(GLOB COMMON_MSG_SOURCES "${MESSAGES_DIR}/*.c")
file(GLOB_RECURSE MESSAGES_SOURCES "${MESSAGES_DIR}/**/*.c")
# Remove files from the test directory
list(FILTER HIGH_LEVEL_SOURCES EXCLUDE REGEX "${APPLICATION_DIR}/test/.*\\.cpp$")

# Enable Werror for ESP32HAL
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

idf_component_register(SRCS "pump-controller.c"
                    "app_pump.cpp"
                    ${HIGH_LEVEL_SOURCES}
                    ${HAL_SOURCES}
                    ${PB_SOURCES}
                    ${COMMON_MSG_SOURCES}
                    ${MESSAGES_SOURCES}
                    INCLUDE_DIRS "."
                    ${NANOPB_DIR}
                    ${ESP32HAL_DIR}/inc
                    ${HAL_DIR}
                    ${PRIVATE_DIR}
                    ${DRIVER_INC_DIR}
                    ${DEVICE_INC_DIR}
                    ${APP_INC_DIR}
                    ${MESSAGES_DIR})
