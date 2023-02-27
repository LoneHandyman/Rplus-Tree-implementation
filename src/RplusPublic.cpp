#ifndef TEMPLATE_RPLUS_PUBLIC_HPP_
#define TEMPLATE_RPLUS_PUBLIC_HPP_

#include "RplusTree.hpp"

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
    assert(current_leaf != nullptr);
    if(!try_insert(current_leaf, data)){
      Entry left, right;
      split_node(current_leaf, left, right);
      adjust_tree(current_leaf->parent, left, right);
    }
  }
}

RPLUS_TREE_TEMPLATE
void RPLUS_TREE_QUAL::print_structure(){
  std::queue<Node*> levels;
  levels.push(root);
  while (!levels.empty()){
    Node* current = levels.front();
    levels.pop();
    if (current == nullptr){
      std::cout << "\t[Null node]\n";
      continue;
    }
    print_node(current);
    if (!current->is_leaf()){
      for (const Entry& entry : current->slots){
        Entry entry_c = entry;
        levels.push(entry_c.child);
      }
    }
    else{
      std::cout << "\t|-> Key: ";
      for (const Entry& entry : current->slots){
        Entry entry_c = entry;
        std::cout << entry_c.key << ", ";
      }
      std::cout << "-eof-\n";
    }
  }
}

RPLUS_TREE_TEMPLATE
uint32_t RPLUS_TREE_QUAL::count_nodes(){
  return node_allocation_storage.size();
}

#endif