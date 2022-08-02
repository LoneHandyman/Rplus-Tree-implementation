#ifndef TEMPLATE_RPLUS_TREE_HPP_
#define TEMPLATE_RPLUS_TREE_HPP_

#define RPLUS_TREE_TEMPLATE   template<typename key_t, uint32_t dimensionality, uint32_t max_entries, uint32_t fill_factor>
#define RPLUS_TREE_QUAL kd::RPlusTree<key_t, dimensionality, max_entries, fill_factor>

#include "Geometry.hpp"
#include "algorithm"
#include <limits>
#include <map>
#include <queue>
#include <utility>
#include <vector>

namespace kd{

  RPLUS_TREE_TEMPLATE
  class RPlusTree{
  private:
    enum PartitionEval{PutLeft, PutRight, ToSplit};

    uint32_t height;

    struct Node;

    std::map<std::uintptr_t, Node*> node_allocation_storage;

    struct Entry{
      key_t key;
      Node* child;
      Rect<dimensionality> mbr;
    };
    Entry build_entry(const Rect<dimensionality>& bounds, const key_t& key_val);
    Entry build_entry(const Rect<dimensionality>& bounds, Node* child_node);

    struct Node{
      std::vector<Entry> slots;
      Node* parent;
      uint32_t level;

      bool is_leaf() {return level == 0;};
    }* root;
    Node* alloc_node(const uint32_t& lvl, Node* parent=nullptr);
    void free_node(Node* node);
    void erase_overflowed(Node* node);
    bool try_insert(Node* node, Entry& new_entry);

    uint32_t sweep(uint32_t& axis, double& cutline, std::vector<Rect<dimensionality>>& rects);
    std::pair<uint32_t, double> choose_cutline(std::vector<Entry>& entries);
    PartitionEval evaluate_partition(Entry& entry, std::pair<uint32_t, double>& cut_info);
    void cut_entry(Entry& old, std::pair<uint32_t, double>& cut_info, Entry& left, Entry& right);
    void split_node(Node* overflowed, Entry& left, Entry& right);
    void adjust_tree(Node* parent, Entry& left, Entry& right);
    void choose_leaves(Node* current, const Rect<dimensionality>& input_rect, std::queue<Node*>& leaves);
  public:
    RPlusTree();
    ~RPlusTree();
    void insert(const key_t& new_key, const Rect<dimensionality>& input_rect);

    uint32_t count_nodes();
  };

}

RPLUS_TREE_TEMPLATE
typename RPLUS_TREE_QUAL::Entry RPLUS_TREE_QUAL::build_entry(const Rect<dimensionality>& bounds, const key_t& key_val){
  Entry new_entry; 
  new_entry.mbr = bounds;
  new_entry.child = nullptr;
  new_entry.key = key_val;
  return new_entry;
}

RPLUS_TREE_TEMPLATE
typename RPLUS_TREE_QUAL::Entry RPLUS_TREE_QUAL::build_entry(const Rect<dimensionality>& bounds, Node* child_node){
  Entry new_entry; 
  new_entry.mbr = bounds;
  new_entry.child = child_node;
  return new_entry;
}

RPLUS_TREE_TEMPLATE
typename RPLUS_TREE_QUAL::Node* RPLUS_TREE_QUAL::alloc_node(const uint32_t& lvl, Node* parent){
  Node* new_node = new Node;
  new_node->level = lvl;
  new_node->parent = parent;
  node_allocation_storage.insert({std::uintptr_t(new_node), new_node});
  return new_node;
}

RPLUS_TREE_TEMPLATE
void RPLUS_TREE_QUAL::free_node(Node* node){
  typename std::map<std::uintptr_t, Node*>::iterator pos;
  if ((pos = node_allocation_storage.find(std::uintptr_t(node))) != node_allocation_storage.end()){
    node_allocation_storage.erase(pos);
    delete node;
  }
}

RPLUS_TREE_TEMPLATE
void RPLUS_TREE_QUAL::erase_overflowed(Node* node){
  if (node != nullptr){
    std::vector<Entry> checked;
    for (const Entry& entry : node->slots){
      Entry entry_c = entry;
      if (entry_c.child != nullptr){
        if (entry_c.child->slots.size() <= max_entries){
          checked.push_back(entry_c);
        }
        else{
          free_node(entry_c.child);
        }
      }
    }
    node->slots = checked;
  }
}

