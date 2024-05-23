#include "rxcpp/rx.hpp"
#include <cstdio>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>

//global async vars
std::mutex mutexSubsEnd;
std::atomic<int> completedFlag(0); 

bool isSubscriptionEnded() {
    std::unique_lock<std::mutex> lock3(mutexSubsEnd);
    bool subEnded = completedFlag.load() != 0;
    lock3.unlock();
    return subEnded;
}

int main(){
    //async vars    
    std::condition_variable notifierSubscriptionEnd;     

    //rxcpp interval sample
    printf("//! [time_interval sample]\n");

    typedef rxcpp::schedulers::scheduler::clock_type::time_point::duration duration_type;

    using namespace std::chrono;
    //this runs in a separate thread created by rxcpp
    auto values = rxcpp::observable<>::interval(milliseconds(1000))
            .time_interval()
            .take(3);
    values.
        subscribe(
            [&](duration_type v) {
                //executed in the main thread
                long long int ms = duration_cast<milliseconds>(v).count();
                printf("OnNext: @%lldms\n", ms);
            },
            [&](std::exception_ptr ep) {
                //executed in the main thread
                try {
                    std::rethrow_exception(ep);
                } catch (const std::exception& ex) {
                    printf("OnError: %s\n", ex.what());
                }
                std::unique_lock<std::mutex> lock1(mutexSubsEnd);
                completedFlag = 2;
                lock1.unlock();
                notifierSubscriptionEnd.notify_one();                
            },
            [&]() { 
                //executed in the main thread
                printf("OnCompleted\n"); 
                std::unique_lock<std::mutex> lock2(mutexSubsEnd);
                completedFlag = 1;
                lock2.unlock();
                notifierSubscriptionEnd.notify_one();
            }
        );
    //all lines bellow are executed in the main thread
    printf("//! [time_interval sample]\n");

    //end conditions        
    //dummy lock for condition_variable to block the main thread
    std::mutex auxMutex;
    std::unique_lock<std::mutex> auxLock(auxMutex);
    //timeout (5s) or subscription ends
    //similar to while(timeout || subscriptionsEnd) do-nothing;
    notifierSubscriptionEnd.wait_for(auxLock, std::chrono::seconds(5), isSubscriptionEnded);

    if (completedFlag==0) {
        printf("Timeout occurred before OnCompleted\n");
    }
    if (completedFlag==1) {
        printf("Subscription completed\n");
    }
    if (completedFlag==2) {
        printf("Subscription raised an exception\n");
    }

    return 0;
}