#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <atomic>
#include <memory>
#include <random>
#include <limits>
#include <vector>

template<typename T>
class SkipList {
private:
    static constexpr int MAX_LEVEL = 32;
    
    struct Node {
        T value;
        int level;
        std::vector<std::atomic<Node*>> next;
        
        Node(T val, int lvl) : value(val), level(lvl), next(lvl + 1) {
            for (int i = 0; i <= lvl; i++) {
                next[i].store(nullptr);
            }
        }
    };

    Node* head;
    Node* tail;
    std::atomic<int> level;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    int randomLevel() {
        int lvl = 0;
        while (dis(gen) < 0.5 && lvl < MAX_LEVEL - 1) {
            lvl++;
        }
        return lvl;
    }

public:
    SkipList() : level(0), gen(rd()), dis(0.0, 1.0) {
        head = new Node(std::numeric_limits<T>::min(), MAX_LEVEL);
        tail = new Node(std::numeric_limits<T>::max(), MAX_LEVEL);
        
        for (int i = 0; i <= MAX_LEVEL; i++) {
            head->next[i].store(tail);
        }
    }

    ~SkipList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next[0].load();
            delete current;
            current = next;
        }
    }

    bool insert(const T& value);
    bool remove(const T& value); 
    bool contains(const T& value) const;
    int size() const;
    
    // Prevent copying and assignment
    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;
};

#endif
