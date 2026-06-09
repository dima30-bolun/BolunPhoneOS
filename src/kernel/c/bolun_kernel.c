#include "bolun/bolun_boot.h"
#include "bolun/bolun_kernel.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BolunKernel {
    BolunKernelConfig config;
    char device_name[BOLUN_TEXT_CAPACITY];
    char company[BOLUN_TEXT_CAPACITY];
    BolunDeviceProfile device_profile;
    uint8_t booted;
    BolunAppDescriptor apps[BOLUN_MAX_APPS];
    size_t app_count;
    BolunTileDescriptor tiles[BOLUN_MAX_TILES];
    size_t tile_count;
    BolunFeatureDescriptor features[BOLUN_MAX_FEATURES];
    size_t feature_count;
    BolunNotificationDescriptor notifications[BOLUN_MAX_NOTIFICATIONS];
    size_t notification_count;
    BolunContactDescriptor contacts[BOLUN_MAX_CONTACTS];
    size_t contact_count;
    BolunMessageDescriptor messages[BOLUN_MAX_MESSAGES];
    size_t message_count;
    uint64_t next_notification_sequence;
    uint64_t next_message_sequence;
};

static void copy_text(char *destination, size_t capacity, const char *source) {
    if (capacity == 0) {
        return;
    }
    snprintf(destination, capacity, "%s", source == NULL ? "" : source);
}

static int contains_ascii_case_insensitive(const char *text, const char *needle) {
    if (text == NULL || needle == NULL || needle[0] == '\0') {
        return 0;
    }
    size_t text_length = strlen(text);
    size_t needle_length = strlen(needle);
    if (needle_length > text_length) {
        return 0;
    }
    for (size_t start = 0; start + needle_length <= text_length; ++start) {
        size_t offset = 0;
        while (offset < needle_length && tolower((unsigned char)text[start + offset]) == tolower((unsigned char)needle[offset])) {
            ++offset;
        }
        if (offset == needle_length) {
            return 1;
        }
    }
    return 0;
}

static BolunAppDescriptor app(const char *id, const char *title, const char *capability, BolunTileSize size, uint32_t accent) {
    BolunAppDescriptor descriptor;
    memset(&descriptor, 0, sizeof(descriptor));
    copy_text(descriptor.app_id, sizeof(descriptor.app_id), id);
    copy_text(descriptor.title, sizeof(descriptor.title), title);
    copy_text(descriptor.publisher, sizeof(descriptor.publisher), "dima30");
    copy_text(descriptor.capability, sizeof(descriptor.capability), capability);
    descriptor.preferred_tile_size = size;
    descriptor.accent_argb = accent;
    return descriptor;
}

static BolunFeatureDescriptor feature(const char *area, const char *name, BolunFeatureState state, const char *notes) {
    BolunFeatureDescriptor descriptor;
    memset(&descriptor, 0, sizeof(descriptor));
    copy_text(descriptor.area, sizeof(descriptor.area), area);
    copy_text(descriptor.name, sizeof(descriptor.name), name);
    copy_text(descriptor.notes, sizeof(descriptor.notes), notes);
    descriptor.state = state;
    return descriptor;
}

static BolunResult add_feature(BolunKernel *kernel, const BolunFeatureDescriptor *descriptor) {
    if (kernel->feature_count >= BOLUN_MAX_FEATURES) {
        return BOLUN_ERROR_FULL;
    }
    kernel->features[kernel->feature_count++] = *descriptor;
    return BOLUN_OK;
}

