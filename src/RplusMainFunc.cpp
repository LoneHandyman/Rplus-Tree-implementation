#ifndef TEMPLATE_RPLUS_MAIN_FUNC_HPP_
#define TEMPLATE_RPLUS_MAIN_FUNC_HPP_

#include "RplusTree.hpp"

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

#endif