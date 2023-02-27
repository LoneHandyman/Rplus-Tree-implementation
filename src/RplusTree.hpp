#pragma once

#include "Geometry.hpp"
#include <algorithm>
#include <limits>
#include <map>
#include <queue>
#include <utility>
#include <vector>
#include <cassert>

#define RPLUS_TREE_TEMPLATE   template<typename key_t, uint32_t dimensionality, uint32_t max_entries, uint32_t fill_factor>
#define RPLUS_TREE_QUAL kd::RPlusTree<key_t, dimensionality, max_entries, fill_factor>

namespace kd{

  RPLUS_TREE_TEMPLATE
  class RPlusTree{
  private:
    enum PartitionEval{PutLeft, PutRight, ToSplit};

    uint32_t height;

    struct Node;
    //RAM? No, just a garbage collector
    std::map<std::uintptr_t, Node*> node_allocation_storage;

    //Structures
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
    }* root;//Root of the tree

    //Allocators & sub-auxiliary
    Node* alloc_node(const uint32_t& lvl, Node* parent=nullptr);
    void free_node(Node* node);
    void erase_overflowed(Node* node);
    bool try_insert(Node* node, Entry& new_entry);
    void print_node(Node* node);

    //Auxiliary functions
    std::pair<uint32_t, double> choose_cutline(std::vector<Entry>& entries);
    void cut_entry(Entry& old, std::pair<uint32_t, double>& cut_info, Entry& left, Entry& right);
    void adjust_tree(Node* parent, Entry& left, Entry& right);
    void choose_leaves(Node* current, const Rect<dimensionality>& input_rect, std::queue<Node*>& leaves);

    //R+Tree main functions
    uint32_t sweep(uint32_t& axis, double& cutline, std::vector<Rect<dimensionality>>& rects);
    PartitionEval evaluate_partition(Entry& entry, std::pair<uint32_t, double>& cut_info);
    void split_node(Node* overflowed, Entry& left, Entry& right);

  public:
    //Constructor
    RPlusTree();
    //Destructor
    ~RPlusTree();

    //Modify operations
    void insert(const key_t& new_key, const Rect<dimensionality>& input_rect);
    //Search operations
    std::vector<key_t> knn_query(const std::vector<double>& query_point, const unsigned int& k);
    //Visualization
    void print_structure();
    uint32_t count_nodes();

    template<typename Entry_t>
    friend class MeasuredEntry;
  };

  template<typename Entry_t>
  struct MeasuredEntry {
    double distance;
    Entry_t* entry;
    MeasuredEntry(const std::vector<double>& point, Entry_t &md_obj);
  };

  template<typename Entry_t>
  bool operator>(const MeasuredEntry<Entry_t> &A, const MeasuredEntry<Entry_t> &B);

}

#include "../source/RplusAllocators.cpp"
#include "../source/RplusAuxiliary.cpp"
#include "../source/RplusKnn.cpp"
#include "../source/RplusPublic.cpp"
#include "../source/RplusMainFunc.cpp"