static BolunResult install_default_features(BolunKernel *kernel) {
    const BolunFeatureDescriptor defaults[] = {
        feature("Kernel", "Process scheduler", BOLUN_FEATURE_READY, "Round-robin educational scheduler contract."),
        feature("Kernel", "Multitasking", BOLUN_FEATURE_READY, "Foreground app plus background services."),
        feature("Kernel", "Memory manager", BOLUN_FEATURE_READY, "Page ownership and quota model."),
        feature("Kernel", "Virtual memory", BOLUN_FEATURE_READY, "Per-process virtual address map abstraction."),
        feature("Kernel", "IPC", BOLUN_FEATURE_READY, "Message queues for system services."),
        feature("Kernel", "System calls", BOLUN_FEATURE_READY, "Stable C ABI entry points."),
        feature("Kernel", "Timers", BOLUN_FEATURE_READY, "Monotonic timers for apps and services."),
        feature("Kernel", "Interrupt handling", BOLUN_FEATURE_READY, "Assembler interrupt stub wired into the HAL."),
        feature("Kernel", "Power saving", BOLUN_FEATURE_READY, "Screen, radio, and CPU idle policies."),
        feature("Kernel", "Event logging", BOLUN_FEATURE_READY, "Boot, app, store, and security events."),
        feature("Boot", "Bootloader", BOLUN_FEATURE_READY, "Bolun boot handoff contract."),
        feature("Boot", "Secure Boot", BOLUN_FEATURE_READY, "Signed image requirement for production devices."),
        feature("Boot", "Recovery Mode", BOLUN_FEATURE_READY, "Rollback and repair environment."),
        feature("Boot", "Fastboot-like mode", BOLUN_FEATURE_READY, "Developer flashing transport."),
        feature("Boot", "Developer Mode", BOLUN_FEATURE_DEVELOPER_ONLY, "Unlocks diagnostics and developer tools folder."),
        feature("Boot", "OTA updates", BOLUN_FEATURE_READY, "A/B style update metadata with rollback."),
        feature("Drivers", "Display", BOLUN_FEATURE_READY, "Framebuffer/display driver contract."),
        feature("Drivers", "Touch screen", BOLUN_FEATURE_READY, "Pointer and gesture input contract."),
        feature("Drivers", "Buttons", BOLUN_FEATURE_READY, "Power, volume, and navigation buttons."),
        feature("Drivers", "Vibration motor", BOLUN_FEATURE_READY, "Haptic feedback service."),
        feature("Drivers", "Accelerometer", BOLUN_FEATURE_READY, "Sensor fusion input."),
        feature("Drivers", "Gyroscope", BOLUN_FEATURE_READY, "Motion sensor input."),
        feature("Drivers", "Compass", BOLUN_FEATURE_READY, "Magnetometer support."),
        feature("Drivers", "Proximity sensor", BOLUN_FEATURE_READY, "Call-screen blanking support."),
        feature("Drivers", "Light sensor", BOLUN_FEATURE_READY, "Adaptive brightness support."),
        feature("Drivers", "GPS", BOLUN_FEATURE_READY, "Location provider."),
        feature("Drivers", "NFC", BOLUN_FEATURE_HARDWARE_OPTIONAL, "Enabled only on NFC devices; QR fallback is always available."),
        feature("Drivers", "Bluetooth", BOLUN_FEATURE_READY, "Pairing, file transfer, and audio."),
        feature("Drivers", "Wi-Fi", BOLUN_FEATURE_READY, "Client networking."),
        feature("Drivers", "LTE / 3G / 2G", BOLUN_FEATURE_HARDWARE_OPTIONAL, "Modem stack when hardware is present."),
        feature("Drivers", "SIM card", BOLUN_FEATURE_HARDWARE_OPTIONAL, "SIM identity and network registration."),
        feature("Drivers", "USB", BOLUN_FEATURE_READY, "Charging, storage, and debug transport."),
        feature("Drivers", "Charging", BOLUN_FEATURE_READY, "USB charger negotiation."),
        feature("Drivers", "Battery", BOLUN_FEATURE_READY, "Health, percentage, and thermal telemetry."),
        feature("Drivers", "Camera", BOLUN_FEATURE_READY, "Capture graph abstraction."),
        feature("Drivers", "Flash", BOLUN_FEATURE_HARDWARE_OPTIONAL, "Torch and photo flash."),
        feature("Drivers", "Speaker", BOLUN_FEATURE_READY, "Audio output."),
        feature("Drivers", "Microphone", BOLUN_FEATURE_READY, "Audio input."),
        feature("Drivers", "Headphone jack", BOLUN_FEATURE_HARDWARE_OPTIONAL, "Analog headset routing when present."),
        feature("Drivers", "SD card", BOLUN_FEATURE_HARDWARE_OPTIONAL, "Removable storage when present."),
        feature("Graphics", "GPU acceleration", BOLUN_FEATURE_READY, "Compositor acceleration contract."),
        feature("Graphics", "Double buffering", BOLUN_FEATURE_READY, "Tear-free shell rendering."),
        feature("Graphics", "60 FPS animations", BOLUN_FEATURE_READY, "Frame pacing target."),
        feature("Graphics", "Dark theme", BOLUN_FEATURE_READY, "System palette."),
        feature("Graphics", "Light theme", BOLUN_FEATURE_READY, "System palette."),
        feature("Graphics", "Live tiles", BOLUN_FEATURE_READY, "Tile model and badge updates."),
        feature("Graphics", "Widgets", BOLUN_FEATURE_READY, "Embeddable glance information."),
        feature("Graphics", "Always-On Display", BOLUN_FEATURE_HARDWARE_OPTIONAL, "Enabled only on compatible display panels."),
        feature("Graphics", "Adaptive themes", BOLUN_FEATURE_READY, "Theme adapts to wallpaper and time."),
        feature("Camera", "Photo", BOLUN_FEATURE_READY, "Still capture."),
        feature("Camera", "Video", BOLUN_FEATURE_READY, "Video capture."),
        feature("Camera", "HDR", BOLUN_FEATURE_READY, "Multi-frame exposure merge."),
        feature("Camera", "Night mode", BOLUN_FEATURE_READY, "Low-light capture pipeline."),
        feature("Camera", "Panorama", BOLUN_FEATURE_READY, "Guided panorama capture."),
        feature("Camera", "Burst", BOLUN_FEATURE_READY, "Continuous shot queue."),
        feature("Camera", "Timer", BOLUN_FEATURE_READY, "Delayed shutter."),
        feature("Camera", "QR scanner", BOLUN_FEATURE_READY, "QR scanning available on every target."),
        feature("Camera", "Document scanner", BOLUN_FEATURE_READY, "Perspective crop and cleanup."),
        feature("Camera", "AI noise reduction", BOLUN_FEATURE_READY, "Denoising stage."),
        feature("Camera", "AI sharpening", BOLUN_FEATURE_READY, "Detail enhancement stage."),
        feature("Camera", "AI color correction", BOLUN_FEATURE_READY, "Automatic color balance."),
        feature("Camera", "AI detail recovery", BOLUN_FEATURE_READY, "Recovery stage for compressed images."),
        feature("Camera", "AI portrait mode", BOLUN_FEATURE_READY, "Subject segmentation and blur."),
        feature("Camera", "Electronic video stabilization", BOLUN_FEATURE_READY, "Motion-smoothed video."),
        feature("Audio", "Voice recorder", BOLUN_FEATURE_READY, "Built-in recording app and service."),
        feature("Audio", "AI noise suppression", BOLUN_FEATURE_READY, "Speech denoising."),
        feature("Audio", "AI voice enhancement", BOLUN_FEATURE_READY, "Voice clarity pipeline."),
        feature("Audio", "Echo cancellation", BOLUN_FEATURE_READY, "Hands-free calls."),
        feature("Audio", "Equalizer", BOLUN_FEATURE_READY, "Per-output EQ presets."),
        feature("Audio", "Spatial audio", BOLUN_FEATURE_READY, "Virtual surround profiles."),
        feature("Audio", "Bluetooth audio", BOLUN_FEATURE_READY, "A2DP-style output contract."),
        feature("Phone", "Calls", BOLUN_FEATURE_READY, "Dialer and call state."),
        feature("Phone", "Contacts", BOLUN_FEATURE_READY, "People database."),
        feature("Phone", "SMS", BOLUN_FEATURE_READY, "Text messaging."),
        feature("Phone", "MMS", BOLUN_FEATURE_READY, "Media messaging."),
        feature("Phone", "VoLTE", BOLUN_FEATURE_HARDWARE_OPTIONAL, "Requires carrier/modem support."),
        feature("Phone", "Call forwarding", BOLUN_FEATURE_READY, "Network setting UI."),
        feature("Phone", "Voicemail", BOLUN_FEATURE_READY, "Mailbox integration."),
        feature("Internet", "TCP", BOLUN_FEATURE_READY, "Transport protocol."),
        feature("Internet", "UDP", BOLUN_FEATURE_READY, "Transport protocol."),
        feature("Internet", "HTTP", BOLUN_FEATURE_READY, "Client protocol."),
        feature("Internet", "HTTPS", BOLUN_FEATURE_READY, "TLS-backed client protocol."),
        feature("Internet", "DNS", BOLUN_FEATURE_READY, "Name resolution."),
        feature("Internet", "DHCP", BOLUN_FEATURE_READY, "Network configuration."),
        feature("Internet", "WebSocket", BOLUN_FEATURE_READY, "Realtime app transport."),
        feature("Internet", "IPv6", BOLUN_FEATURE_READY, "Dual-stack networking."),
        feature("Internet", "VPN Framework", BOLUN_FEATURE_READY, "System VPN routing."),
        feature("Internet", "VPN app API", BOLUN_FEATURE_READY, "Third-party VPN provider contract."),
        feature("Security", "Memory encryption", BOLUN_FEATURE_READY, "Protected memory regions."),
        feature("Security", "File encryption", BOLUN_FEATURE_READY, "Per-user encrypted storage."),
        feature("Security", "App permissions", BOLUN_FEATURE_READY, "Runtime permission grants."),
        feature("Security", "App sandbox", BOLUN_FEATURE_READY, "Application isolation model."),
        feature("Security", "Digital signature verification", BOLUN_FEATURE_READY, "Signed apps and updates."),
        feature("Security", "Bootloader protection", BOLUN_FEATURE_READY, "Secure boot chain policy."),
        feature("Security", "B-ID", BOLUN_FEATURE_READY, "Bolun identity sign-in."),
        feature("Security", "PIN", BOLUN_FEATURE_READY, "Lock credential."),
        feature("Security", "Password", BOLUN_FEATURE_READY, "Lock credential."),
        feature("Security", "Pattern lock", BOLUN_FEATURE_READY, "Lock credential."),
        feature("Bolun AI", "AI assistant", BOLUN_FEATURE_READY, "System assistant surface."),
        feature("Bolun AI", "AI chat", BOLUN_FEATURE_READY, "Conversational UI."),
        feature("Bolun AI", "AI translator", BOLUN_FEATURE_READY, "Text translation."),
        feature("Bolun AI", "AI OCR", BOLUN_FEATURE_READY, "Text recognition."),
        feature("Bolun AI", "AI search", BOLUN_FEATURE_READY, "On-device semantic search contract."),
        feature("Bolun AI", "AI text summaries", BOLUN_FEATURE_READY, "Summary action."),
        feature("Bolun AI", "AI photo editor", BOLUN_FEATURE_READY, "Image editing actions."),
        feature("Bolun AI", "AI video enhancement", BOLUN_FEATURE_READY, "Video cleanup actions."),
        feature("Bolun AI", "AI dictation", BOLUN_FEATURE_READY, "Speech to text."),
        feature("Bolun AI", "AI keyboard", BOLUN_FEATURE_READY, "Keyboard suggestions."),
        feature("Bolun AI", "AI anti-spam", BOLUN_FEATURE_READY, "Call and message filtering."),
        feature("Bolun AI", "AI battery optimization", BOLUN_FEATURE_READY, "Adaptive power policies."),
        feature("Bolun AI", "AI memory optimization", BOLUN_FEATURE_READY, "App lifecycle tuning."),
        feature("Dima30 Store", "Install apps", BOLUN_FEATURE_READY, "Package installation."),
        feature("Dima30 Store", "Update apps", BOLUN_FEATURE_READY, "Manual app updates."),
        feature("Dima30 Store", "Automatic updates", BOLUN_FEATURE_READY, "Background app updates."),
        feature("Dima30 Store", "Ratings", BOLUN_FEATURE_READY, "User ratings."),
        feature("Dima30 Store", "Reviews", BOLUN_FEATURE_READY, "User reviews."),
        feature("Dima30 Store", "Security check", BOLUN_FEATURE_READY, "Store malware policy."),
        feature("Dima30 Store", "Developer certificates", BOLUN_FEATURE_READY, "Trusted developer identity."),
        feature("Dima30 Store", "Categories", BOLUN_FEATURE_READY, "App discovery."),
        feature("Dima30 Store", "Search", BOLUN_FEATURE_READY, "Catalog search."),
        feature("Dima30 Store", "Download stats", BOLUN_FEATURE_READY, "Store statistics."),
        feature("Dima30 Store", "Update history", BOLUN_FEATURE_READY, "Release history."),
        feature("Dima30 Store", "Beta programs", BOLUN_FEATURE_READY, "Opt-in beta tracks."),
        feature("Dima30 Store", "App version rollback", BOLUN_FEATURE_READY, "Roll back to previous app version."),
        feature("Dima30 Store", "Device compatibility", BOLUN_FEATURE_READY, "Checks hardware and OS version."),
        feature("Dima30 Store", "Removed app notification", BOLUN_FEATURE_READY, "Shows server removal status."),
        feature("Dima30 Store", "Removal reason", BOLUN_FEATURE_READY, "Server-supplied reason is shown."),
        feature("Dima30 Store", "Keep installed safe apps", BOLUN_FEATURE_READY, "Safe installed apps are not deleted automatically."),
        feature("Dima30 Store", "Block unsafe removed apps", BOLUN_FEATURE_READY, "Security removals block launch with official reason."),
        feature("Developer Tools", "Package manager", BOLUN_FEATURE_DEVELOPER_ONLY, "Developer folder tool."),
        feature("Developer Tools", ".bapp support", BOLUN_FEATURE_DEVELOPER_ONLY, "Bolun app package format."),
        feature("Developer Tools", "Ben_open.txt", BOLUN_FEATURE_DEVELOPER_ONLY, "Package metadata processing."),
        feature("Developer Tools", "Python runtime", BOLUN_FEATURE_DEVELOPER_ONLY, "Script runtime."),
        feature("Developer Tools", "Lua runtime", BOLUN_FEATURE_DEVELOPER_ONLY, "Script runtime."),
        feature("Developer Tools", "JavaScript runtime", BOLUN_FEATURE_DEVELOPER_ONLY, "Script runtime."),
        feature("Developer Tools", "TinyCC", BOLUN_FEATURE_DEVELOPER_ONLY, "C compiler tooling."),
        feature("Developer Tools", "C++ support", BOLUN_FEATURE_DEVELOPER_ONLY, "Native app tooling."),
        feature("Developer Tools", "Rust support", BOLUN_FEATURE_DEVELOPER_ONLY, "Native app tooling."),
        feature("Developer Tools", "C# runtime", BOLUN_FEATURE_DEVELOPER_ONLY, "Managed app runtime."),
        feature("Developer Tools", "HTML5", BOLUN_FEATURE_DEVELOPER_ONLY, "Web app surface."),
        feature("Developer Tools", "WebAssembly", BOLUN_FEATURE_DEVELOPER_ONLY, "Portable code runtime."),
        feature("Developer Tools", "Bolun Phone API", BOLUN_FEATURE_DEVELOPER_ONLY, "System API reference."),
        feature("Developer Tools", "Bolun Phone SDK", BOLUN_FEATURE_DEVELOPER_ONLY, "Build SDK."),
        feature("Developer Tools", "Debug libraries", BOLUN_FEATURE_DEVELOPER_ONLY, "Diagnostics helpers."),
        feature("Developer Tools", "Developer documentation", BOLUN_FEATURE_DEVELOPER_ONLY, "Offline docs."),
        feature("Developer Tools", "Digital signature tools", BOLUN_FEATURE_DEVELOPER_ONLY, "App signing checks."),
        feature("Developer Tools", ".bapp build tools", BOLUN_FEATURE_DEVELOPER_ONLY, "Package builder."),
        feature("Developer Tools", "API emulator", BOLUN_FEATURE_DEVELOPER_ONLY, "Optional API simulator."),
        feature("File System", "Journaling", BOLUN_FEATURE_READY, "Crash-resilient metadata."),
        feature("File System", "File search", BOLUN_FEATURE_READY, "Indexed search."),
        feature("File System", "Recycle bin", BOLUN_FEATURE_READY, "Recover deleted files."),
        feature("File System", "Archiver", BOLUN_FEATURE_READY, "Archive handling."),
        feature("File System", "File sharing", BOLUN_FEATURE_READY, "Share contracts."),
        feature("File System", "Encryption", BOLUN_FEATURE_READY, "Encrypted storage."),
        feature("File System", "Integrity check", BOLUN_FEATURE_READY, "Hash verification."),
        feature("Cloud", "Sync", BOLUN_FEATURE_READY, "Bolun Cloud sync."),
        feature("Cloud", "Backups", BOLUN_FEATURE_READY, "Device backup."),
        feature("Cloud", "Cloud storage", BOLUN_FEATURE_READY, "User files."),
        feature("Cloud", "Settings sync", BOLUN_FEATURE_READY, "Settings roam."),
        feature("Cloud", "App sync", BOLUN_FEATURE_READY, "App list roam."),
        feature("Performance", "ARM NEON", BOLUN_FEATURE_READY, "SIMD optimization path."),
        feature("Performance", "Multithreading", BOLUN_FEATURE_READY, "Threading primitives."),
        feature("Performance", "Async I/O", BOLUN_FEATURE_READY, "Non-blocking I/O."),
        feature("Performance", "Caching", BOLUN_FEATURE_READY, "App and file cache."),
        feature("Performance", "Fast app launch", BOLUN_FEATURE_READY, "Preload and cache hints."),
        feature("Performance", "Memory optimization", BOLUN_FEATURE_READY, "Adaptive trimming."),
        feature("Extras", "Bolun Account", BOLUN_FEATURE_READY, "Account service."),
        feature("Extras", "Bolun Cloud", BOLUN_FEATURE_READY, "Cloud service."),
        feature("Extras", "Bolun Pay", BOLUN_FEATURE_READY, "Uses NFC plus QR on NFC devices, QR on non-NFC devices."),
        feature("Extras", "Bolun Share", BOLUN_FEATURE_READY, "Local sharing."),
        feature("Extras", "Bolun Link", BOLUN_FEATURE_READY, "Device linking."),
        feature("Extras", "Bolun Cast", BOLUN_FEATURE_READY, "Screen/media casting."),
        feature("Extras", "Mesh network", BOLUN_FEATURE_READY, "Device-to-device mesh."),
        feature("Extras", "P2P file exchange", BOLUN_FEATURE_READY, "Direct file sharing."),
        feature("Extras", "Voice assistant", BOLUN_FEATURE_READY, "Built-in assistant."),
        feature("Extras", "Notification center", BOLUN_FEATURE_READY, "Unified notifications."),
        feature("Extras", "Theme system", BOLUN_FEATURE_READY, "Theme packages."),
        feature("Extras", "Multiple users", BOLUN_FEATURE_READY, "User profiles."),
        feature("Extras", "Desktop mode", BOLUN_FEATURE_READY, "Monitor-attached desktop shell."),
        feature("Extras", "Android compatibility layer", BOLUN_FEATURE_PLANNED, "Optional future compatibility layer."),
        feature("Extras", "Modules and extensions", BOLUN_FEATURE_READY, "System extension model."),
        feature("Extras", "System service API", BOLUN_FEATURE_READY, "Create system services."),
        feature("Extras", "Full system rollback", BOLUN_FEATURE_READY, "OS update rollback metadata."),
    };

    for (size_t index = 0; index < sizeof(defaults) / sizeof(defaults[0]); ++index) {
        BolunResult result = add_feature(kernel, &defaults[index]);
        if (result != BOLUN_OK) {
            return result;
        }
    }
    return BOLUN_OK;
}


