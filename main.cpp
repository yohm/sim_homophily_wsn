#include <iostream>
#include <fstream>
#include <array>
#include "homophily_wsn.hpp"

int main( int argc, char** argv) {

  if( argc != 10 ) {
    std::cerr << "Usage : ./main.out <N> <p_tri> <p_jump> <delta> <p_ld> <F> <q> <t_max> <seed>" << std::endl;
    exit(1);
  }

  long net_size = std::atol( argv[1] );
  double p_tri  = std::atof( argv[2] );
  double p_jump = std::atof( argv[3] );
  double delta  = std::atof( argv[4] );
  double p_ld   = std::atof( argv[5] );
  long F        = std::atol( argv[6] );
  long q        = std::atol( argv[7] );
  long t_max    = std::atol( argv[8] );
  long seed     = std::atol( argv[9]);
  double p_nd   = 0.0; // std::atof( argv[5] );
  double aging  = 1.0; // std::atof( argv[7] );
  double w_th   = 0.01; //std::atof( argv[8] );

  long t_init = static_cast<long> (t_max * 0.2);
  long t_measure_interval = 512;
  std::cerr << "Lists of given parameters are as follows:" << std::endl
            << "net_size:\t" << net_size << std::endl
            << "p_tri:\t" << p_tri << std::endl
            << "p_jump:\t" << p_jump << std::endl
            << "delta:\t" << delta << std::endl
            //<< "p_nd:\t" << p_nd << std::endl
            << "p_ld:\t" << p_ld << std::endl
            //<< "aging:\t" << aging << std::endl
            //<< "w_th:\t" << w_th << std::endl
            << "F:\t" << F << std::endl
            << "q:\t" << q << std::endl
            << "t_init:\t" << t_init << std::endl
            << "t_max:\t" << t_max << std::endl
            << "t_measure_interval:\t" << t_measure_interval << std::endl
            << "seed:\t" << seed << std::endl;

  HomophilyWSN sim(seed, net_size, p_tri, p_jump, delta,
                   p_nd, p_ld, aging, w_th, F, q);
  sim.Run(t_init, -1);
  auto res = sim.Run(t_max, t_measure_interval);

  std::ofstream fout("results.txt");
  for( double r : res ) {
    fout << r << std::endl;
  }
  return 0;
}

