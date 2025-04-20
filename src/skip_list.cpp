// skip_list.cpp
#include "skip_list.h"
#include <thread>
#include <iostream>

Node* LockFreeSkipList::create_node(int key, int value, Node* down) {
    return new Node(key, value, down);
}

int LockFreeSkipList::random_level() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<> dist(1, max_level);
    return dist(gen);
}

LockFreeSkipList::LockFreeSkipList(int max_lvl) : max_level(max_lvl) {
    head = create_node(INT_MIN, 0);
    tail = create_node(INT_MAX, 0);
    head->next = tail;
    current_levels = 1;
}

LockFreeSkipList::~LockFreeSkipList() {
    Node* curr = head;
    while (curr) {
        Node* next = curr->down.load();
        while (curr) {
            Node* temp = curr;
            curr = curr->next;
            delete temp;
        }
        curr = next;
    }
}

bool LockFreeSkipList::insert(int key, int value) {
    std::vector<Node*> preds(max_level);
    std::vector<Node*> succs(max_level);
    
    while (true) {
        Node* pred = head;
        Node* curr = nullptr;
        for (int level = current_levels - 1; level >= 0; --level) {
            curr = pred->next;
            while (curr->key < key) {
                pred = curr;
                curr = curr->next;
            }
            preds[level] = pred;
            succs[level] = curr;
        }
        if (curr->key == key) {
            return false; // 이미 존재
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
                delete newNode;
                break;
            }
            down = newNode;
        }
        return true;
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
            pred->next.compare_exchange_strong(curr, next);
            return true;
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