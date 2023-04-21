#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    long cIdle = LinuxParser::IdleJiffies();
    long cnIdle = LinuxParser::ActiveJiffies();
    long cTotal = cIdle + cnIdle;
    long dIdle = cIdle - pIdle_;
    long dTotal = cTotal - pTotal_;
    pIdle_ = cIdle;
    pnIdle_ = cnIdle;
    pTotal_ = cTotal;
    return ((float)(dTotal - dIdle)) / dTotal; 
}