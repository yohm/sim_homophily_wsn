#include <omp.h>
#include <cmath>
#include <algorithm>
#include "node.hpp"

double Node::Strength() const {
  double weight_sum = 0.0;
  for( const Edge& e : m_edges ) {
    weight_sum += e.weight;
  }
  return weight_sum;
}

const size_t PROBS_SIZE_MAX = 100;

Edge* Node::EdgeSelection(Node* parent_node) {
  double prob_sum = 0.0;
  const size_t edge_size = m_edges.size();

  if( PROBS_SIZE_MAX > edge_size ) {
    double probs[PROBS_SIZE_MAX];
    for( size_t i=0; i<edge_size; i++) {
      const Edge& e = m_edges[i];
      if( e.node != parent_node ) {
        prob_sum += m_edges[i].weight;
      }
      probs[i] = prob_sum;
    }
    double r = prob_sum * Random::Rand01( omp_get_thread_num() );
    double * found = std::upper_bound(probs, probs+edge_size, r);
    assert( found != probs+PROBS_SIZE_MAX );
    return &(m_edges[ found - probs ]);
  }
  else {
    std::vector<double> v_probs( m_edges.size() );
    //std::vector<double> probs( m_edges.size(), 0.0 );
    size_t idx = 0;
    for( const Edge & e : m_edges ) {
      if( e.node != parent_node ) {
        prob_sum += e.weight;
      }
      v_probs[idx] = prob_sum;
      idx++;
    }

    double r = prob_sum * Random::Rand01( omp_get_thread_num() );
    std::vector<double>::iterator found = std::upper_bound(v_probs.begin(), v_probs.end(), r);
    assert( found != v_probs.end() );
    return &(m_edges[ found - v_probs.begin() ]);
  }
}

Edge* Node::FindEdge(Node* nj) {
  for( Edge& e : m_edges ) {
    if( e.node == nj ) { return &e; }
  }
  return NULL;
}

void Node::AddEdge(Node* nj, double initial_weight) {
  assert( FindEdge(nj) == NULL );
  m_edges.push_back(Edge(nj, initial_weight));
}

void Node::EnhanceEdge(Node* nj, double delta) {
  Edge* edge = FindEdge(nj);
  assert(edge != NULL);
  #pragma omp atomic
  edge->weight += delta;
}

void Node::DeleteEdge(Node* nj) {
  Edge* edge = FindEdge(nj);
  assert(edge != NULL);
  *edge = m_edges.back();
  m_edges.pop_back();
  assert( FindEdge(nj) == NULL );
}

void Node::AgingEdge(double aging_factor, double threshold) {
  for( Edge& e : m_edges ) {
    e.weight *= aging_factor;
  }
  auto isLessThanThreshold = [threshold]( const Edge& e ) {
    return e.weight < threshold;
  };
  m_edges.erase( std::remove_if(m_edges.begin(), m_edges.end(), isLessThanThreshold),
                 m_edges.end());
}

void Node::SortEdgesByID() {
  auto compareByID = []( const Edge& lhs, const Edge& rhs ) {
    return (lhs.node->GetId() < rhs.node->GetId() );
  };
  std::sort( m_edges.begin(), m_edges.end(), compareByID);
}

double Node::LocalCC() const {
  double total = 0.0;
  const size_t k = m_edges.size();
  if( k < 2 ) { return 0.0; }
  for( size_t i=0; i < k; i++ ) {
    for( size_t j = i+1; j < k; j++) {
      Node* n1 = m_edges[i].node;
      Node* n2 = m_edges[j].node;
      if( n1->FindEdge(n2) != NULL ) { total += 1.0; }
    }
  }
  double num_pairs = k * (k-1.0) / 2.0;
  return total / num_pairs;
}
