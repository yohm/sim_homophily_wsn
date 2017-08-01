#ifndef WSN_ND_LD_AGING_HPP
#define WSN_ND_LD_AGING_HPP

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
class WsnNDLDAging {
public:
  static const size_t NUM_OUTPUTS = 7;
  WsnNDLDAging(
    uint64_t seed, size_t net_size, double p_tri, double p_jump, double delta,
    double p_nd, double p_ld, double aging, double w_th
  );
  ~WsnNDLDAging() {};
  std::array<double,NUM_OUTPUTS> Run( uint32_t tmax, long measure_interval);
  void PrintEdge( std::ofstream& fout);
  void ToJson( std::ostream & out ) const;
  double AverageDegree();
  double AverageStrength();
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

  // state variables
  std::vector<Node> m_nodes;
  std::vector< std::pair<Node*,Node*> > m_enhancements;
  std::vector< std::pair<Node*,Node*> > m_attachements;

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
  Node* RandomSelectExceptForNeighbors(Node* i);

  // non-copyable
  WsnNDLDAging(const WsnNDLDAging&);
  WsnNDLDAging& operator=(const WsnNDLDAging&);

  double CalculateAverage(const std::vector<double>& vector) const;
  double PCC( const std::vector<double>& xs, const std::vector<double>& ys ) const;
  double PCC_k_knn() const;
  std::pair<double,double> CC_PCC_ck() const;
  void CalculateLocalCC( std::vector<double> & v_local_cc ) const;
  std::pair<double,double> O_PCC_ow() const;
  double LocalOverlap( const Node& ni, const Node& nj ) const;
};

#endif