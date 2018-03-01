#include <iostream>
#include <fstream>
#include <array>
#include "NetSpreading.hpp"

int main( int argc, char** argv) {

  if( argc != 4 ) {
    std::cerr << "Usage : ./spreading.out <N> <edg_file> <seed>" << std::endl;
    std::cerr << argc << std::endl;
    exit(1);
  }


  NetSpreading n(std::atol(argv[1]));
  n.LoadFile(argv[2]);
  n.RunSpreading(std::atol(argv[3]));

  return 0;
}
