#include <iostream>
#include <fstream>
#include <array>
#include "homophily_wsn.hpp"

int main( int argc, char** argv) {

  if( argc != 11 ) {
    std::cerr << "Usage : ./main.out <N> <p_tri> <p_jump> <p_ld> <aging> <w_th> <F> <q> <t_max> <seed>" << std::endl;
    std::cerr << argc << std::endl;
    exit(1);
  }

  long net_size = std::atol( argv[1] );
  double p_tri  = std::atof( argv[2] );
  double p_jump = std::atof( argv[3] );
  double p_ld   = std::atof( argv[4] );
  double aging  = std::atof( argv[5] );
  double w_th   = std::atof( argv[6] );
  long F        = std::atol( argv[7] );
  long q        = std::atol( argv[8] );
  long t_max    = std::atol( argv[9] );
  long seed     = std::atol( argv[10] );
  double delta  = 1.0; //std::atof( argv[4] );
  double p_nd   = 0.0; // std::atof( argv[5] );

  long t_measure_interval = 512;
  std::cerr << "Lists of given parameters are as follows:" << std::endl
            << "net_size:\t" << net_size << std::endl
            << "p_tri:\t" << p_tri << std::endl
            << "p_jump:\t" << p_jump << std::endl
            //<< "delta:\t" << delta << std::endl
            //<< "p_nd:\t" << p_nd << std::endl
            << "p_ld:\t" << p_ld << std::endl
            << "aging:\t" << aging << std::endl
            << "w_th:\t" << w_th << std::endl
            << "F:\t" << F << std::endl
            << "q:\t" << q << std::endl
            << "t_max:\t" << t_max << std::endl
            << "t_measure_interval:\t" << t_measure_interval << std::endl
            << "seed:\t" << seed << std::endl;

  HomophilyWSN sim(seed, net_size, p_tri, p_jump, delta,
                   p_nd, p_ld, aging, w_th, F, q);
  auto res = sim.Run(t_max, t_measure_interval);

  auto counts = sim.AttachCounts();
  std::cout << "counts: " << std::get<0>(counts) << ", " << std::get<1>(counts) << ", " << std::get<2>(counts) << std::endl;

  /*
  std::ofstream fout("_output.json");
  fout << "{\n"
       << "  \"degree\": "    << res[0] << ",\n"
       << "  \"strength\": "  << res[1] << ",\n"
       << "  \"pcc_k_knn\": " << res[2] << ",\n"
       << "  \"CC\": "        << res[3] << ",\n"
       << "  \"pcc_Ck\": "    << res[4] << ",\n"
       << "  \"overlap\": "   << res[5] << ",\n"
       << "  \"pcc_Ow\": "    << res[6] << "\n"
       << "}" << std::endl;
  fout.flush();
  fout.close();
   */

  std::ofstream eout("net.edg");
  sim.PrintEdge(eout);
  eout.close();

  std::ofstream tout("traits.txt");
  sim.PrintTraits(tout);
  tout.close();

  return 0;
}

