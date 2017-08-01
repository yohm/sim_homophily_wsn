#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <iostream>
#include <array>
#include <random>

//=================================================
// singleton class for random number generator
class Random {
public:
  static void Init(uint64_t seed, int num_threads = 1) {
    m_rnds.clear();

    for(uint64_t i=0; i <num_threads; i++) {
      std::seed_seq seq = {seed, i};
      m_rnds.push_back( std::mt19937_64(seq) );
    }
  }
  static double Rand01(int thread_num) {
    std::uniform_real_distribution<double> uni(0.0, 1.0);
    return uni(m_rnds[thread_num]);
  }
  static double Gaussian(int thread_num) {
    std::normal_distribution<double> gaussian;
    return gaussian(m_rnds[thread_num]);
  }
private:
  static std::vector< std::mt19937_64 > m_rnds;
};

#endif
