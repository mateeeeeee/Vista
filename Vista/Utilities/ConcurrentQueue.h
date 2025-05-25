#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

namespace vista
{
    template<typename T>
    class ConcurrentQueue
    {
    public:

        ConcurrentQueue() = default;
        VISTA_NONCOPYABLE(ConcurrentQueue)
		VISTA_DEFAULT_MOVABLE(ConcurrentQueue)
        ~ConcurrentQueue() = default;

        void Push(T const& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(value);
            condVariable.notify_one();
        }

        void Push(T&& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::forward<T>(value));
            condVariable.notify_one();
        }

        void WaitPop(T& value)
        {
            std::unique_lock<std::mutex> lock(mutex);
            condVariable.wait(lock, [this] {return !queue.empty(); });
            value = std::move(queue.front());
            queue.pop();
        }

        Bool TryPop(T& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (queue.empty()) return false;

            value = std::move(queue.front());
            queue.pop();
            return true;
        }

        Bool Empty() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.empty();
        }

        Uint64 Size() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.size();
        }

    private:
        std::queue<T> queue;
        mutable std::mutex mutex;
        std::condition_variable condVariable;
    };
}
