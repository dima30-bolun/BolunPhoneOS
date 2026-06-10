#ifndef BOLUN_SUBSYSTEMS_H
#define BOLUN_SUBSYSTEMS_H

#include "bolun_types.h"

#ifdef __cplusplus
extern "C" {
#endif

BolunSubsystemDescriptor bolun_ai_subsystem(void);
BolunSubsystemDescriptor bolun_apps_subsystem(void);
BolunSubsystemDescriptor bolun_battery_subsystem(void);
BolunSubsystemDescriptor bolun_bid_subsystem(void);
BolunSubsystemDescriptor bolun_bluetooth_subsystem(void);
BolunSubsystemDescriptor bolun_cloud_subsystem(void);
BolunSubsystemDescriptor bolun_crypto_subsystem(void);
BolunSubsystemDescriptor bolun_input_subsystem(void);
BolunSubsystemDescriptor bolun_installer_subsystem(void);
BolunSubsystemDescriptor bolun_notifications_subsystem(void);
BolunSubsystemDescriptor bolun_permissions_subsystem(void);
BolunSubsystemDescriptor bolun_phone_subsystem(void);
BolunSubsystemDescriptor bolun_services_subsystem(void);
BolunSubsystemDescriptor bolun_shell_subsystem(void);
BolunSubsystemDescriptor bolun_themes_subsystem(void);
BolunSubsystemDescriptor bolun_usb_subsystem(void);
BolunSubsystemDescriptor bolun_vpn_subsystem(void);
BolunSubsystemDescriptor bolun_wifi_subsystem(void);
BolunSubsystemDescriptor bolun_youtube_subsystem(void);
BolunSubsystemDescriptor bolun_audio_subsystem(void);
BolunSubsystemDescriptor bolun_bapp_subsystem(void);
BolunSubsystemDescriptor bolun_camera_subsystem(void);
BolunSubsystemDescriptor bolun_drivers_subsystem(void);
BolunSubsystemDescriptor bolun_filesystem_subsystem(void);
BolunSubsystemDescriptor bolun_graphics_subsystem(void);
BolunSubsystemDescriptor bolun_network_subsystem(void);
BolunSubsystemDescriptor bolun_power_subsystem(void);
BolunSubsystemDescriptor bolun_security_subsystem(void);
BolunSubsystemDescriptor bolun_store_subsystem(void);
BolunSubsystemDescriptor bolun_update_subsystem(void);

#ifdef __cplusplus
}
#endif

#endif
