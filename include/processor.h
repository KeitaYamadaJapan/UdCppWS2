#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 long pIdle_{0};
 long pnIdle_{0};
 long pTotal_{0};
};

#endif