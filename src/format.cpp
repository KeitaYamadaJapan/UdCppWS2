#include <string>
#include<sstream>
#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    long sec = seconds % 60;
    long minutes = seconds / 60;
    long min = minutes % 60;
    long hour = minutes / 60;
    std::stringstream ss;
    if(hour < 10) ss << "0";
    ss << hour << ":";
    if(min < 10) ss << "0";
    ss << min << ":";
    if(sec < 10) ss << "0";
    ss << sec;
    return ss.str(); 
}