static int is_developer_tool_app(const char *app_id) {
    return strcmp(app_id, "developer-tools") == 0 || strcmp(app_id, "package-manager") == 0 || strcmp(app_id, "log-viewer") == 0 || strcmp(app_id, "api-explorer") == 0 || strcmp(app_id, "hardware-test") == 0 || strcmp(app_id, "system-diagnostics") == 0;
}

static BolunResult install_default_apps(BolunKernel *kernel) {
    const BolunAppDescriptor defaults[] = {
        app("phone", "Phone", "CALL", BOLUN_TILE_MEDIUM, 0xff1f78ff),
        app("contacts", "Contacts", "PEOPLE", BOLUN_TILE_MEDIUM, 0xff2672ec),
        app("messages", "Messages", "SMS", BOLUN_TILE_MEDIUM, 0xff9acd4f),
        app("mail", "Email", "MAIL", BOLUN_TILE_MEDIUM, 0xff0078d7),
        app("video-calls", "Video Calls", "VIDEO", BOLUN_TILE_MEDIUM, 0xff5c2d91),
        app("call-log", "Call Log", "LIST", BOLUN_TILE_SMALL, 0xff1f78ff),
        app("browser", "Browser", "WEB", BOLUN_TILE_MEDIUM, 0xff0078d7),
        app("youtube", "YouTube", "PLAY", BOLUN_TILE_MEDIUM, 0xffd13438),
        app("dima30-store", "Dima30 Store", "BAG", BOLUN_TILE_MEDIUM, 0xff188038),
        app("downloads", "Downloads", "DOWN", BOLUN_TILE_SMALL, 0xff2d7d9a),
        app("vpn", "VPN", "LOCK", BOLUN_TILE_SMALL, 0xff4b4b4b),
        app("camera", "Camera", "CAM", BOLUN_TILE_WIDE, 0xff315f8d),
        app("gallery", "Gallery", "PHOTO", BOLUN_TILE_WIDE, 0xff315f8d),
        app("video", "Video", "FILM", BOLUN_TILE_MEDIUM, 0xff2b579a),
        app("music", "Music", "NOTE", BOLUN_TILE_MEDIUM, 0xff2b579a),
        app("voice-recorder", "Voice Recorder", "MIC", BOLUN_TILE_SMALL, 0xff4b4b4b),
        app("fm-radio", "FM Radio", "RADIO", BOLUN_TILE_SMALL, 0xff107c10),
        app("notes", "Notes", "NOTE", BOLUN_TILE_MEDIUM, 0xffffb900),
        app("calendar", "Calendar", "DATE", BOLUN_TILE_MEDIUM, 0xff00b7c3),
        app("calculator", "Calculator", "CALC", BOLUN_TILE_SMALL, 0xff737373),
        app("clock", "Clock", "TIME", BOLUN_TILE_SMALL, 0xff2d2d30),
        app("alarm", "Alarm", "ALARM", BOLUN_TILE_SMALL, 0xff2d2d30),
        app("timer", "Timer", "TIMER", BOLUN_TILE_SMALL, 0xff2d2d30),
        app("stopwatch", "Stopwatch", "WATCH", BOLUN_TILE_SMALL, 0xff2d2d30),
        app("documents", "Documents", "DOC", BOLUN_TILE_MEDIUM, 0xff605e5c),
        app("pdf-reader", "PDF Reader", "PDF", BOLUN_TILE_MEDIUM, 0xffa4262c),
        app("file-manager", "File Manager", "FILE", BOLUN_TILE_MEDIUM, 0xff8764b8),
        app("archiver", "Archiver", "ZIP", BOLUN_TILE_SMALL, 0xff8764b8),
        app("recycle-bin", "Recycle Bin", "BIN", BOLUN_TILE_SMALL, 0xff605e5c),
        app("settings", "Settings", "GEAR", BOLUN_TILE_MEDIUM, 0xff2d2d30),
        app("system-update", "System Update", "OTA", BOLUN_TILE_MEDIUM, 0xff0078d7),
        app("security", "Security", "SHIELD", BOLUN_TILE_MEDIUM, 0xff107c10),
        app("b-id", "B-ID", "ID", BOLUN_TILE_SMALL, 0xff0078d7),
        app("bolun-account", "Bolun Account", "USER", BOLUN_TILE_SMALL, 0xff0078d7),
        app("backup", "Backup", "CLOUD", BOLUN_TILE_SMALL, 0xff0078d7),
        app("restore", "Restore", "RESTORE", BOLUN_TILE_SMALL, 0xff0078d7),
        app("app-permissions", "App Permissions", "PERM", BOLUN_TILE_SMALL, 0xff107c10),
        app("storage", "Storage", "DISK", BOLUN_TILE_SMALL, 0xff605e5c),
        app("battery", "Battery", "BAT", BOLUN_TILE_SMALL, 0xff107c10),
        app("memory", "Memory", "RAM", BOLUN_TILE_SMALL, 0xff605e5c),
        app("network", "Network", "NET", BOLUN_TILE_SMALL, 0xff0078d7),
        app("bluetooth", "Bluetooth", "BT", BOLUN_TILE_SMALL, 0xff0078d7),
        app("wifi", "Wi-Fi", "WIFI", BOLUN_TILE_SMALL, 0xff0078d7),
        app("about", "About Device", "INFO", BOLUN_TILE_SMALL, 0xff605e5c),
        app("maps", "Maps", "MAP", BOLUN_TILE_MEDIUM, 0xff315f8d),
        app("navigator", "Navigator", "NAV", BOLUN_TILE_MEDIUM, 0xff315f8d),
        app("compass", "Compass", "COMP", BOLUN_TILE_SMALL, 0xff315f8d),
        app("weather", "Weather", "SUN", BOLUN_TILE_WIDE, 0xff0078d7),
        app("bolun-ai", "Bolun AI", "AI", BOLUN_TILE_WIDE, 0xff5c2d91),
        app("ai-camera", "AI Camera", "AICAM", BOLUN_TILE_MEDIUM, 0xff5c2d91),
        app("ai-translator", "AI Translator", "TR", BOLUN_TILE_MEDIUM, 0xff5c2d91),
        app("ai-ocr", "AI OCR", "OCR", BOLUN_TILE_SMALL, 0xff5c2d91),
        app("ai-search", "AI Search", "SEARCH", BOLUN_TILE_SMALL, 0xff5c2d91),
        app("ai-photo-editor", "AI Photo Editor", "EDIT", BOLUN_TILE_MEDIUM, 0xff5c2d91),
        app("bolun-share", "Bolun Share", "SHARE", BOLUN_TILE_MEDIUM, 0xff00b7c3),
        app("bolun-link", "Bolun Link", "LINK", BOLUN_TILE_MEDIUM, 0xff00b7c3),
        app("bolun-cast", "Bolun Cast", "CAST", BOLUN_TILE_MEDIUM, 0xff00b7c3),
        app("passwords", "Passwords", "KEY", BOLUN_TILE_SMALL, 0xff107c10),
        app("certificates", "Certificates", "CERT", BOLUN_TILE_SMALL, 0xff107c10),
        app("app-check", "App Check", "CHECK", BOLUN_TILE_SMALL, 0xff107c10),
        app("bolun-defender", "Bolun Defender", "DEF", BOLUN_TILE_MEDIUM, 0xff107c10),
        app("keyboard", "Keyboard", "KBD", BOLUN_TILE_SMALL, 0xff605e5c),
        app("wallpapers", "Wallpapers", "WALL", BOLUN_TILE_SMALL, 0xff315f8d),
        app("themes", "Themes", "THEME", BOLUN_TILE_SMALL, 0xff315f8d),
        app("emoji", "Emoji", "EMOJI", BOLUN_TILE_SMALL, 0xffffb900),
        app("qr-scanner", "QR Scanner", "QR", BOLUN_TILE_SMALL, 0xff107c10),
        app("document-scanner", "Document Scanner", "SCAN", BOLUN_TILE_SMALL, 0xff107c10),
        app("notification-center", "Notification Center", "BELL", BOLUN_TILE_MEDIUM, 0xff0078d7),
        app("control-center", "Control Center", "CTRL", BOLUN_TILE_MEDIUM, 0xff0078d7),
        app("voice-assistant", "Voice Assistant", "VOICE", BOLUN_TILE_MEDIUM, 0xff5c2d91),
        app("search", "Search", "FIND", BOLUN_TILE_MEDIUM, 0xff0078d7),
        app("widgets", "Widgets", "WID", BOLUN_TILE_MEDIUM, 0xff00b7c3),
        app("live-tiles", "Live Tiles", "TILE", BOLUN_TILE_MEDIUM, 0xff00b7c3),
        app("emergency", "Emergency Help", "SOS", BOLUN_TILE_SMALL, 0xffd13438),
        app("feedback", "Feedback", "FB", BOLUN_TILE_SMALL, 0xff0078d7),
        app("licenses", "Licenses", "LIC", BOLUN_TILE_SMALL, 0xff605e5c),
        app("user-guide", "User Guide", "HELP", BOLUN_TILE_SMALL, 0xff605e5c),
        app("developer-tools", "Developer Tools", "DEV", BOLUN_TILE_MEDIUM, 0xff2d2d30),
        app("package-manager", "Package Manager", "PKG", BOLUN_TILE_SMALL, 0xff2d2d30),
        app("log-viewer", "Log Viewer", "LOG", BOLUN_TILE_SMALL, 0xff2d2d30),
        app("api-explorer", "API Explorer", "API", BOLUN_TILE_SMALL, 0xff2d2d30),
        app("hardware-test", "Hardware Test", "HW", BOLUN_TILE_SMALL, 0xff2d2d30),
        app("system-diagnostics", "System Diagnostics", "DIAG", BOLUN_TILE_SMALL, 0xff2d2d30),
    };

    for (size_t index = 0; index < sizeof(defaults) / sizeof(defaults[0]); ++index) {
        if (!kernel->device_profile.developer_mode_enabled && is_developer_tool_app(defaults[index].app_id)) {
            continue;
        }
        BolunResult result = bolun_kernel_install_app(kernel, &defaults[index]);
        if (result != BOLUN_OK) {
            return result;
        }
    }
    return BOLUN_OK;
}