RPLUS_TREE_TEMPLATE
bool RPLUS_TREE_QUAL::try_insert(Node* node, Entry& new_entry){
  if (node != nullptr){
    if (new_entry.child != nullptr)
      new_entry.child->parent = node;
    node->slots.push_back(new_entry);
    return node->slots.size() <= max_entries;
  }
  return false;
}

RPLUS_TREE_TEMPLATE
RPLUS_TREE_QUAL::RPlusTree(){
  static_assert(fill_factor >= 2, "(Fill factor/min. number of entries) can not be less than 1.");
  static_assert(max_entries > fill_factor * 2, "Max entries should be greater than fill factor * 2.");

  height = 0;
  root = alloc_node(height, nullptr);
}

RPLUS_TREE_TEMPLATE
RPLUS_TREE_QUAL::~RPlusTree(){
  for (auto const& pair_kv : node_allocation_storage){
    delete pair_kv.second;
  }
}

RPLUS_TREE_TEMPLATE
void RPLUS_TREE_QUAL::insert(const key_t& new_key, const Rect<dimensionality>& input_rect){
  Entry data = build_entry(input_rect, new_key);
  std::queue<Node*> leaves;
  choose_leaves(root, input_rect, leaves);
  while (!leaves.empty()){
    Node* current_leaf = leaves.front();
    leaves.pop();
    if(!try_insert(current_leaf, data)){
      Entry left, right;
      split_node(current_leaf, left, right);
      adjust_tree(current_leaf->parent, left, right);
    }
  }
}

RPLUS_TREE_TEMPLATE
uint32_t RPLUS_TREE_QUAL::count_nodes(){
  return node_allocation_storage.size();
}

RPLUS_TREE_TEMPLATE
uint32_t RPLUS_TREE_QUAL::sweep(uint32_t& axis, double& cutline, std::vector<Rect<dimensionality>>& rects){
  std::vector<double> min_sweep_values;
  min_sweep_values.resize(rects.size());

  for (uint32_t i = 0; i < rects.size(); ++i)
    min_sweep_values[i] = rects[i].get_low_axis(axis);

  std::sort(min_sweep_values.begin(), min_sweep_values.end());
  cutline = min_sweep_values[fill_factor - 1];

  uint32_t cost = 0;
  for (uint32_t i = 0; i < rects.size(); ++i){
    if (rects[i].get_low_axis(axis) < cutline && rects[i].get_high_axis(axis) > cutline)
      ++cost;
  }
  return cost;
}

RPLUS_TREE_TEMPLATE
std::pair<uint32_t, double> RPLUS_TREE_QUAL::choose_cutline(std::vector<Entry>& entries){
  uint32_t lowest_cost = std::numeric_limits<uint32_t>::max();
  uint32_t choosen_axis;
  double choosen_cut, cutline;
  std::vector<Rect<dimensionality>> rects;
  rects.resize(entries.size());
  for (uint32_t i = 0; i < entries.size(); ++i){
    rects[i] = entries[i].mbr;
  }
  for (uint32_t axis = 0; axis < dimensionality; ++axis){
    uint32_t temp = sweep(axis, cutline, rects);
    if (temp < lowest_cost){
      lowest_cost = temp;
      choosen_axis = axis;
      choosen_cut = cutline;
    }
  }
  return std::make_pair(choosen_axis, choosen_cut);
}

RPLUS_TREE_TEMPLATE
typename RPLUS_TREE_QUAL::PartitionEval RPLUS_TREE_QUAL::evaluate_partition(Entry& entry, std::pair<uint32_t, double>& cut_info){
  if (entry.mbr.get_low_axis(cut_info.first) >= cut_info.second){
    return PartitionEval::PutLeft;
  }
  else if (entry.mbr.get_high_axis(cut_info.first) <= cut_info.second){
    return PartitionEval::PutRight;
  }
  return PartitionEval::ToSplit;
}

