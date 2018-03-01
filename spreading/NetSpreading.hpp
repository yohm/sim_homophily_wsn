//
// Created by Yohsuke Murase on 2018/03/01.
//

#ifndef NETSPREADING_HPP
#define NETSPREADING_HPP

#include <iostream>
#include <vector>
#include <map>
#include <random>

class Node;

typedef std::pair<double, std::pair<long,long> > event_t; // (t, (i,j))

class EQueue {
public:
  std::multimap<double, std::pair<long,long> > m_q;
  void PushEvent(const event_t& event) { m_q.insert(event); };
  event_t PopNextEvent() {
    auto it = m_q.begin();
    if(it == m_q.end()) { return event_t(-1, {-1,-1}); }
    double t = it->first;
    long i = it->second.first;
    long j = it->second.second;
    m_q.erase(it);
    return event_t(t,{i,j});
  };
  size_t Size() const { return m_q.size(); }
};


class NetSpreading {
  friend class Node;
public:
  NetSpreading(long N);
  void LoadFile(const char* edg_file);
  void RunSpreading(long seed);
private:
  double m_t; //time
  std::mt19937 m_random;
  std::vector<Node> m_nodes;
  EQueue m_events;
};

class Node {
public:
  Node(long _id) : id(_id), infected(false), infected_at(-1) {};
  const long id;
  void AddLink(long j, double w) {m_links[j] = w; };
  void SetInfected(double t) { infected = true; infected_at = t; }
  void CalcEventsAndPush(NetSpreading& net) {
    auto w_to_dt = [&net](double w) {
      std::exponential_distribution<double> dist(w);
      return dist(net.m_random);
    };
    for( const auto& l: m_links ) {
      long j = l.first;
      double w = l.second;
      if( net.m_nodes[j].infected ) { continue; }
      double t = w_to_dt(w) + infected_at;
      net.m_events.PushEvent( event_t(t,{id,j}) );
    }
  }
  bool infected;
  double infected_at;
private:
  std::map<long,double> m_links;
};


#endif //NETSPREADING_HPP
