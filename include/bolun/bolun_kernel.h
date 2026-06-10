#ifndef BOLUN_KERNEL_H
#define BOLUN_KERNEL_H

#include "bolun_result.h"
#include "bolun_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunKernel BolunKernel;

typedef struct BolunKernelConfig {
    const char *device_name;
    const char *company;
    uint32_t default_accent_argb;
    const char *device_codename;
    uint8_t developer_mode_enabled;
} BolunKernelConfig;

BolunKernelConfig bolun_kernel_default_config(void);
BolunResult bolun_kernel_create(const BolunKernelConfig *config, BolunKernel **out_kernel);
void bolun_kernel_destroy(BolunKernel *kernel);
BolunResult bolun_kernel_boot(BolunKernel *kernel);
BolunResult bolun_kernel_shutdown(BolunKernel *kernel);
BolunResult bolun_kernel_get_status(const BolunKernel *kernel, BolunSystemStatus *out_status);
BolunResult bolun_kernel_install_app(BolunKernel *kernel, const BolunAppDescriptor *app);
BolunResult bolun_kernel_launch_app(BolunKernel *kernel, const char *app_id, char *out_message, size_t out_message_size);
BolunResult bolun_kernel_list_tiles(const BolunKernel *kernel, BolunTileDescriptor *out_tiles, size_t capacity, size_t *out_count);
BolunResult bolun_kernel_list_features(const BolunKernel *kernel, BolunFeatureDescriptor *out_features, size_t capacity, size_t *out_count);
BolunResult bolun_kernel_list_subsystems(const BolunKernel *kernel, BolunSubsystemDescriptor *out_subsystems, size_t capacity, size_t *out_count);
BolunResult bolun_kernel_get_device_profile(const BolunKernel *kernel, BolunDeviceProfile *out_profile);
BolunResult bolun_kernel_evaluate_store_removal(const BolunKernel *kernel, const char *app_id, const char *server_reason, uint8_t security_removal, BolunStoreAppRemovalPolicy *out_policy);
BolunResult bolun_kernel_push_notification(BolunKernel *kernel, const BolunNotificationDescriptor *notification);
BolunResult bolun_kernel_list_notifications(const BolunKernel *kernel, BolunNotificationDescriptor *out_notifications, size_t capacity, size_t *out_count);
BolunResult bolun_kernel_add_contact(BolunKernel *kernel, const BolunContactDescriptor *contact);
BolunResult bolun_kernel_send_message(BolunKernel *kernel, const BolunMessageDescriptor *message);
BolunResult bolun_kernel_list_messages(const BolunKernel *kernel, BolunMessageDescriptor *out_messages, size_t capacity, size_t *out_count);

#ifdef __cplusplus
}
#endif

#endif
