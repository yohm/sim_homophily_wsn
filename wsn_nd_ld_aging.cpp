#include <cmath>
#include "wsn_nd_ld_aging.hpp"

WsnNDLDAging::WsnNDLDAging(
  uint64_t seed, size_t net_size, double p_tri, double p_jump, double delta,
  double p_nd, double p_ld, double aging, double w_th)
: m_seed(seed), m_net_size(net_size), m_p_tri(p_tri), m_p_jump(p_jump), m_delta(delta),
  m_p_nd(p_nd), m_p_ld(p_ld), m_aging(aging), m_link_th(w_th)
{
  #pragma omp parallel
  {
    int num_threads = omp_get_num_threads();
    #pragma omp master
    {
      std::cerr << "num_threads: " << num_threads << std::endl;
      Random::Init(seed, num_threads);
      for( size_t i = 0; i < m_net_size; i++) {
        Node node(i);
        m_nodes.push_back( node );
      }
    }
  }
}

std::array<double,WsnNDLDAging::NUM_OUTPUTS> WsnNDLDAging::Run( uint32_t t_max, long measure_interval ) {
  const bool measure_time_series = (0 < measure_interval);

  std::array< std::vector<double>, NUM_OUTPUTS> a_v_series;

  #pragma omp parallel
  {
    for( uint32_t t=0; t < t_max; ++t) {
      LocalAndGlobalAttachement();

      #pragma omp master
      {
        if( m_p_nd > 0.0 ) {
          NodeDeletion();
        }
      }
      #pragma omp barrier

      if( m_p_ld > 0.0 ) {
        LinkDeletion();
      }

      if( m_aging < 1.0 ) {
        LinkAging();
      }

      #pragma omp master
      {
        if( measure_time_series && (t%measure_interval==0)) {
          a_v_series[0].push_back( AverageDegree() );
          a_v_series[1].push_back( AverageStrength() );
          a_v_series[2].push_back( PCC_k_knn() );
          auto cc_pccck = CC_PCC_ck();
          a_v_series[3].push_back( cc_pccck.first );
          a_v_series[4].push_back( cc_pccck.second );
          auto o_pcc_ow = O_PCC_ow();
          a_v_series[5].push_back(o_pcc_ow.first);
          a_v_series[6].push_back(o_pcc_ow.second);
        }
      }
      #pragma omp barrier
    }
  }

  std::array<double,NUM_OUTPUTS> results;
  for( size_t i=0; i<NUM_OUTPUTS; i++ ) {
    results[i] = CalculateAverage( a_v_series[i] );
  }
  return results;
}

void WsnNDLDAging::PrintEdge( std::ofstream & fout) {
  for( const Node & n : m_nodes ) {
    for( const Edge & e : n.GetEdges() ) {
      size_t i = n.GetId();
      size_t j = e.node->GetId();
      if( i < j ) { fout << i << ' ' << j << ' ' << e.weight << std::endl; }
    }
  }
}

void WsnNDLDAging::ToJson( std::ostream& out ) const {
  out << "{ \"num_nodes\": " << m_nodes.size() << ",\n";

  out << "\"links\": [\n";
  std::string token = "";
  for( const Node & n : m_nodes ) {
    for( const Edge & e : n.GetEdges() ) {
      size_t i = n.GetId();
      size_t j = e.node->GetId();
      if( i < j ) {
        out << token << "[" << i << "," << j << "," << e.weight << "]";
        token = ",\n";
      }
    }
  }
  out << "]}";
}

double WsnNDLDAging::AverageDegree() {
  size_t total = 0;
  if( m_nodes.empty() ) { return 0.0; }
  for( const Node & n : m_nodes ) {
    total += n.Degree();
  }
  return static_cast<double>(total) / m_nodes.size();
}

double WsnNDLDAging::AverageStrength() {
  double total = 0.0;
  if( m_nodes.empty() ) { return 0.0; }
  for( const Node & n : m_nodes ) {
    total += n.Strength();
  }
  return total / m_nodes.size();
}

void WsnNDLDAging::LocalAndGlobalAttachement() {
  GA();
  StrengthenEdges();
  LA();
  StrengthenEdges();
}

void WsnNDLDAging::GA() {
  // Global attachment
  int thread_num = omp_get_thread_num();
  std::vector< std::pair<Node*,Node*> > local_attachements;

  const size_t size = m_nodes.size();
  #pragma omp for schedule(static)
  for( size_t i = 0; i < size; ++i) {
    Node * ni = &m_nodes[i];
    double r = Random::Rand01(thread_num);
    if( ni->Degree() == 0 || r < m_p_jump ) {
      if( ni->Degree() == m_net_size - 1 ) { continue; }
      Node* nj = RandomSelectExceptForNeighbors(ni);
      assert( ni->FindEdge(nj) == NULL );
      AttachPair(ni, nj, local_attachements);
    }
  }

  #pragma omp critical
  {
    m_attachements.insert(m_attachements.end(), local_attachements.begin(), local_attachements.end());
  }
  #pragma omp barrier
}

