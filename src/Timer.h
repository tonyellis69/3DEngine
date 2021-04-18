#include <iostream>
#include <chrono>

class CTimer
{
public:
    CTimer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const { 
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count(); }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};

// Example code
//int main()
//{
//    CTimer tmr;
//    double t = tmr.elapsed();
//    std::cout << t << std::endl;
//
//    tmr.reset();
//    t = tmr.elapsed();
//    std::cout << t << std::endl;
//    return 0;
//}
