set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components/esp_websocket_client")

# create a cmake variable for the ESP_HAL sources
set(ESP32HAL_DIR "../../../ESP_HAL")
set(APPLICATION_DIR "../../../c_source")
set(HAL_DIR "${APPLICATION_DIR}/HAL")
set(PRIVATE_DIR "../../../private")

file(GLOB HAL_SOURCES "${ESP32HAL_DIR}/src/*.cpp")

# Enable Werror for ESP32HAL
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

idf_component_register(SRCS "pump-controller.c"
                    "app.cpp"
                    ${HAL_SOURCES}
                    INCLUDE_DIRS "."
                    ${ESP32HAL_DIR}/inc
                    ${HAL_DIR}
                    ${PRIVATE_DIR})
