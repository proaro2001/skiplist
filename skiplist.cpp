#include "skiplist.h"
#include "random.h"
#include <cassert>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

// checks if the value needs to be inserted at a higher level
bool SkipList::shouldInsertAtHigherLevel() const {
  return probability >= Random::random() % 100;
}

// you code goes here
ostream &operator<<(ostream &out, const SkipList &skip) {
  for (int i = skip.levels; i > 0; i--) {
    out << "[level: " << i << "] ";
    SNode *dummy = skip.head->next[i - 1];
    while (dummy != nullptr) {
      out << dummy->val << "-->";
      dummy = dummy->next[i - 1];
    }
    out << "nullptr\n"; // it reaches the end of the level
  }
  return out;
}
/*
  This is codes for SNode class
*/

// constructor
SNode::SNode(int val) {
  this->val = val;
  next.push_back(nullptr);
}
// copy constructor
SNode::SNode(const SNode &other) {
  this->val = other.val;
  this->next = copyHeadVector(other);
}

// helper method to get other's head
vector<SNode *> SNode::copyHeadVector(const SNode &other) {
  vector<SNode *> temp(other.next.begin(), other.next.end());
  for (int i = 0; i < other.next.size(); i++) {
    (other.next[i] != nullptr) ? temp[i] = new SNode(*other.next[i])
                               : temp[i] = nullptr;
  }
  return temp;
}

/*
  This is codes for SkipList class
*/

// default SkipList has Depth of 1, just one doubly-linked list
SkipList::SkipList(int levels, int probability)
    : levels(levels), probability(probability) {
  head = new SNode();
  for (int i = 1; i < levels; i++) {
    head->next.push_back(nullptr);
  }
}

// copy constructor
SkipList::SkipList(const SkipList &other) {
  this->levels = other.levels;
  this->probability = other.probability;
  this->head = new SNode(*other.head);
}

// destructor
SkipList::~SkipList() {
  SNode *curr = head->next[0];
  while (curr != nullptr) {
    SNode *temp = curr;
    curr = curr->next[0];
    delete temp;
  }
  delete head;
  delete curr;
};

// Add to list, assume no duplicates
void SkipList::add(int val) {
  unordered_map<int, SNode *> path;
  findPath(val, head, head->next[levels - 1], path, levels - 1);
  int lvs = 0;
  SNode *temp = new SNode(val);
  for (int i = 1; i < levels; i++) {
    temp->next.push_back(nullptr);
  }
  while (lvs < levels && (lvs == 0 || shouldInsertAtHigherLevel())) {
    addHelper(path[lvs], path[lvs]->next[lvs], lvs, temp);
    lvs++;
  }
}

// helper function
void SkipList::addHelper(SNode *pre, SNode *cur, int lvs, SNode *addMe) {
  if (lvs == 0) {
    pre->next[0] = addMe;
    addMe->next[0] = cur;
  } else {
    if (cur == nullptr) {
      pre->next[lvs] = addMe;
    } else {
      pre->next[lvs] = addMe;
      addMe->next[lvs] = cur;
    }
  }
}

// helper function to find path
void SkipList::findPath(int val, SNode *prev, SNode *curr,
                        unordered_map<int, SNode *> &path, int lvs) {
  if (lvs < 0) {
    return;
  }
  if (curr == nullptr || val < curr->val) {
    path.emplace(lvs, prev); // record the previous path into path
    lvs--;
    findPath(val, prev, prev->next[lvs], path, lvs);
  } else {
    findPath(val, curr, curr->next[lvs], path, lvs);
  }
}

// Add to list, assume no duplicates
void SkipList::add(const vector<int> &values) {
  for (int x : values) {
    this->add(x);
  }
}

// return true if successfully removed
bool SkipList::remove(int val) {
  int lvs = levels - 1;
  while (lvs > 0 && !removeHelper(val, lvs, head, head->next[lvs])) {
    lvs--;
  }
  return removeHelper(val, 0, head, head->next[0]);
}

// helper function for remove function
bool SkipList::removeHelper(int val, int lvs, SNode *prev, SNode *curr) {
  if (curr == nullptr) {
    return false;
  }
  if (val == curr->val) {
    if (lvs == 0) {
      prev->next[lvs] = curr->next[lvs];
      delete curr;
      return true;
    }
    prev->next[lvs] = curr->next[lvs];
  } else if (val > curr->val && val > prev->val) {
    return removeHelper(val, lvs, curr, curr->next[lvs]);
  }
  return false;
}
bool SkipList::contains(int val) const {
  int lvs = levels - 1;
  return containsHelper(head, val, head->next[lvs], lvs);
}
// helper function for contains
bool SkipList::containsHelper(SNode *prev, int val, SNode *curr,
                              int lvs) const {
  if (curr == nullptr || val < curr->val) {
    lvs--;
    if (lvs < 0) {
      return false;
    }
    return containsHelper(head, val, head->next[lvs], lvs);
  }
  if (val == curr->val) {
    return true;
  }
  if (val > prev->val && val > curr->val) {
    return containsHelper(curr, val, curr->next[lvs], lvs);
  }
  return false;
}