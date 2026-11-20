#include <QApplication>
#include <QMetaObject>

#include "hackrf_controller.hpp"
#include "main_window.hpp"
#include "usb_hotplug_watcher.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    hackrf_init();

    int ret = 0;
    {
        HackRFController controller;
        controller.set_scan_ranges({{2000, 2700}}); // Default 2 GHz to 2.7 GHz

        const bool connected = controller.connect_device();

        // Set even without a device, so that it also applies to one that is plugged in later.
        controller.set_gain_state(HackRFGainState{false, 24, 0});

        if (connected) {
            controller.start_sweep();
        }

        MainWindow main_window(&controller);

        // Declared after the window so that it is destroyed first and stops sending it events.
        UsbHotplugWatcher hotplug_watcher([&main_window](UsbHotplugWatcher::Event event) {
            // This runs on the libusb thread: only pass the event on to the GUI thread.
            QMetaObject::invokeMethod(
                &main_window,
                [&main_window, event]() {
                    if (event == UsbHotplugWatcher::Event::Arrived) {
                        main_window.device_arrived();
                    } else {
                        main_window.device_left();
                    }
                },
                Qt::QueuedConnection);
        });

        main_window.showMaximized();

        ret = app.exec();

        if (controller.is_connected()) {
            controller.stop_sweep();
        }
    } // the controller closes the device here, before hackrf_exit()

    hackrf_exit();

    return ret;
}
