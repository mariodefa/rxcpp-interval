#include "rxcpp/rx.hpp"
#include <cstdio>
#include <chrono>

int main(){
    //rxcpp interval sample
    printf("//! [time_interval sample]\n");

    typedef rxcpp::schedulers::scheduler::clock_type::time_point::duration duration_type;

    using namespace std::chrono;
    auto values = rxcpp::observable<>::interval(milliseconds(1000))
            .time_interval()
            .take(3);
    values
        .as_blocking() //block the main thread
        .subscribe(
            [&](duration_type v) {
                long long int ms = duration_cast<milliseconds>(v).count();
                printf("OnNext: @%lldms\n", ms);
            },
            [&](std::exception_ptr ep) {
                try {
                    std::rethrow_exception(ep);
                } catch (const std::exception& ex) {
                    printf("OnError: %s\n", ex.what());
                }      
            },
            [&]() { 
                printf("OnCompleted\n"); 
            }
        );
    printf("//! [time_interval sample]\n");

    return 0;
}