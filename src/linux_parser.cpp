#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;


template<typename T>
T findValueByKey(string const &path, string const& keyFilter){
  string line,key;
  T value;
  std::ifstream stream(path);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == keyFilter){
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
//float LinuxParser::MemoryUtilization() { 
  //std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  //std::string line,name,value;
  //long m_Total, m_Free;//, m_Avail;
  //if(filestream.is_open()){
    //int cnt{0};
    //while(std::getline(filestream, line)){
      //std::istringstream linestream(line);
      //linestream >> name >> value;
      //if(name == "MemTotal:"){
        //m_Total = std::stol(value);
        //cnt++;
      //}
      //if(name == "MemFree:"){
        //m_Free = std::stol(value);
        //cnt++;
      //}
      //if(name == "MemAvailable:"){
      //  m_Avail = std::stol(value);
      //  cnt++;
      //}
      //if(cnt == 2){
        //return ((float)(m_Total - m_Free))/m_Total;
      //}
    //}
  //}
 // return ((float)(m_Total - m_Free))/m_Total; 
//}

float LinuxParser::MemoryUtilization(){
  const string path(kProcDirectory + kMeminfoFilename);
  const string memTotal = "MemTotal:";
  const string memFree = "MemFree:";
  const float Total = findValueByKey<float>(path, memTotal);
  const float Free = findValueByKey<float>(path, memFree);
  return (Total - Free) / Total;
}



// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  long uptime;
  if(filestream.is_open()){
    filestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long totalTime = 0;
  std::stringstream path;
  path <<  kProcDirectory << pid << kStatFilename;
  std::ifstream filestream(path.str());
  if(filestream.is_open()){
    std::string nomean;
    for(int i = 1; i <= 13; i++){
      filestream >> nomean;
    }
    long utime{0}, stime{0}, cutime{0}, cstime{0};
    filestream >> utime >> stime >> cutime >> cstime;
    totalTime += (utime + stime + cutime + cstime);
  };
  return totalTime/sysconf(_SC_CLK_TCK); 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::vector<std::string> cpuJiffies = LinuxParser::CpuUtilization();
  long kU = std::stol(cpuJiffies[kUser_]);
  long kN = std::stol(cpuJiffies[kNice_]);
  long kSy = std::stol(cpuJiffies[kSystem_]);
  long kI = std::stol(cpuJiffies[kIRQ_]);
  long kSo = std::stol(cpuJiffies[kSoftIRQ_]);
  long kSt = std::stol(cpuJiffies[kSteal_]);

  return kU + kN + kSy + kI + kSo + kSt; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::vector<std::string> cpuJiffies = LinuxParser::CpuUtilization();
  long kId = std::stol(cpuJiffies[kIdle_]);
  long kIO = std::stol(cpuJiffies[kIOwait_]);
  return kId + kIO; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::vector<std::string> cpuJiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    std::string line, item;
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> item;
      if(item == "cpu"){
        while(linestream >> item){
          cpuJiffies.emplace_back(item);
        }
        return cpuJiffies;
      }
    }
  }
  return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    std::string line, item;
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> item;
      if(item == "processes"){
        linestream >> item;
        return std::stoi(item);
      }
    }
  }
  return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    std::string line, item;
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> item;
      if(item == "procs_running"){
        linestream >> item;
        return std::stoi(item);
      }
    }
  } 
  return 0; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  std::stringstream path;
  path << kProcDirectory << pid << kCmdlineFilename;
  std::ifstream filestream(path.str());
  if(filestream.is_open()){
    std::string cmd;
    if(filestream >> cmd){
      return cmd;
    }
  }
  
  return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  std::stringstream path;
  path << kProcDirectory << pid << kStatusFilename;
  std::ifstream filestream(path.str());
  if(filestream.is_open()){
    std::string line, name, value;
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> name >> value;
      if(name == "VmSize:"){
        return value;
      }
    }
  }

  return string(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  std::stringstream path;
  path << kProcDirectory << pid << kStatusFilename;
  std::ifstream filestream(path.str());
  if(filestream.is_open()){
    std::string line;
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      std::string name, value;
      linestream >> name >> value;
      if(name == "Uid:"){
        return value;
      } 
    }
  }
  return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  std::string uId = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  std::string line,name,buf,cId;
  while(std::getline(filestream, line)){
    std::replace(line.begin(), line.end(),':', ' ');
    std::istringstream linestream(line);
    linestream >> name >> buf >> cId;
    if(cId == uId){
      return name;
    } 
  }
  return string(); 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
   std::stringstream path;
   path << kProcDirectory << pid << kStatFilename;
   std::ifstream filestream(path.str());
   if(filestream.is_open()){
     std::string buff, utime; 
     for(int i = 1; i <= 21; i++){
       filestream >> buff;
     }
     filestream >> utime;
     return std::stol(utime)/sysconf(_SC_CLK_TCK);
   }  
  return 0; 
}