void WsnNDLDAging::LA() {
  // Local attachment
  int thread_num = omp_get_thread_num();
  std::vector< std::pair<Node*,Node*> > local_enhancements;
  std::vector< std::pair<Node*,Node*> > local_attachements;

  const size_t size = m_nodes.size();
  #pragma omp for schedule(static)
  for( size_t i=0; i < size; ++i) {
    // search first child
    Node* ni = &m_nodes[i];
    if( ni->Degree() == 0 ) { continue; }
    Edge* first_edge = ni->EdgeSelection(NULL);
    Node* first_child = first_edge->node;
    EnhancePair(ni, first_child, local_enhancements);

    // search second child
    if( first_child->Degree() == 1 ) { continue; }
    Edge* second_edge = first_child->EdgeSelection(ni);
    Node* second_child = second_edge->node;
    EnhancePair(first_child, second_child, local_enhancements);

    // connect i and second_child with p_tri
    if( ni->FindEdge(second_child) ) {
      EnhancePair(ni, second_child, local_enhancements);
    } else {
      if( Random::Rand01(thread_num) < m_p_tri ) {
        AttachPair(ni, second_child, local_attachements);
      }
    }
  }

  #pragma omp critical
  {
    m_enhancements.insert(m_enhancements.end(), local_enhancements.begin(), local_enhancements.end());
    m_attachements.insert(m_attachements.end(), local_attachements.begin(), local_attachements.end());
  }
  #pragma omp barrier
}

void WsnNDLDAging::LinkDeletion() {
  std::map<size_t, std::vector<size_t> > linksToRemove;
  const int thread_num = omp_get_thread_num();

  #pragma omp for schedule(static,100)
  for( size_t i=0; i < m_net_size; i++) {
    for( const Edge& edge : m_nodes[i].GetEdges() ) {
      size_t j = edge.node->GetId();
      if( j > i && Random::Rand01( thread_num ) < m_p_ld ) {
        linksToRemove[i].push_back(j);
        linksToRemove[j].push_back(i);
      }
    }
  }

  #pragma omp critical
  {
    for( const auto pair : linksToRemove ) {
      size_t i = pair.first;
      const std::vector<size_t> &jList = pair.second;
      for( size_t j : jList ) {
        m_nodes[i].DeleteEdge( &m_nodes[j] );
      }
    }
  }
  #pragma omp barrier

  #pragma omp for
  for( size_t i=0; i < m_net_size; i++) {
    m_nodes[i].SortEdgesByID();
  }
}

void WsnNDLDAging::NodeDeletion() {
  assert( omp_get_thread_num() == 0 );
  for( Node & n : m_nodes ) {
    if( Random::Rand01(0) < m_p_nd ) {
      DeleteNode(&n);
    }
  }
}

void WsnNDLDAging::DeleteNode(Node* ni) {
  for( const Edge& e : ni->GetEdges() ) {
    e.node->DeleteEdge(ni);
  }
  ni->ClearAll();
}


void WsnNDLDAging::StrengthenEdges() {
  // strengthen edges
  // #pragma omp barrier
  #pragma omp master
  {
  std::sort(m_attachements.begin(), m_attachements.end());
  m_attachements.erase( std::unique(m_attachements.begin(), m_attachements.end()), m_attachements.end() );

  for( auto pair : m_attachements ) {
    Node* ni = pair.first;
    Node* nj = pair.second;
    assert( ni->FindEdge(nj) == NULL );
    assert( nj->FindEdge(ni) == NULL );
    const double w_0 = 1.0;
    ni->AddEdge(nj, w_0);
    nj->AddEdge(ni, w_0);
  }
  }
  #pragma omp barrier

  const size_t en_size = m_enhancements.size();
  #pragma omp for schedule(static)
  for( size_t idx = 0; idx < en_size; idx++) {
    Node* ni = m_enhancements[idx].first;
    Node* nj = m_enhancements[idx].second;
    ni->EnhanceEdge(nj, m_delta);
    nj->EnhanceEdge(ni, m_delta);
  }

  #pragma omp master
  {
  m_attachements.clear();
  m_enhancements.clear();
  }
  #pragma omp barrier
}

void WsnNDLDAging::AttachPair(Node* ni, Node* nj, std::vector< std::pair<Node*,Node*> >& attachements) {
  std::pair<Node*, Node*> node_pair = (ni<nj) ? std::make_pair(ni, nj) : std::make_pair(nj, ni);
  attachements.push_back(node_pair);
}

void WsnNDLDAging::EnhancePair(Node * ni, Node* nj, std::vector< std::pair<Node*,Node*> >& enhancements) {
  std::pair<Node*, Node*> node_pair = (ni<nj) ? std::make_pair(ni, nj) : std::make_pair(nj, ni);
  enhancements.push_back(node_pair);
}

void WsnNDLDAging::LinkAging() {
  #pragma omp for schedule(static)
  for( size_t i=0; i < m_net_size; i++) {
    m_nodes[i].AgingEdge(m_aging, m_link_th);
  }
}

