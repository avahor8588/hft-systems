#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

template <typename T>
class BoundedBlockingQueue {
    public:
        BoundedBlockingQueue(int cap){
            capacity = cap;
        }

        void enqueu(T element){
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [&]() {return q.size() < capacity;});
            q.push(element);
            cv.notify_one();
        }

        T dequeu(){
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [&]() {return !q.empty();});
            T element = q.front();
            q.pop();
            cv.notify_one();
            return element;
        }

        int size(){
            std::unique_lock<std::mutex> lk(mtx);
            return q.size();
        }

    private:
        std::queue<T> q;
        std::mutex mtx;
        std::condition_variable cv;
        int capacity;
};

int main(){
    BoundedBlockingQueue<int> queue(2);
    std::thread t1(&BoundedBlockingQueue<int>::enqueu, &queue, 1);
    std::thread t2(&BoundedBlockingQueue<int>::enqueu, &queue, 2);
    std::thread t3(&BoundedBlockingQueue<int>::enqueu, &queue, 3);
    std::thread t4(&BoundedBlockingQueue<int>::dequeu, &queue);
    std::thread t5(&BoundedBlockingQueue<int>::dequeu, &queue);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();


    return 0;

}