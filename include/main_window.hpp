#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <qwt_color_map.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_spectrogram.h>

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <vector>

#include "dataset_spectrum.hpp"
#include "hackrf_controller.hpp"
#include "waterfall_raster_data.hpp"

constexpr int COLOR_MAP_SAMPLES = 300;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(HackRFController* controller, QWidget* parent = nullptr);

    // USB hotplug notifications. Should be called on the GUI thread.
    void device_arrived();
    void device_left();

  private:
    QwtPlot* custom_plot_ = nullptr;
    std::vector<QwtPlotCurve*> curves_;

    QwtPlot* color_plot_ = nullptr;
    QwtPlotSpectrogram* color_map_ = nullptr;
    WaterfallRasterData* raster_data_ = nullptr;

    DatasetSpectrum dataset_spectrum_;
    HackRFController* controller_ = nullptr;

    QLabel* connection_label_ = nullptr;

    // Gain controls
    QLineEdit* total_gain_field_ = nullptr;

    // Scan range controls
    QListWidget* range_list_ = nullptr;
    QSpinBox* start_freq_spin_ = nullptr;
    QSpinBox* end_freq_spin_ = nullptr;
    QPushButton* add_range_btn_ = nullptr;
    QPushButton* remove_range_btn_ = nullptr;
    QPushButton* apply_ranges_btn_ = nullptr;

    void update_plot(const FFTSweepData& data);
    void update_total_gain();
    void setup_sidebar(QWidget* sidebar);
    void refresh_range_list();
    void add_scan_range();
    void remove_selected_range();
    void apply_scan_ranges();
    void connect_to_device();
    void update_connection_status();
};

#endif // MAIN_WINDOW_HPP
