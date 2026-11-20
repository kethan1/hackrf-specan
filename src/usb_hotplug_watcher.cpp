#include "usb_hotplug_watcher.hpp"

#include <libhackrf/hackrf.h>
#include <libusb-1.0/libusb.h>

#include <iostream>
#include <utility>

namespace {

constexpr int HACKRF_VENDOR_ID = 0x1d50;

bool is_hackrf_product(uint16_t product_id) {
    return product_id == USB_BOARD_ID_JAWBREAKER ||
           product_id == USB_BOARD_ID_HACKRF_ONE ||
           product_id == USB_BOARD_ID_RAD1O;
}

}  // namespace

UsbHotplugWatcher::UsbHotplugWatcher(Callback callback) : callback_(std::move(callback)) {
    int rc = libusb_init(&context_);
    if (rc != LIBUSB_SUCCESS) {
        std::cerr << "Failed to initialize libusb: " << rc << "\n";
        context_ = nullptr;
        return;
    }

    if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        std::cerr << "USB hotplug events are not supported on this platform\n";
        libusb_exit(context_);
        context_ = nullptr;
        return;
    }

    rc = libusb_hotplug_register_callback(
        context_,
        static_cast<libusb_hotplug_event>(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
        static_cast<libusb_hotplug_flag>(0),
        HACKRF_VENDOR_ID, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
        &UsbHotplugWatcher::on_hotplug_event, this, &callback_handle_);

    if (rc != LIBUSB_SUCCESS) {
        std::cerr << "Error creating a hotplug callback: " << rc << "\n";
        libusb_exit(context_);
        context_ = nullptr;
        return;
    }

    running_ = true;
    thread_ = std::thread([this]() { handle_events(); });
}

UsbHotplugWatcher::~UsbHotplugWatcher() {
    if (!context_) {
        return;
    }

    running_ = false;
    libusb_hotplug_deregister_callback(context_, callback_handle_);
    libusb_interrupt_event_handler(context_);
    thread_.join();

    libusb_exit(context_);
}

bool UsbHotplugWatcher::is_active() const {
    return context_ != nullptr;
}

void UsbHotplugWatcher::handle_events() {
    while (running_) {
        timeval timeout{1, 0};
        libusb_handle_events_timeout_completed(context_, &timeout, nullptr);
    }
}

int LIBUSB_CALL UsbHotplugWatcher::on_hotplug_event(libusb_context* /*context*/, libusb_device* device,
                                                    libusb_hotplug_event event, void* user_data) {
    libusb_device_descriptor descriptor{};
    if (libusb_get_device_descriptor(device, &descriptor) != LIBUSB_SUCCESS ||
        !is_hackrf_product(descriptor.idProduct)) {
        return 0;
    }

    auto* watcher = static_cast<UsbHotplugWatcher*>(user_data);
    if (watcher->callback_) {
        watcher->callback_(event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED ? Event::Arrived : Event::Left);
    }

    return 0;  // stay registered
}
