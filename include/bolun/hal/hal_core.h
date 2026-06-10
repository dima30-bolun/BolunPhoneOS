#ifndef BOLUN_HAL_CORE_H
#define BOLUN_HAL_CORE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Bolun Phone HAL (Hardware Abstraction Layer)
 * Unified interface for hardware access across different Lumia models
 */

typedef enum {
    HAL_DEVICE_REDMI_9A,
    HAL_DEVICE_REDMI_9C_NFC,
    HAL_DEVICE_LUMIA_950,
    HAL_DEVICE_LUMIA_950_XL,
    HAL_DEVICE_LUMIA_1520,
    HAL_DEVICE_BOLUN_PLAY,
    HAL_DEVICE_UNKNOWN
} hal_device_type_t;

typedef enum {
    HAL_STATUS_SUCCESS = 0,
    HAL_STATUS_ERROR = -1,
    HAL_STATUS_NOT_SUPPORTED = -2,
    HAL_STATUS_TIMEOUT = -3,
    HAL_STATUS_INVALID_PARAM = -4
} hal_status_t;

/**
 * Display HAL Interface
 */
typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t bpp;
    uint32_t refresh_rate;
} hal_display_info_t;

hal_status_t hal_display_init(void);
hal_status_t hal_display_deinit(void);
hal_status_t hal_display_get_info(hal_display_info_t *info);
hal_status_t hal_display_set_brightness(uint8_t level);
hal_status_t hal_display_get_brightness(uint8_t *level);
hal_status_t hal_display_set_contrast(uint8_t level);
hal_status_t hal_display_power(bool on);
hal_status_t hal_display_update(const uint8_t *framebuffer, uint32_t size);

/**
 * Touch Input HAL Interface
 */
typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t pressure;
    uint8_t touch_id;
} hal_touch_event_t;

typedef void (*hal_touch_callback_t)(hal_touch_event_t *event);

hal_status_t hal_touch_init(void);
hal_status_t hal_touch_deinit(void);
hal_status_t hal_touch_register_callback(hal_touch_callback_t callback);
hal_status_t hal_touch_get_event(hal_touch_event_t *event);

/**
 * Camera HAL Interface
 */
typedef enum {
    HAL_CAMERA_REAR,
    HAL_CAMERA_FRONT
} hal_camera_type_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t fps;
    bool has_flash;
    bool has_autofocus;
} hal_camera_info_t;

hal_status_t hal_camera_init(hal_camera_type_t camera);
hal_status_t hal_camera_deinit(hal_camera_type_t camera);
hal_status_t hal_camera_get_info(hal_camera_type_t camera, hal_camera_info_t *info);
hal_status_t hal_camera_start_preview(hal_camera_type_t camera);
hal_status_t hal_camera_stop_preview(hal_camera_type_t camera);
hal_status_t hal_camera_take_photo(const char *filename);
hal_status_t hal_camera_start_video(const char *filename);
hal_status_t hal_camera_stop_video(void);
hal_status_t hal_camera_set_flash(bool enable);
hal_status_t hal_camera_set_autofocus(bool enable);

/**
 * Audio HAL Interface
 */
typedef enum {
    HAL_AUDIO_SPEAKER,
    HAL_AUDIO_EARPIECE,
    HAL_AUDIO_HEADPHONE,
    HAL_AUDIO_BT_SPEAKER
} hal_audio_output_t;

typedef struct {
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bit_depth;
} hal_audio_format_t;

hal_status_t hal_audio_init(void);
hal_status_t hal_audio_deinit(void);
hal_status_t hal_audio_set_output(hal_audio_output_t output);
hal_status_t hal_audio_set_volume(uint8_t level);
hal_status_t hal_audio_play_file(const char *filename);
hal_status_t hal_audio_stop_playback(void);
hal_status_t hal_audio_start_recording(const char *filename, hal_audio_format_t *format);
hal_status_t hal_audio_stop_recording(void);

/**
 * Network HAL Interface
 */
typedef enum {
    HAL_NET_WIFI,
    HAL_NET_CELLULAR,
    HAL_NET_BLUETOOTH,
    HAL_NET_NFC
} hal_network_type_t;

typedef struct {
    char ssid[32];
    int8_t signal_strength;
    bool is_connected;
} hal_wifi_info_t;

hal_status_t hal_wifi_init(void);
hal_status_t hal_wifi_deinit(void);
hal_status_t hal_wifi_scan(void);
hal_status_t hal_wifi_connect(const char *ssid, const char *password);
hal_status_t hal_wifi_disconnect(void);
hal_status_t hal_wifi_get_status(hal_wifi_info_t *info);

hal_status_t hal_bt_init(void);
hal_status_t hal_bt_deinit(void);
hal_status_t hal_bt_enable(void);
hal_status_t hal_bt_disable(void);
hal_status_t hal_bt_scan(void);

hal_status_t hal_nfc_init(void);
hal_status_t hal_nfc_deinit(void);
hal_status_t hal_nfc_read(uint8_t *data, uint32_t *size);
hal_status_t hal_nfc_write(const uint8_t *data, uint32_t size);

/**
 * Sensor HAL Interface
 */
typedef enum {
    HAL_SENSOR_ACCELEROMETER,
    HAL_SENSOR_GYROSCOPE,
    HAL_SENSOR_MAGNETOMETER,
    HAL_SENSOR_PROXIMITY,
    HAL_SENSOR_LIGHT,
    HAL_SENSOR_GPS
} hal_sensor_type_t;

typedef struct {
    float x;
    float y;
    float z;
} hal_sensor_data_t;

hal_status_t hal_sensor_init(hal_sensor_type_t sensor);
hal_status_t hal_sensor_deinit(hal_sensor_type_t sensor);
hal_status_t hal_sensor_read(hal_sensor_type_t sensor, hal_sensor_data_t *data);

/**
 * Power Management HAL Interface
 */
typedef enum {
    HAL_POWER_ACTIVE,
    HAL_POWER_IDLE,
    HAL_POWER_SUSPEND,
    HAL_POWER_HIBERNATE
} hal_power_state_t;

typedef struct {
    uint8_t battery_level;
    bool is_charging;
    uint32_t time_remaining_ms;
    float temperature;
} hal_battery_info_t;

hal_status_t hal_power_set_state(hal_power_state_t state);
hal_status_t hal_power_get_battery_info(hal_battery_info_t *info);
hal_status_t hal_power_set_brightness(uint8_t level);

/**
 * Device Management
 */
hal_status_t hal_init(hal_device_type_t device);
hal_status_t hal_deinit(void);
hal_device_type_t hal_get_device_type(void);
const char* hal_get_device_name(void);
hal_status_t hal_get_device_info(char *info_buffer, uint32_t buffer_size);

#endif // BOLUN_HAL_CORE_H