static BolunDeviceProfile make_device_profile(const char *codename, const char *device_name, uint8_t developer_mode_enabled) {
    BolunDeviceProfile profile;
    memset(&profile, 0, sizeof(profile));
    const char *normalized_codename = (codename == NULL || codename[0] == '\0') ? "bolun-play" : codename;
    copy_text(profile.codename, sizeof(profile.codename), normalized_codename);
    copy_text(profile.display_name, sizeof(profile.display_name), device_name);
    if (contains_ascii_case_insensitive(normalized_codename, "lumia")) {
        copy_text(profile.target_family, sizeof(profile.target_family), "Lumia-compatible ARM phone target");
    } else if (contains_ascii_case_insensitive(normalized_codename, "redmi-9a")) {
        copy_text(profile.target_family, sizeof(profile.target_family), "Redmi 9A ARM phone target");
    } else if (contains_ascii_case_insensitive(normalized_codename, "redmi-9c")) {
        copy_text(profile.target_family, sizeof(profile.target_family), "Redmi 9C ARM phone target");
    } else {
        copy_text(profile.target_family, sizeof(profile.target_family), "Bolun Play ARM phone target");
    }
    profile.nfc_available = contains_ascii_case_insensitive(normalized_codename, "nfc") || contains_ascii_case_insensitive(normalized_codename, "bolun-play");
    profile.qr_code_available = 1;
    profile.secure_boot_required = 1;
    profile.developer_mode_enabled = developer_mode_enabled;
    return profile;
}

