#include "dataset_spectrum.hpp"

#include <QVector>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>

constexpr uint64_t MHZ_TO_HZ = 1'000'000ULL;

template <class T> std::vector<uint64_t> linspace(T start, T stop, int num, bool endpoint = true) {
    // Return an empty vector if num is non-positive.
    if (num <= 0) {
        return std::vector<T>{};
    }

    // Special case for a single element.
    if (num == 1) {
        return std::vector<T>{static_cast<T>(start)};
    }

    std::vector<T> result(num);
    double step_size = endpoint ? (stop - start) / (num - 1) : (stop - start) / num;

    for (int i = 0; i < num; ++i) {
        result[i] = static_cast<T>(start + i * step_size);
    }

    return result;
}

DatasetSpectrum::DatasetSpectrum() : initialized(false) {}

DatasetSpectrum::DatasetSpectrum(double fft_bin_size_hz, std::vector<uint16_t> freq_ranges)
    : fft_bin_size_hz(fft_bin_size_hz), freq_ranges(freq_ranges), initialized(true) {}

int DatasetSpectrum::get_num_datapoints() const {
    if (freq_ranges.empty()) {
        return 0;
    }

    int datapoints = 0;
    for (size_t i = 0; i < freq_ranges.size(); i += 2) {
        datapoints += (freq_ranges[i + 1] - freq_ranges[i]) / (fft_bin_size_hz / 1e6);
    }
    return datapoints;
}

int DatasetSpectrum::get_total_num_datapoints() const {
    if (freq_ranges.empty()) {
        return 0;
    }

    // From the lowest start to the highest end, whatever order the ranges are in.
    uint16_t lowest_mhz = freq_ranges[0];
    uint16_t highest_mhz = freq_ranges[1];
    for (size_t i = 0; i + 1 < freq_ranges.size(); i += 2) {
        lowest_mhz = std::min(lowest_mhz, freq_ranges[i]);
        highest_mhz = std::max(highest_mhz, freq_ranges[i + 1]);
    }

    if (highest_mhz <= lowest_mhz) {
        return 0;
    }

    // Rounded, not truncated: the bin width is not a whole number of Hz.
    return static_cast<int>(std::lround((highest_mhz - lowest_mhz) * 1e6 / fft_bin_size_hz));
}

int DatasetSpectrum::get_num_ranges() const {
    return static_cast<int>(freq_ranges.size() / 2);
}

const std::vector<uint16_t>& DatasetSpectrum::get_freq_ranges() const {
    return freq_ranges;
}

void DatasetSpectrum::add_new_data(uint64_t start_freq, uint64_t end_freq,
                                   const std::vector<float>& pwr) {
    std::vector<uint64_t> freqs = linspace<uint64_t>(start_freq, end_freq, pwr.size(), false);

    for (size_t i = 0; i < pwr.size(); ++i) {
        spectrum[freqs[i]] = pwr[i];
    }
}

std::map<uint64_t, float> DatasetSpectrum::get_spectrum() const {
    return spectrum;
}

QVector<double> DatasetSpectrum::get_power_array(int range) const {
    QVector<double> spectrum_vec;

    if (range < 0 || range >= get_num_ranges()) {
        return spectrum_vec;
    }

    const auto end = spectrum.lower_bound(freq_ranges[range * 2 + 1] * MHZ_TO_HZ);
    for (auto it = spectrum.lower_bound(freq_ranges[range * 2] * MHZ_TO_HZ); it != end; ++it) {
        spectrum_vec.push_back(it->second);
    }

    return spectrum_vec;
}

QVector<double> DatasetSpectrum::get_frequency_array(int range) const {
    QVector<double> freq_vec;

    if (range < 0 || range >= get_num_ranges()) {
        return freq_vec;
    }

    const auto end = spectrum.lower_bound(freq_ranges[range * 2 + 1] * MHZ_TO_HZ);
    for (auto it = spectrum.lower_bound(freq_ranges[range * 2] * MHZ_TO_HZ); it != end; ++it) {
        freq_vec.push_back(it->first / 1e6);
    }

    return freq_vec;
}

void DatasetSpectrum::clear() {
    spectrum.clear();
}

bool DatasetSpectrum::is_initialized() const {
    return initialized;
}
