#pragma once

#include "spdlog/sinks/base_sink.h"
#include "spdlog/details/null_mutex.h"
#include "spdlog/details/log_msg_buffer.h"

#include <mutex>
#include <string>
#include <vector>

namespace spdlog {
namespace sinks {

template <typename Mutex>
class mesh_engine_sink final : public base_sink<Mutex>
{
public:
    explicit mesh_engine_sink() {}

    bool scrollToBottom = false;

    std::vector<std::string>& formatted()
    {
        std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);

        return i_;
    }

    void clear()
    {
        std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);

        i_.clear();
    }

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        memory_buf_t formatted;
        this->formatter_->format(msg, formatted);
        i_.push_back(fmt::to_string(formatted));
        scrollToBottom = true;
    }

    void flush_() override {}

private:
    std::vector<std::string> i_;
};

using mesh_engine_sink_mt = mesh_engine_sink<std::mutex>;
using mesh_engine_sink_st = mesh_engine_sink<details::null_mutex>;

}
}