#ifndef LOCK_FREE_LINKED_LIST_H
#define LOCK_FREE_LINKED_LIST_H

#include <atomic>
#include <limits>
#include <cstdint>
#include <iostream>

// Lock-free linked list based on Harris's algorithm.
// This implementation uses pointer-tagging (lowest bit) to mark logically deleted nodes.
template<typename T>
class LockFreeLinkedList {
private:
    struct Node {
        T value;
        std::atomic<Node*> next;
        Node(T val) : value(val), next(nullptr) {}
    };

    // Sentinel head node (with minimal value)
    Node* head;

    // Helper functions to manipulate mark bits in pointers.
    static Node* getMarked(Node* ptr) {
        return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(ptr) | 0x1);
    }
    static Node* getUnmarked(Node* ptr) {
        return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(ptr) & ~uintptr_t(0x1));
    }
    static bool isMarked(Node* ptr) {
        return (reinterpret_cast<uintptr_t>(ptr) & 0x1) != 0;
    }

public:
    LockFreeLinkedList() {
        // Create head and tail sentinel nodes.
        head = new Node(std::numeric_limits<T>::min());
        Node* tail = new Node(std::numeric_limits<T>::max());
        head->next.store(tail);
    }

    ~LockFreeLinkedList() {
        Node* curr = head;
        while (curr != nullptr) {
            Node* next = getUnmarked(curr->next.load());
            delete curr;
            curr = next;
        }
    }

    // Returns true if 'value' is in the list and the corresponding node is not marked.
    bool contains(const T& value) {
        Node* curr = head;
        while (curr->value < value) {
            Node* next = getUnmarked(curr->next.load());
            curr = next;
        }
        return (curr->value == value && !isMarked(curr->next.load()));
    }

    // Inserts value into the list.
    // Returns false if the value already exists.
    bool insert(const T& value) {
        while (true) {
            Node *pred, *curr;
            if (find(value, pred, curr)) {
                return false; // value already present
            }
            Node* newNode = new Node(value);
            newNode->next.store(curr);
            if (pred->next.compare_exchange_strong(curr, newNode)) {
                return true;
            }
            delete newNode; // CAS failed, retry
        }
    }

    // Removes value from the list.
    // Returns false if the value is not found.
    bool remove(const T& value) {
        Node *pred, *curr;
        while (true) {
            if (!find(value, pred, curr)) {
                return false; // not found
            }
            Node* succ = curr->next.load();
            // Logically mark the current node as deleted.
            if (!curr->next.compare_exchange_strong(succ, getMarked(succ))) {
                continue; // retry if marking fails
            }
            // Physically remove the node from the list.
            if (pred->next.compare_exchange_strong(curr, succ)) {
                delete curr;
            }
            return true;
        }
    }

    // Returns the number of elements in the list (excluding sentinels).
    int size() {
        int count = 0;
        Node* curr = getUnmarked(head->next.load());
        while (curr && curr->value != std::numeric_limits<T>::max()) {
            if (!isMarked(curr->next.load())) {
                count++;
            }
            curr = getUnmarked(curr->next.load());
        }
        return count;
    }

    // **추가**: 리스트가 올바른 오름차순이고 중복/마킹 노드가 없는지 검사
    //  - return true if valid, false otherwise
    bool validate() {
        Node* curr = head;
        T prevVal = curr->value; // should be -∞ for head
        curr = getUnmarked(curr->next.load());
        while (curr && curr->value != std::numeric_limits<T>::max()) {
            // 만약 현재 노드가 마킹되어 있으면, 논리 삭제됨 -> 무시해도 되지만
            // 여기서는 "정확성 테스트"를 위해 만약 삭제된 노드가 남아있으면 invalid로 본다.
            if (isMarked(curr->next.load())) {
                // 논문에서는 "물리삭제"를 통해 제거해야 한다고 하지만,
                // 여기서는 단순히 invalid 판정
                return false;
            }
            // 오름차순 정렬 검사
            if (curr->value <= prevVal) {
                return false;
            }
            prevVal = curr->value;
            curr = getUnmarked(curr->next.load());
        }
        return true;
    }

private:
    // Internal find() routine.
    // Searches for 'value' and sets pred to the last node with value < 'value'
    // and curr to the first node with value >= 'value'.
    // Returns true if curr->value == value.
    bool find(const T& value, Node*& pred, Node*& curr) {
        while (true) {
            pred = head;
            curr = getUnmarked(pred->next.load());
            while (true) {
                Node* succ = curr->next.load();
                // Physically remove marked nodes.
                while (isMarked(succ)) {
                    if (!pred->next.compare_exchange_strong(curr, getUnmarked(succ))) {
                        // CAS 실패 시 다시 처음부터
                        goto retry;
                    }
                    curr = getUnmarked(pred->next.load());
                    succ = curr->next.load();
                }
                if (curr->value >= value) {
                    return (curr->value == value);
                }
                pred = curr;
                curr = getUnmarked(curr->next.load());
            }
        retry:;
        }
        // (Unreachable)
        return false;
    }
};

#endif // LOCK_FREE_LINKED_LIST_H
