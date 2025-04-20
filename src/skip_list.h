// skip_list.h
#pragma once
#include <atomic>
#include <vector>
#include <random>
#include <mutex>
#include <chrono>

struct Node {
    int key;
    int value;
    std::atomic<Node*> next;
    std::atomic<Node*> down;
    std::atomic<bool> marked;
    std::atomic<bool> flag;

    Node(int k, int v, Node* d = nullptr) 
        : key(k), value(v), next(nullptr), down(d), marked(false), flag(false) {}
};

class LockFreeSkipList {
private:
    Node* head;
    Node* tail;
    int max_level;
    std::atomic<int> current_levels;

    Node* create_node(int key, int value, Node* down = nullptr);
    int random_level();

public:
    LockFreeSkipList(int max_lvl = 20);
    ~LockFreeSkipList();

    bool insert(int key, int value);
    bool remove(int key);
    bool contains(int key);
};