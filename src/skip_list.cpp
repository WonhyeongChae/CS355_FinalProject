#include "skip_list.h"
#include <thread>
#include <iostream>
#include <algorithm>

Node* LockFreeSkipList::create_node(int key, int value, Node* down) {
    std::lock_guard<std::mutex> lock(pool_mutex);
    node_pool.push_back(std::make_unique<Node>(key, value, down));
    return node_pool.back().get();
}

int LockFreeSkipList::random_level() {
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(1, max_level);
    int level = 1;
    while (dist(gen) % 2 == 0 && level < max_level) {
        level++;
    }
    return level;
}

void LockFreeSkipList::init_head_tail() {
    head = create_node(INT_MIN, 0);
    tail = create_node(INT_MAX, 0);
    head->next = tail;
    for (int i = 1; i < max_level; ++i) {
        Node* new_head = create_node(INT_MIN, 0, head);
        Node* new_tail = create_node(INT_MAX, 0, tail);
        new_head->next = new_tail;
        head = new_head;
        tail = new_tail;
    }
    current_levels = max_level;
}

LockFreeSkipList::LockFreeSkipList(int max_lvl) : max_level(max_lvl) {
    init_head_tail();
}

LockFreeSkipList::~LockFreeSkipList() {}

bool LockFreeSkipList::insert(int key, int value) {
    std::vector<Node*> preds(max_level);
    std::vector<Node*> succs(max_level);
    
    while (true) {
        Node* pred = head;
        bool retry = false;
        for (int level = current_levels - 1; level >= 0; --level) {
            Node* curr = pred->next;
            while (true) {
                Node* next = curr->next;
                while (curr->marked) {
                    if (!pred->next.compare_exchange_weak(curr, next)) {
                        retry = true;
                        break;
                    }
                    curr = pred->next;
                }
                if (retry) break;
                if (curr->key < key) {
                    pred = curr;
                    curr = next;
                } else {
                    break;
                }
            }
            if (retry) break;
            preds[level] = pred;
            succs[level] = curr;
        }
        if (retry) continue;
        if (succs[0]->key == key) {
            return false;
        }

        Node* down = nullptr;
        int new_level = random_level();
        for (int level = 0; level < new_level; ++level) {
            Node* newNode = create_node(key, value, down);
            newNode->next = succs[level];
            if (level >= current_levels) {
                preds[level] = head;
                succs[level] = tail;
            }
            if (!preds[level]->next.compare_exchange_weak(succs[level], newNode)) {
                std::this_thread::yield();
                retry = true;
                break;
            }
            down = newNode;
        }
        if (!retry) {
            current_levels.store(std::max(current_levels.load(), static_cast<int>(new_level)));
            return true;
        }
    }
}

bool LockFreeSkipList::remove(int key) {
    Node* victim = nullptr;
    bool is_marked = false;

    while (true) {
        Node* pred = head;
        Node* curr = head->next;
        while (curr->key < key) {
            pred = curr;
            curr = curr->next;
        }
        if (curr->key != key) {
            return false;
        }
        if (!victim) {
            victim = curr;
        }

        if (curr != victim || is_marked) {
            return true;
        }

        if (!curr->flag.load() && 
            curr->marked.compare_exchange_strong(is_marked, true)) {
            Node* next = curr->next;
            if (pred->next.compare_exchange_strong(curr, next)) {
                return true;
            }
        }
    }
}

bool LockFreeSkipList::contains(int key) {
    Node* curr = head;
    for (int level = current_levels - 1; level >= 0; --level) {
        curr = head;
        while (true) {
            Node* next = curr->next;
            while (next->marked) {
                curr->next.compare_exchange_weak(next, next->next);
                next = curr->next;
            }
            if (next->key < key) {
                curr = next;
            } else {
                break;
            }
        }
    }
    return curr->key == key;
}