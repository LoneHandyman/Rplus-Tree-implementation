#ifndef TEMPLATE_RPLUS_KNN_HPP_
#define TEMPLATE_RPLUS_KNN_HPP_

#include "RplusTree.hpp"
#include <set>

template<typename Entry_t>
kd::MeasuredEntry<Entry_t>::MeasuredEntry(const std::vector<double>& point, Entry_t &md_obj) {
  entry = &md_obj;
  distance = entry->mbr.min_dist(point);
}

template<typename Entry_t>
bool kd::operator>(const MeasuredEntry<Entry_t> &A, const MeasuredEntry<Entry_t> &B) {
  return A.distance > B.distance;
}

RPLUS_TREE_TEMPLATE
std::vector<key_t> RPLUS_TREE_QUAL::knn_query(const std::vector<double>& query_point, const unsigned int& k){
  assert(root != nullptr);
  
  std::vector<key_t> knns(k);
  std::set<key_t> knns_temp;
  std::priority_queue<MeasuredEntry<RPlusTree::Entry>, std::vector<MeasuredEntry<RPlusTree::Entry>>, std::greater<MeasuredEntry<RPlusTree::Entry>>> best_branches;

  Node* closest_node = root;
  Entry* closest_entry = nullptr;

  do{
    if (closest_node != nullptr){
      for (unsigned int i = 0; i < closest_node->slots.size(); ++i) {
        MeasuredEntry<RPlusTree::Entry> measured_entry(query_point, closest_node->slots[i]);
        best_branches.push(measured_entry);
      }
    }
    else {
      assert(closest_entry != nullptr);
      knns_temp.insert(closest_entry->key);
    }
    closest_entry = best_branches.top().entry;
    closest_node = closest_entry->child;
    best_branches.pop();
  }
  while(knns_temp.size() < k && !best_branches.empty());
  std::copy(knns_temp.begin(), knns_temp.end(), knns.begin());
  return knns;
}

#endif