RPLUS_TREE_TEMPLATE
void RPLUS_TREE_QUAL::cut_entry(Entry& old, std::pair<uint32_t, double>& cut_info, Entry& left, Entry& right){
  Rect<dimensionality> left_rect = old.mbr;
  Rect<dimensionality> right_rect = left_rect.cut(cut_info.second, cut_info.first);

  if (old.child != nullptr){
    left = build_entry(left_rect, alloc_node(old.child->level, old.child->parent));
    right = build_entry(right_rect, alloc_node(old.child->level, old.child->parent));
    for (const Entry& entry : old.child->slots){
      Entry entry_c = entry;
      switch(evaluate_partition(entry_c, cut_info)){
        case PartitionEval::PutLeft:
          try_insert(left.child, entry_c);
          break;
        case PartitionEval::PutRight:
          try_insert(right.child, entry_c);
          break;
        case PartitionEval::ToSplit:
          Entry split_left, split_right;
          cut_entry(entry_c, cut_info, split_left, split_right);
          try_insert(left.child, split_left);
          try_insert(right.child, split_right);
          break;
      }
    }
  }
  else{
    left = build_entry(left_rect, old.key);
    right = build_entry(right_rect, old.key);
  }
}

RPLUS_TREE_TEMPLATE
void RPLUS_TREE_QUAL::split_node(Node* overflowed, Entry& left, Entry& right){
  std::pair<uint32_t, double> cut_info = choose_cutline(overflowed->slots);
  Rect<dimensionality> void_mbr;
  left = build_entry(void_mbr, alloc_node(overflowed->level, overflowed->parent));
  right = build_entry(void_mbr, alloc_node(overflowed->level, overflowed->parent));
  for (const Entry& entry : overflowed->slots){
    Entry entry_c = entry;
    switch(evaluate_partition(entry_c, cut_info)){
      case PartitionEval::PutLeft:
        try_insert(left.child, entry_c);
        left.mbr.expand(entry_c.mbr);
        break;
      case PartitionEval::PutRight:
        try_insert(right.child, entry_c);
        right.mbr.expand(entry_c.mbr);
        break;
      case PartitionEval::ToSplit:
        Entry split_left, split_right;
        cut_entry(entry_c, cut_info, split_left, split_right);
        try_insert(left.child, split_left);
        left.mbr.expand(split_left.mbr);
        try_insert(right.child, split_right);
        right.mbr.expand(split_right.mbr);
        break;
    }
  }
}

RPLUS_TREE_TEMPLATE
void RPLUS_TREE_QUAL::adjust_tree(Node* parent, Entry& left, Entry& right){
  if (parent == nullptr){
    free_node(root);
    Node* new_root = alloc_node(++height, nullptr);
    try_insert(new_root, left);
    try_insert(new_root, right);
    root = new_root;
  }
  else{
    erase_overflowed(parent);
    try_insert(parent, left);
    if (!try_insert(parent, right)){
      Entry pleft, pright;
      split_node(parent, pleft, pright);
      adjust_tree(parent->parent, pleft, pright);
    }
  }
}

RPLUS_TREE_TEMPLATE
void RPLUS_TREE_QUAL::choose_leaves(Node* current, const Rect<dimensionality>& input_rect, std::queue<Node*>& leaves){
  if (current->is_leaf() || current->slots.size() == 0) {
    leaves.push(current);
    return;
  }

  bool overlap = false;

  for (const Entry& entry : current->slots) {
    Entry entry_c = entry;
    if (entry_c.mbr.overlaps(input_rect)) {
      choose_leaves(entry.child, input_rect, leaves);
      overlap = true;
    }
  }

  if (!overlap) {
    double min_increment = std::numeric_limits<double>::max();
    Node* next = nullptr;
    for (const Entry& entry : current->slots) {
      Entry entry_c = entry;
      Rect<dimensionality> expanded = entry_c.mbr;
                           expanded.expand(input_rect);
      double increment = expanded.get_area() - input_rect.get_area();
      if (increment < min_increment) {
        min_increment = increment;
        next = entry_c.child;
      } 
    }
    choose_leaves(next, input_rect, leaves);
  }
}

#endif