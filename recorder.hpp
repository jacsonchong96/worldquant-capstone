
#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <filesystem>

class Recorder {
public:
    Recorder(const std::string& out_dir, const std::string& filename) {
        namespace fs = std::filesystem;
        if (!fs::exists(out_dir)) fs::create_directories(out_dir);
        file_path_ = out_dir + "/" + filename;
        out_.open(file_path_, std::ios::out | std::ios::app);
        if (out_.tellp() == 0) {
            out_ << "ts_recv_ns,ws_idx,raw\n"; // the CSV header
        }
    }

    ~Recorder() { if (out_.is_open()) out_.close(); }

    void append(const std::string& raw_msg, long long ts_ns, int ws_idx) {
        std::lock_guard<std::mutex> lk(mu_);
        std::string safe = raw_msg;
        std::replace(safe.begin(), safe.end(), '\n', ' ');
        std::replace(safe.begin(), safe.end(), ',', ';'); // do not do any CSV breaking
        out_ << ts_ns << "," << ws_idx << ",\"" << safe << "\"\n";
    }

    const std::string& path() const { 
        return file_path_; }

private:
    std::string file_path_;
    std::ofstream out_;
    std::mutex mu_;
};
