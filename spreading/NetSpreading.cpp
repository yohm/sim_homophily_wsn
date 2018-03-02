//
// Created by Yohsuke Murase on 2018/03/01.
//

#include <iostream>
#include <fstream>
#include <set>
#include "NetSpreading.hpp"

NetSpreading::NetSpreading(long N) {
  for(long i=0; i<N; i++) { m_nodes.push_back( Node(i) ); }
}

void NetSpreading::LoadFile(const char *edg_file) {
  std::ifstream fin(edg_file);
  long i,j;
  double w;
  while( fin >> i >> j >> w ) {
    m_nodes[i].AddLink(j, w);
    m_nodes[j].AddLink(i, w);
  }
}

void NetSpreading::RunSpreading(long seed) {
  m_random.seed(seed);
  std::uniform_real_distribution<double> dist;
  long init = static_cast<long>( m_nodes.size() * dist(m_random) );

  double m_t = 0.0;
  m_nodes[init].SetInfected(m_t);
  m_nodes[init].CalcEventsAndPush(*this);

  while(true) {
    event_t next_event = m_events.PopNextEvent();
    // std::cerr << m_t << ' ' << m_events.Size() << std::endl;
    double t = next_event.first;
    if(t < 0) { break; }
    m_t = t;
    long i = next_event.second.first;
    long j = next_event.second.second;
    if(m_nodes[j].infected) { continue; }
    m_nodes[j].SetInfected(m_t);
    m_nodes[j].CalcEventsAndPush(*this);
  }
}

void NetSpreading::PrintInfections() const {
  std::set<double> infections;
  for(const auto& n: m_nodes) {
    infections.insert(n.infected_at);
  }
  long n = 0;
  for(auto t: infections) {
    std::cout << t << ' ' << n << std::endl;
    n++;
  }

}

