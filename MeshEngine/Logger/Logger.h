#pragma once

#include <spdlog/spdlog.h>

#include "spdlog/sinks/base_sink.h"
#include <spdlog/sinks/daily_file_sink.h>

#include "spdlog/details/null_mutex.h"
#include "spdlog/details/log_msg_buffer.h"

#include <mutex>
#include <string>
#include <vector>
#include <memory>

namespace MeshEngine {
    namespace Logger {

        // Custom Sinks

        template <typename Mutex>
        class ringbuffer_sink final : public spdlog::sinks::base_sink<Mutex>
        {
        public:
            explicit ringbuffer_sink() {}

            std::vector<std::string>& formatted()
            {
                std::lock_guard<Mutex> lock(spdlog::sinks::base_sink<Mutex>::mutex_);

                return i_;
            }

            bool updated()
            {
                if (u_)
                {
                    u_ = false;
                    return true;
                }

                return u_;
            }

            void clear()
            {
                std::lock_guard<Mutex> lock(spdlog::sinks::base_sink<Mutex>::mutex_);

                i_.clear();
            }

        protected:
            void sink_it_(const spdlog::details::log_msg& msg) override
            {
                spdlog::memory_buf_t formatted;
                this->formatter_->format(msg, formatted);
                i_.push_back(fmt::to_string(formatted));
                u_ = true;
            }

            void flush_() override {}

        private:
            std::vector<std::string> i_;
            bool u_ = false;
        };

        using ringbuffer_sink_mt = ringbuffer_sink<std::mutex>;
        using ringbuffer_sink_st = ringbuffer_sink<spdlog::details::null_mutex>;

        using daily_file_sink_mt = spdlog::sinks::daily_file_sink_mt;
        using daily_file_sink_st = spdlog::sinks::daily_file_sink_st;

        // General

        inline void init(const std::string& filename = "Logs/MeshEngine.txt", uint32_t startHour = 23, uint32_t startMinute = 55) 
        {
            auto ringLogger = std::make_shared<ringbuffer_sink_mt>();
            auto fileLogger = std::make_shared<daily_file_sink_st>(filename, startHour, startMinute);

            spdlog::default_logger()->sinks().push_back(ringLogger);
            spdlog::default_logger()->sinks().push_back(fileLogger);
        }

        inline std::shared_ptr<MeshEngine::Logger::ringbuffer_sink_mt> ringbuffer_sink()
        {
            auto it = std::find_if(spdlog::default_logger()->sinks().begin(), spdlog::default_logger()->sinks().end(), [](const auto& sink)
                {
                    return dynamic_cast<MeshEngine::Logger::ringbuffer_sink_mt*>(sink.get()) != nullptr;
                });

            return std::dynamic_pointer_cast<MeshEngine::Logger::ringbuffer_sink_mt>(*it);
        }

        inline std::shared_ptr<MeshEngine::Logger::daily_file_sink_st> daily_file_sink()
        {
            auto it = std::find_if(spdlog::default_logger()->sinks().begin(), spdlog::default_logger()->sinks().end(), [](const auto& sink)
                {
                    return dynamic_cast<MeshEngine::Logger::daily_file_sink_st*>(sink.get()) != nullptr;
                });

            return std::dynamic_pointer_cast<MeshEngine::Logger::daily_file_sink_st>(*it);
        }

        // UTF-8 Logging Functions

        template <typename... Args>
        inline void trace(fmt::format_string<Args...> fmt, Args&&... args) 
        {
            spdlog::trace(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void debug(fmt::format_string<Args...> fmt, Args&&... args) 
        {
            spdlog::debug(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void info(fmt::format_string<Args...> fmt, Args&&... args) 
        {
            spdlog::info(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void warn(fmt::format_string<Args...> fmt, Args&&... args) 
        {
            spdlog::warn(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void error(fmt::format_string<Args...> fmt, Args&&... args) 
        {
            spdlog::error(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void critical(fmt::format_string<Args...> fmt, Args&&... args) 
        {
            spdlog::critical(fmt, std::forward<Args>(args)...);
        }

        // Template Logging Functions

        template <typename T>
        inline void trace(const T& msg) 
        {
            spdlog::trace(msg);
        }

        template <typename T>
        inline void debug(const T& msg) 
        {
            spdlog::debug(msg);
        }

        template <typename T>
        inline void info(const T& msg) 
        {
            spdlog::info(msg);
        }

        template <typename T>
        inline void warn(const T& msg) 
        {
            spdlog::warn(msg);
        }

        template <typename T>
        inline void error(const T& msg) 
        {
            spdlog::error(msg);
        }

        template <typename T>
        inline void critical(const T& msg) 
        {
            spdlog::critical(msg);
        }

// WCHAR Logging Functions
#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT

        template <typename... Args>
        inline void trace(fmt::wformat_string<Args...> fmt, Args&&... args) 
        {
            spdlog::trace(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void debug(fmt::wformat_string<Args...> fmt, Args&&... args) 
        {
            spdlog::debug(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void info(fmt::wformat_string<Args...> fmt, Args&&... args) 
        {
            spdlog::info(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void warn(fmt::wformat_string<Args...> fmt, Args&&... args) 
        {
            spdlog::warn(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void error(fmt::wformat_string<Args...> fmt, Args&&... args) 
        {
            spdlog::error(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void critical(fmt::wformat_string<Args...> fmt, Args&&... args) 
        {
            spdlog::critical(fmt, std::forward<Args>(args)...);
        }
#endif

    } // namespace Logger
} // namespace MeshEngine
