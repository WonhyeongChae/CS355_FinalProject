#include "include/skip_list.h"
#include <vector>

template<typename T>
bool SkipList<T>::insert(const T& value) {
    std::vector<Node*> update(MAX_LEVEL + 1);
    Node* current = head;

    // Find the position to insert the new node
    for (int i = level; i >= 0; i--) {
        Node* next = current->next[i].load();
        while (next != nullptr && next->value < value) {
            current = next;
            next = current->next[i].load();
        }
        update[i] = current;
    }

    // If the value already exists, return false
    Node* next = current->next[0].load();
    if (next != nullptr && next->value == value) {
        return false;
    }

    // Generate random level for the new node
    int newLevel = randomLevel();
    if (newLevel > level) {
        for (int i = level + 1; i <= newLevel; i++) {
            update[i] = head;
        }
        level = newLevel;
    }

    // Create and insert the new node
    Node* newNode = new Node(value, newLevel);
    for (int i = 0; i <= newLevel; i++) {
        Node* next = update[i]->next[i].load();
        newNode->next[i].store(next);
        update[i]->next[i].store(newNode);
    }

    return true;
}

template<typename T>
bool SkipList<T>::remove(const T& value) {
    std::vector<Node*> update(MAX_LEVEL + 1);
    Node* current = head;

    // Find the node to remove
    for (int i = level; i >= 0; i--) {
        Node* next = current->next[i].load();
        while (next != nullptr && next->value < value) {
            current = next;
            next = current->next[i].load();
        }
        update[i] = current;
    }

    // If the value doesn't exist, return false
    Node* next = current->next[0].load();
    if (next == nullptr || next->value != value) {
        return false;
    }

    // Remove the node
    Node* nodeToDelete = next;
    for (int i = 0; i <= level; i++) {
        Node* next = update[i]->next[i].load();
        if (next != nodeToDelete) {
            break;
        }
        Node* nextNext = nodeToDelete->next[i].load();
        update[i]->next[i].store(nextNext);
    }

    // Update the level if necessary
    while (level > 0 && head->next[level].load() == nullptr) {
        level--;
    }

    delete nodeToDelete;
    return true;
}

template<typename T>
bool SkipList<T>::contains(const T& value) const {
    Node* current = head;

    // Search for the value
    for (int i = level; i >= 0; i--) {
        Node* next = current->next[i].load();
        while (next != nullptr && next->value < value) {
            current = next;
            next = current->next[i].load();
        }
        if (next != nullptr && next->value == value) {
            return true;
        }
    }

    return false;
}

template <typename T>
int SkipList<T>::size() const
{
    int count = 0;
    Node* current = head->next[0].load();
    
    while (current != tail) {
        count++;
        current = current->next[0].load();
    }
    
    return count;
}

// Explicit template instantiations for common types
template class SkipList<int>;
template class SkipList<float>;
template class SkipList<double>;
template class SkipList<std::string>;
