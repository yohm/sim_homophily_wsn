#ifndef HOMOPHILY_WSN_HPP
#define HOMOPHILY_WSN_HPP

#include <omp.h>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <cstdint>
#include "random.hpp"
#include "node.hpp"


//================================================
class HomophilyWSN {
public:
  static const size_t NUM_OUTPUTS = 7;
  HomophilyWSN(
    uint64_t seed, size_t net_size, double p_tri, double p_jump, double delta,
    double p_nd, double p_ld, double aging, double w_th, long F, long q
  );
  ~HomophilyWSN() {};
  std::array<double,NUM_OUTPUTS> Run( uint32_t tmax, long measure_interval);
  void PrintEdge( std::ofstream& fout);
  void PrintTraits( std::ofstream& fout);
  void ToJson( std::ostream & out ) const;
  double AverageDegree();
  double AverageStrength();
  std::tuple<long,long,long> AttachCounts() const { return std::make_tuple(m_ga_count1,m_ga_count2,m_la_count); };
protected:
  // parameters
  const uint64_t m_seed;
  const size_t m_net_size;
  const double m_p_tri;
  const double m_p_jump;
  const double m_delta;
  const double m_p_nd;
  const double m_p_ld;
  const double m_aging;
  const double m_link_th;
  const long m_F;
  const long m_q;

  // state variables
  std::vector<Node> m_nodes;
  std::vector< std::pair<Node*,Node*> > m_enhancements;
  std::vector< std::pair<Node*,Node*> > m_attachements;
  std::map< std::pair<size_t,size_t>, std::vector<size_t> > m_mapTraitsNodes;
  long m_ga_count1, m_ga_count2, m_la_count;

  void ConstructMapTraitsNodes();
  void LocalAndGlobalAttachement(); // LA and GA
  void LA();
  void GA();
  void AttachPair(Node* i, Node* j, std::vector< std::pair<Node*,Node*> >& attachements);
  void EnhancePair(Node* i, Node* j, std::vector< std::pair<Node*,Node*> >& enhancements);
  void StrengthenEdges();
  void LinkDeletion();
  void LinkAging();
  void NodeDeletion();
  void DeleteNode(Node* ni);
  Node* RandomSelectNodeSharingTrait(Node *i, size_t f);

  // non-copyable
  HomophilyWSN(const HomophilyWSN&);
  HomophilyWSN& operator=(const HomophilyWSN&);

  double CalculateAverage(const std::vector<double>& vector) const;
  double PCC( const std::vector<double>& xs, const std::vector<double>& ys ) const;
  double PCC_k_knn() const;
  std::pair<double,double> CC_PCC_ck() const;
  void CalculateLocalCC( std::vector<double> & v_local_cc ) const;
  std::pair<double,double> O_PCC_ow() const;
  double LocalOverlap( const Node& ni, const Node& nj ) const;
};

#endif
