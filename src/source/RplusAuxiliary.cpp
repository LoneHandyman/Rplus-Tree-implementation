#ifndef TEMPLATE_RPLUS_AUXILIARY_HPP_
#define TEMPLATE_RPLUS_AUXILIARY_HPP_

#include "RplusTree.hpp"

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
    if (next != nullptr)
      choose_leaves(next, input_rect, leaves);
  }
}

#endif