Node* WsnNDLDAging::RandomSelectExceptForNeighbors(Node* ni) {
  size_t num_candidate = m_net_size - ni->Degree() - 1;
  int idx = static_cast<int>( Random::Rand01( omp_get_thread_num() ) * num_candidate );

  std::vector<size_t> exclude_index;
  for( const Edge& e : ni->GetEdges() ) {
    exclude_index.push_back( e.node->GetId() );
  }
  exclude_index.push_back(ni->GetId());
  std::sort(exclude_index.begin(), exclude_index.end());
  assert( exclude_index.size() == ni->Degree() + 1 );

  for( size_t excluded : exclude_index ) {
    if( idx >= excluded ) { idx++; }
    else { break; }
  }
  assert( idx < static_cast<int>(m_net_size) );
  return &m_nodes[idx];
}

double WsnNDLDAging::CalculateAverage(const std::vector<double>& vector) const {
  if( vector.empty() ) { return 0.0; }
  double sum = 0.0;
  for( const auto x : vector ) {
    sum += x;
  }
  return sum / vector.size();
}

double WsnNDLDAging::PCC( const std::vector<double>& xs, const std::vector<double>& ys ) const {

  auto sum = []( const std::vector<double>& a ) {
    double ret = 0.0;
    for( double x: a ) { ret += x; }
    return ret;
  };
  auto square_sum = []( const std::vector<double>& a ) {
    double ret = 0.0;
    for( double x: a ) { ret += x*x; }
    return ret;
  };

  double xsum = sum( xs );
  double ysum = sum( ys );
  double x_square_sum = square_sum( xs );
  double y_square_sum = square_sum( ys );
  double product_sum = 0.0;
  for( size_t i=0; i<xs.size(); i++) {
    product_sum += xs[i] * ys[i];
  }
  double n = static_cast<double>( xs.size() );
  double numerator = product_sum - (xsum*ysum/n);
  double denominator = (x_square_sum - xsum*xsum/n) * (y_square_sum - ysum*ysum/n);
  denominator = std::sqrt( denominator );
  if( denominator == 0.0 ) { return 0.0; }
  return numerator / denominator;
}

double WsnNDLDAging::PCC_k_knn() const {
  std::vector<double> xs;
  std::vector<double> ys;

  for( size_t i=0; i < m_net_size; i++ ) {
    for( const Edge& e: m_nodes[i].GetEdges() ) {
      const size_t j = e.node->GetId();
      if( j > i ) {
        const Node& ni = m_nodes[i];
        const Node& nj = m_nodes[j];
        double ki = static_cast<double>(ni.Degree());
        double kj = static_cast<double>(nj.Degree());
        xs.push_back(ki);
        ys.push_back(kj);
      }
    }
  }
  return PCC( xs, ys );
}

void WsnNDLDAging::CalculateLocalCC(std::vector<double> &v_local_cc) const {
  v_local_cc.resize( m_net_size );

  for( size_t i=0; i< m_net_size; i++ ) {
    v_local_cc[i] = m_nodes[i].LocalCC();
  }
}

std::pair<double,double> WsnNDLDAging::CC_PCC_ck() const {
  std::vector<double> v_local_cc;
  CalculateLocalCC( v_local_cc );

  std::vector<double> v_k( m_net_size );
  for( size_t i=0; i<m_net_size; i++ ) {
    v_k[i] = m_nodes[i].Degree();
  }

  double cc = CalculateAverage( v_local_cc );
  double pcc_ck = PCC( v_local_cc, v_k );
  return std::make_pair(cc, pcc_ck);
}

std::pair<double,double> WsnNDLDAging::O_PCC_ow() const {
  std::vector<double> v_overlaps, v_weights;

  for( size_t i=0; i < m_net_size; i++ ) {
    for( const Edge& e : m_nodes[i].GetEdges() ) {
      const size_t j = e.node->GetId();
      if( j > i ) {
        const Node& ni = m_nodes[i];
        const Node& nj = m_nodes[j];
        double o = LocalOverlap( ni, nj );
        v_overlaps.push_back(o);
        v_weights.push_back(e.weight);
      }
    }
  }

  double ave_o = CalculateAverage( v_overlaps );
  double pcc_ow = PCC( v_overlaps, v_weights );
  return std::make_pair( ave_o, pcc_ow );
}

double WsnNDLDAging::LocalOverlap(const Node &ni, const Node &nj) const {
  const size_t ki = ni.Degree();
  const size_t kj = nj.Degree();
  if( ki == 1 && kj == 1 ) { return 0.0; }

  std::set<Node*> neighbors_i;
  for( const Edge& e : ni.GetEdges() ) {
    neighbors_i.insert( e.node );
  }
  size_t num_common = 0;
  for( const Edge& e : nj.GetEdges() ) {
    Node * pn = e.node;
    if( neighbors_i.find(pn) != neighbors_i.end() ) { num_common++;}
  }

  return static_cast<double>(num_common)/(ki+kj-2-num_common);
}

