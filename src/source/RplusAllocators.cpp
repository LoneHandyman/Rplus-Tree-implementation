#ifndef TEMPLATE_RPLUS_ALLOCATORS_HPP_
#define TEMPLATE_RPLUS_ALLOCATORS_HPP_

#include "RplusTree.hpp"


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
void RPLUS_TREE_QUAL::print_node(Node* node){
  if (node != nullptr){
    std::cout << "[Mem. dir.: " << node << "]\n";
    std::cout << "\t|-> (Level    : " << node->level << ")\n";
    std::cout << "\t|-> (Parent   : " << node->parent << ")\n";
    std::cout << "\t|-> (#Entries : " << node->slots.size() << ")\n";
  }
}

#endif
