#ifndef USB_HOTPLUG_WATCHER_HPP
#define USB_HOTPLUG_WATCHER_HPP

#include <libusb-1.0/libusb.h>

#include <atomic>
#include <functional>
#include <thread>

class UsbHotplugWatcher {
   public:
    enum class Event {
        Arrived,
        Left,
    };

    // Runs on the watcher's thread, inside libusb's event handling. It must return quickly and must
    // not open or interact with the device there.
    using Callback = std::function<void(Event)>;

    explicit UsbHotplugWatcher(Callback callback);
    ~UsbHotplugWatcher();

    UsbHotplugWatcher(const UsbHotplugWatcher&) = delete;
    UsbHotplugWatcher& operator=(const UsbHotplugWatcher&) = delete;

    // False if hotplug events are not available.
    [[nodiscard]] bool is_active() const;

   private:
    static int LIBUSB_CALL on_hotplug_event(libusb_context* context, libusb_device* device, libusb_hotplug_event event, void* user_data);
    void handle_events();

    Callback callback_;
    libusb_context* context_ = nullptr;
    libusb_hotplug_callback_handle callback_handle_{};
    std::atomic_bool running_{false};
    std::thread thread_;
};

#endif  // USB_HOTPLUG_WATCHER_HPP