const char *bolun_result_message(BolunResult result) {
    switch (result) {
    case BOLUN_OK:
        return "ok";
    case BOLUN_ERROR_INVALID_ARGUMENT:
        return "invalid argument";
    case BOLUN_ERROR_NOT_BOOTED:
        return "kernel is not booted";
    case BOLUN_ERROR_NOT_FOUND:
        return "not found";
    case BOLUN_ERROR_FULL:
        return "storage is full";
    default:
        return "internal error";
    }
}

BolunKernelConfig bolun_kernel_default_config(void) {
    BolunKernelConfig config;
    config.device_name = "Bolun Phone";
    config.company = "dima30";
    config.default_accent_argb = 0xff0078d7;
    config.device_codename = "bolun-play";
    config.developer_mode_enabled = 0;
    return config;
}

BolunResult bolun_kernel_create(const BolunKernelConfig *config, BolunKernel **out_kernel) {
    if (out_kernel == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    BolunKernel *kernel = calloc(1, sizeof(BolunKernel));
    if (kernel == NULL) {
        return BOLUN_ERROR_INTERNAL;
    }
    BolunKernelConfig default_config = bolun_kernel_default_config();
    if (config == NULL) {
        config = &default_config;
    }
    kernel->config = *config;
    copy_text(kernel->device_name, sizeof(kernel->device_name), config->device_name == NULL ? default_config.device_name : config->device_name);
    copy_text(kernel->company, sizeof(kernel->company), config->company == NULL ? default_config.company : config->company);
    kernel->device_profile = make_device_profile(config->device_codename, kernel->device_name, config->developer_mode_enabled);
    kernel->next_notification_sequence = 1;
    kernel->next_message_sequence = 1;
    *out_kernel = kernel;
    return BOLUN_OK;
}

void bolun_kernel_destroy(BolunKernel *kernel) {
    free(kernel);
}

BolunResult bolun_kernel_boot(BolunKernel *kernel) {
    if (kernel == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (!kernel->booted) {
        BolunResult result = install_default_features(kernel);
        if (result != BOLUN_OK) {
            return result;
        }
        result = install_default_apps(kernel);
        if (result != BOLUN_OK) {
            return result;
        }
        BolunNotificationDescriptor notification;
        memset(&notification, 0, sizeof(notification));
        copy_text(notification.app_id, sizeof(notification.app_id), "system");
        copy_text(notification.title, sizeof(notification.title), "Welcome");
        snprintf(notification.body, sizeof(notification.body), "%s by %s | %s | cpu=0x%llx | NFC=%s QR=%s", kernel->device_name, kernel->company, bolun_boot_banner(), (unsigned long long)bolun_cpu_probe(), kernel->device_profile.nfc_available ? "yes" : "no", kernel->device_profile.qr_code_available ? "yes" : "no");
        bolun_kernel_push_notification(kernel, &notification);
        kernel->booted = 1;
    }
    return BOLUN_OK;
}

BolunResult bolun_kernel_shutdown(BolunKernel *kernel) {
    if (kernel == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    kernel->booted = 0;
    return BOLUN_OK;
}

BolunResult bolun_kernel_get_status(const BolunKernel *kernel, BolunSystemStatus *out_status) {
    if (kernel == NULL || out_status == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    memset(out_status, 0, sizeof(*out_status));
    copy_text(out_status->device_name, sizeof(out_status->device_name), kernel->device_name);
    copy_text(out_status->company, sizeof(out_status->company), kernel->company);
    out_status->booted = kernel->booted;
    out_status->app_count = kernel->app_count;
    out_status->tile_count = kernel->tile_count;
    out_status->feature_count = kernel->feature_count;
    out_status->nfc_available = kernel->device_profile.nfc_available;
    out_status->qr_code_available = kernel->device_profile.qr_code_available;
    for (size_t index = 0; index < kernel->notification_count; ++index) {
        if (kernel->notifications[index].unread) {
            ++out_status->unread_notifications;
        }
    }
    return BOLUN_OK;
}

BolunResult bolun_kernel_install_app(BolunKernel *kernel, const BolunAppDescriptor *app_descriptor) {
    if (kernel == NULL || app_descriptor == NULL || app_descriptor->app_id[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (kernel->app_count >= BOLUN_MAX_APPS || kernel->tile_count >= BOLUN_MAX_TILES) {
        return BOLUN_ERROR_FULL;
    }
    for (size_t index = 0; index < kernel->app_count; ++index) {
        if (strcmp(kernel->apps[index].app_id, app_descriptor->app_id) == 0) {
            return BOLUN_ERROR_INVALID_ARGUMENT;
        }
    }
    kernel->apps[kernel->app_count++] = *app_descriptor;
    BolunTileDescriptor tile;
    memset(&tile, 0, sizeof(tile));
    copy_text(tile.app_id, sizeof(tile.app_id), app_descriptor->app_id);
    copy_text(tile.title, sizeof(tile.title), app_descriptor->title);
    copy_text(tile.glyph, sizeof(tile.glyph), app_descriptor->capability);
    tile.size = app_descriptor->preferred_tile_size;
    tile.accent_argb = app_descriptor->accent_argb;
    kernel->tiles[kernel->tile_count++] = tile;
    return BOLUN_OK;
}

BolunResult bolun_kernel_launch_app(BolunKernel *kernel, const char *app_id, char *out_message, size_t out_message_size) {
    if (kernel == NULL || app_id == NULL || out_message == NULL || out_message_size == 0) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (!kernel->booted) {
        return BOLUN_ERROR_NOT_BOOTED;
    }
    for (size_t index = 0; index < kernel->app_count; ++index) {
        if (strcmp(kernel->apps[index].app_id, app_id) == 0) {
            snprintf(out_message, out_message_size, "%s is ready on %s", kernel->apps[index].title, kernel->device_name);
            return BOLUN_OK;
        }
    }
    return BOLUN_ERROR_NOT_FOUND;
}

BolunResult bolun_kernel_list_tiles(const BolunKernel *kernel, BolunTileDescriptor *out_tiles, size_t capacity, size_t *out_count) {
    if (kernel == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_count = kernel->tile_count;
    if (out_tiles == NULL) {
        return BOLUN_OK;
    }
    size_t count = kernel->tile_count < capacity ? kernel->tile_count : capacity;
    memcpy(out_tiles, kernel->tiles, count * sizeof(BolunTileDescriptor));
    return BOLUN_OK;
}

BolunResult bolun_kernel_list_features(const BolunKernel *kernel, BolunFeatureDescriptor *out_features, size_t capacity, size_t *out_count) {
    if (kernel == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_count = kernel->feature_count;
    if (out_features == NULL) {
        return BOLUN_OK;
    }
    size_t count = kernel->feature_count < capacity ? kernel->feature_count : capacity;
    memcpy(out_features, kernel->features, count * sizeof(BolunFeatureDescriptor));
    return BOLUN_OK;
}

BolunResult bolun_kernel_get_device_profile(const BolunKernel *kernel, BolunDeviceProfile *out_profile) {
    if (kernel == NULL || out_profile == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_profile = kernel->device_profile;
    return BOLUN_OK;
}

BolunResult bolun_kernel_evaluate_store_removal(const BolunKernel *kernel, const char *app_id, const char *server_reason, uint8_t security_removal, BolunStoreAppRemovalPolicy *out_policy) {
    if (kernel == NULL || app_id == NULL || app_id[0] == '\0' || out_policy == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    memset(out_policy, 0, sizeof(*out_policy));
    copy_text(out_policy->app_id, sizeof(out_policy->app_id), app_id);
    copy_text(out_policy->reason, sizeof(out_policy->reason), server_reason == NULL || server_reason[0] == '\0' ? "Dima30 Store removed this app without an additional server reason." : server_reason);
    out_policy->removed_from_store = 1;
    out_policy->security_removal = security_removal ? 1 : 0;
    out_policy->installed_copy_kept = 1;
    out_policy->launch_allowed = security_removal ? 0 : 1;
    return BOLUN_OK;
}

BolunResult bolun_kernel_push_notification(BolunKernel *kernel, const BolunNotificationDescriptor *notification) {
    if (kernel == NULL || notification == NULL || notification->title[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (kernel->notification_count >= BOLUN_MAX_NOTIFICATIONS) {
        return BOLUN_ERROR_FULL;
    }
    BolunNotificationDescriptor stored = *notification;
    stored.sequence = kernel->next_notification_sequence++;
    stored.unread = 1;
    kernel->notifications[kernel->notification_count++] = stored;
    return BOLUN_OK;
}

BolunResult bolun_kernel_list_notifications(const BolunKernel *kernel, BolunNotificationDescriptor *out_notifications, size_t capacity, size_t *out_count) {
    if (kernel == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_count = kernel->notification_count;
    if (out_notifications != NULL) {
        size_t count = kernel->notification_count < capacity ? kernel->notification_count : capacity;
        memcpy(out_notifications, kernel->notifications, count * sizeof(BolunNotificationDescriptor));
    }
    return BOLUN_OK;
}

BolunResult bolun_kernel_add_contact(BolunKernel *kernel, const BolunContactDescriptor *contact) {
    if (kernel == NULL || contact == NULL || contact->name[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (kernel->contact_count >= BOLUN_MAX_CONTACTS) {
        return BOLUN_ERROR_FULL;
    }
    kernel->contacts[kernel->contact_count++] = *contact;
    return BOLUN_OK;
}

BolunResult bolun_kernel_send_message(BolunKernel *kernel, const BolunMessageDescriptor *message) {
    if (kernel == NULL || message == NULL || message->sender[0] == '\0' || message->recipient[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (kernel->message_count >= BOLUN_MAX_MESSAGES) {
        return BOLUN_ERROR_FULL;
    }
    BolunMessageDescriptor stored = *message;
    stored.sequence = kernel->next_message_sequence++;
    stored.delivered = 1;
    kernel->messages[kernel->message_count++] = stored;
    return BOLUN_OK;
}

BolunResult bolun_kernel_list_messages(const BolunKernel *kernel, BolunMessageDescriptor *out_messages, size_t capacity, size_t *out_count) {
    if (kernel == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_count = kernel->message_count;
    if (out_messages != NULL) {
        size_t count = kernel->message_count < capacity ? kernel->message_count : capacity;
        memcpy(out_messages, kernel->messages, count * sizeof(BolunMessageDescriptor));
    }
    return BOLUN_OK;
}
