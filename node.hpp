#ifndef NODE_HPP
#define NODE_HPP

#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include "random.hpp"

class Node;

//=================================================
class Edge {
public:
  Edge(Node* n, double w0) {
    node = n;
    weight = w0;
  }
  Node* node;
  double weight;
};

//=================================================
class Node {
public:
  Node(size_t id, std::vector<size_t> traits) : m_id(id), m_traits(traits) {};
  size_t GetId() const { return m_id; }
  size_t TraitAt(size_t f) const { return m_traits[f]; }

  // randomly select edge with the probability proportional to its weight
  // if excluded_node is not NULL, the parent node is not included in the candidates
  // when excluded_node is NULL, the edge is selected from all the connecting edges
  Edge* EdgeSelection(Node* excluded_node);
  Edge* EdgeSelectionSharingTrait(Node* excluded_node, size_t f);
  size_t Degree() const { return m_edges.size(); }
  size_t DegreeAtTrait(size_t f) const;
  double Strength() const;
  double LocalCC() const;
  Edge* FindEdge(Node* nj);  // return the pointer to edge. If not found, return NULL;
  void AddEdge(Node* nj, double initial_weight);
  void EnhanceEdge(Node* nj, double delta);
  void DeleteEdge(Node* nj);
  void AgingEdge(double aging_factor, double threshold);
  const std::vector<Edge>& GetEdges() const { return m_edges; }
  void ClearAll() { m_edges.clear(); }
  void SortEdgesByID();
  std::vector<size_t> sharedTraits( const Node* n );
protected:
  size_t m_id;
  std::vector<size_t> m_traits;
  std::vector<Edge> m_edges;
};

#endif
