//
// Created by Shae Bolt on 12/23/2021.
//

#ifndef VULKANWORKSPACE_THREADSAFEQUEUE_H
#define VULKANWORKSPACE_THREADSAFEQUEUE_H


#include <mutex>
#include <condition_variable>
#include <tuple>
#include <queue>
#include <utility>
namespace vul {
    template<class T>
    class ThreadSafeQueue {
        std::queue<T> q;
        mutable std::mutex mtx;
        std::condition_variable cv;
        std::condition_variable sync_wait;
        bool finish_processing = false;
        int sync_counter = 0;

        //all threads which were trying to do something with this queue will notify one and decriment the counter.
        void decrement_sync_counter() {
            if (--sync_counter == 0) {
                sync_wait.notify_one();
            }
        }

    public:

        using size_type = typename std::queue<T>::size_type;
        using container_type = typename std::queue<T>::container_type;
        using value_type = typename std::queue<T>::value_type;
        using reference = typename std::queue<T>::reference;
        using const_reference = typename std::queue<T>::const_reference;

        ThreadSafeQueue() = default;

        ~ThreadSafeQueue() {
            finish();
        }

        void push(T&& item) {

            std::scoped_lock<std::mutex> lock(mtx);

            q.push(std::move(item));
            cv.notify_one();

        }

        std::tuple<reference, std::scoped_lock<std::mutex>> guard_front(){
            return {q.front(), std::scoped_lock<std::mutex>(mtx)};
        }


        std::tuple<const_reference , std::scoped_lock<std::mutex>> guard_front() const{
            return {q.front(), std::scoped_lock<std::mutex>(mtx)};
        }

        std::tuple<reference, std::scoped_lock<std::mutex>> guard_back(){
            return {q.back(), std::scoped_lock<std::mutex>(mtx)};
        }


        std::tuple<const_reference , std::scoped_lock<std::mutex>> guard_back() const{
            return {q.back(), std::scoped_lock<std::mutex>(mtx)};
        }

        [[nodiscard]]
        size_type size() const{

            std::scoped_lock<std::mutex> lock(mtx);

            return q.size();

        }

        [[nodiscard]]
        bool empty() const{
            std::scoped_lock<std::mutex> lock(mtx);
            return q.empty();

        }

        [[nodiscard]]
        bool pop(T& item) {

            std::scoped_lock<std::mutex> lock(mtx);

            if (q.empty()) {
                return false;
            }

            item = std::move(q.front());
            q.pop();

            return true;
        }



        //assuming another thread is waiting to pop.
        [[nodiscard]]
        bool pop_sync(T& item) {

            std::unique_lock<std::mutex> lock(mtx);

            sync_counter++;

            cv.wait(lock, [&] {
                return !q.empty() || finish_processing;
            });

            if (q.empty()) {
                decrement_sync_counter();
                return false;
            }

            item = std::move(q.front());
            q.pop();

            decrement_sync_counter();
            return true;

        }

        void finish() {

            std::unique_lock<std::mutex> lock(mtx);

            //tell threads that we are finished processing, so even if queue is empty, exit.
            finish_processing = true;
            //notify all threads waiting that we are done.
            cv.notify_all();

            //each thread will decrement the sync counter, and signal sync_wait.
            //we wait until a sync_wait signal is sent where sync_counter is zero
            //when that happens we know all the other threads are done using the
            //queue.
            sync_wait.wait(lock, [&]() {
                return sync_counter == 0;
            });

            //not sure why we need this,
            finish_processing = false;

        }

    };

}


#endif //VULKANWORKSPACE_THREADSAFEQUEUE_H
