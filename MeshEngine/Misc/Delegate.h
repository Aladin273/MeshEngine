#pragma once

#include <functional>
#include <vector>
#include <algorithm>

namespace MeshEngine
{
    template <typename... Args>
    class Delegate 
    {
    public:
        using Callback = std::function<void(Args...)>;

        void add(const Callback& callback)
        {
            m_callbacks.push_back(callback);
        }

        void addUnique(const Callback& callback)
        {
            if (std::none_of(m_callbacks.begin(), m_callbacks.end(), [&](const Callback& addedCallback) 
                {
                    return addedCallback.target_type() == callback.target_type();
                }))
            {
                m_callbacks.push_back(callback);
            }
        }

        template <typename T>
        void add(T* instance, void (T::* method)(Args...))
        {
            add(std::bind(method, instance, std::placeholders::_1, std::placeholders::_2));
        }

        template <typename T>
        void addUnique(T* instance, void (T::* method)(Args...))
        {
            Callback bound = std::bind(method, instance, std::placeholders::_1, std::placeholders::_2);
            addUnique(bound);
        }

        void remove(const Callback& callback)
        {
            m_callbacks.erase(std::remove(m_callbacks.begin(), m_callbacks.end(), callback), m_callbacks.end());
        }

        template <typename T>
        void remove(T* instance, void (T::* method)(Args...))
        {
            Callback bound = std::bind(method, instance, std::placeholders::_1, std::placeholders::_2);
            remove(bound);
        }

        void broadcast(Args... args)
        {
            for (const auto& callback : m_callbacks)
            {
                if (callback)
                {
                    callback(args...);
                }
            }
        }

        void clear()
        {
            m_callbacks.clear();
        }

    private:
        std::vector<Callback> m_callbacks;
    };
}