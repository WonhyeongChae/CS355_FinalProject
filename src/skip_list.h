#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <atomic>
#include <memory>
#include <random>
#include <limits>
#include <vector>
#include <iostream> // For validate output

template<typename T>
class SkipList {
private:
    static constexpr int MAX_LEVEL = 32; // Maximum possible level

    struct Node {
        T value;
        int level; // The actual level of this node
        std::vector<std::atomic<Node*>> next; // next[i] points to the next node at level i

        Node(T val, int lvl) : value(val), level(lvl), next(lvl + 1) {
            // Initialize all next pointers to nullptr initially.
            // In the constructor, head's pointers will be set to tail.
            for (int i = 0; i <= lvl; i++) {
                next[i].store(nullptr);
            }
        }
    };

    Node* head; // Sentinel node with minimum value
    Node* tail; // Sentinel node with maximum value
    std::atomic<int> currentLevel; // Current highest level in the list
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    // Determines the level for a new node
    int randomLevel() {
        int lvl = 0;
        // Increase level with 50% probability, up to MAX_LEVEL - 1
        // (because levels are 0 to MAX_LEVEL, so MAX_LEVEL+1 total possible levels)
        while (dis(gen) < 0.5 && lvl < MAX_LEVEL) {
            lvl++;
        }
        return lvl;
    }

public:
    // Constructor: Initializes head and tail sentinels
    SkipList() : currentLevel(0), gen(rd()), dis(0.0, 1.0) {
        // Using MAX_LEVEL for sentinels simplifies logic, they exist at all potential levels
        head = new Node(std::numeric_limits<T>::min(), MAX_LEVEL);
        tail = new Node(std::numeric_limits<T>::max(), MAX_LEVEL);

        // Initialize head's next pointers to point to tail at all levels
        for (int i = 0; i <= MAX_LEVEL; i++) {
            head->next[i].store(tail);
        }
    }

    // Destructor: Deallocates all nodes
    ~SkipList() {
        Node* current = head->next[0].load();
        while (current != tail) {
            Node* next = current->next[0].load();
            delete current;
            current = next;
        }
        delete head;
        delete tail;
    }

    // Inserts a value into the skip list. Returns true if successful, false if value already exists.
    bool insert(const T& value);

    // Removes a value from the skip list. Returns true if successful, false if value not found.
    bool remove(const T& value);

    // Checks if a value exists in the skip list.
    // Note: In concurrent scenarios, the result might be stale immediately after return.
    bool contains(const T& value) const;

    // Returns the approximate number of elements in the list.
    // Note: Not thread-safe for exact count during concurrent modifications.
    int size() const;

    // Validates the structural integrity of the skip list.
    // Note: Should be called when no other threads are modifying the list.
    bool validate() const;

    // Prevent copying and assignment
    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;
};

#endif // SKIP_LIST_H
