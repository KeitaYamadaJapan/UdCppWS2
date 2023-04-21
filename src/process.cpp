#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid){
    pid_ = pid;
    userName_ = LinuxParser::User(pid_);
}

// TODO: Return this process's ID
int Process::Pid() { 
    return pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    int systemUpTime = LinuxParser::UpTime();
    int totalTimeActivate = LinuxParser::ActiveJiffies(pid_);
    int processUpTime = LinuxParser::UpTime(pid_);
    int cupTime = systemUpTime - processUpTime;
    cpuUtilization_ = ((float)totalTimeActivate) / cupTime;
    return cpuUtilization_; 
}

// TODO: Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(pid_); 
}

// TODO: Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram(pid_); 
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
    return userName_; 
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime(); 
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return this->cpuUtilization_ > a.cpuUtilization_; 
}
