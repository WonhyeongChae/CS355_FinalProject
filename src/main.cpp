#ifdef _WIN32
    #include <windows.h>
    #include <thread>
    #include <vector> // Need vector for HANDLE storage
#else
    #include <thread>
    #include <vector> // Need vector for std::thread storage
#endif

#include <iostream>
#include <chrono>
#include <cstdlib>
#include <numeric>

#include "skip_list.h"
#include "linked_list.h"

// --- Linked List Worker Functions (Keep them) ---
#ifdef _WIN32
    DWORD WINAPI thread1_ll(LPVOID param) {
        LockFreeLinkedList<int>* ll = static_cast<LockFreeLinkedList<int>*>(param);
        for (int i = 0; i < 1000; i++) {
            ll->insert(i);
        }
        return 0;
    }

    DWORD WINAPI thread2_ll(LPVOID param) {
        LockFreeLinkedList<int>* ll = static_cast<LockFreeLinkedList<int>*>(param);
        for (int i = 1000; i < 2000; i++) {
            ll->insert(i);
        }
        return 0;
    }

    DWORD WINAPI thread3_ll(LPVOID param) {
        LockFreeLinkedList<int>* ll = static_cast<LockFreeLinkedList<int>*>(param);
        for (int i = 0; i < 1000; i++) {
            ll->remove(i);
        }
        return 0;
    }
#else
    void thread1_ll(LockFreeLinkedList<int>* ll) {
        for (int i = 0; i < 1000; i++) {
            ll->insert(i);
        }
    }

    void thread2_ll(LockFreeLinkedList<int>* ll) {
        for (int i = 1000; i < 2000; i++) {
            ll->insert(i);
        }
    }

    void thread3_ll(LockFreeLinkedList<int>* ll) {
        for (int i = 0; i < 1000; i++) {
            ll->remove(i);
        }
    }
#endif

#ifdef _WIN32
    // Structure for passing data to insertRange for linked list
    struct LinkedListWorkerData {
        LockFreeLinkedList<int>* list;
        int startVal;
        int endVal;
    };
    DWORD WINAPI insertRange(LPVOID param) {
        auto* data = static_cast<LinkedListWorkerData*>(param);
        LockFreeLinkedList<int>* ll = data->list;
        int startVal = data->startVal;
        int endVal   = data->endVal;
        for (int i = startVal; i < endVal; i++) {
            ll->insert(i);
        }
        delete data; // Clean up allocated data
        return 0;
    }
#else
    void insertRange(LockFreeLinkedList<int>* ll, int startVal, int endVal) {
        for (int i = startVal; i < endVal; i++) {
            ll->insert(i);
        }
    }
#endif
// --- End Linked List Worker Functions ---

const int NUM_THREADS = 4;
const int NUM_OPS = 10000;

std::atomic<int> insert_count{0};

void test_single_thread() {
    LockFreeSkipList list;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_OPS; ++i) {
        list.insert(i, i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Single-threaded insert: " << duration.count() << "s\n";
}

void test_multi_thread() {
    LockFreeSkipList list;
    std::vector<std::thread> threads;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&list]() {
            for (int j = 0; j < NUM_OPS; ++j) {
                int key = insert_count.fetch_add(1, std::memory_order_relaxed);
                list.insert(key, key);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Multi-threaded insert: " << duration.count() << "s\n";
}

int main(int argc, char* argv[]) {
    std::cout << "CS355 Final Project" << std::endl;
    std::cout << "Authors: Jin Park and Wonhyeong Chae" << std::endl;
    std::cout << "Date: " << __DATE__ << std::endl; // Use compiler macro for date
    std::cout << "Description: This is a simple program to test the lock-free linked list and skip list implementations." << std::endl;

    test_single_thread();
    test_multi_thread();

    // --- Linked List Tests (Keep them) ---
    // --------------------- [2] Jin Park: Lock-Free Linked List "simple test" ---------------------
    {
        std::cout << "\n[Lock-Free Linked List Test - Basic]\n";
        LockFreeLinkedList<int> ll;
    #ifdef _WIN32
        HANDLE ll_h1 = CreateThread(NULL, 0, thread1_ll, &ll, 0, NULL);
        HANDLE ll_h2 = CreateThread(NULL, 0, thread2_ll, &ll, 0, NULL);

        if (ll_h1) WaitForSingleObject(ll_h1, INFINITE);
        if (ll_h2) WaitForSingleObject(ll_h2, INFINITE);

        std::cout << "Linked List size: " << ll.size() << std::endl;
        std::cout << "Contains 1500: " << (ll.contains(1500) ? "Yes" : "No") << std::endl;
        std::cout << "Contains 2500: " << (ll.contains(2500) ? "Yes" : "No") << std::endl;

        // Test remove operation for linked list
        HANDLE ll_h3 = CreateThread(NULL, 0, thread3_ll, &ll, 0, NULL);
        if (ll_h3) WaitForSingleObject(ll_h3, INFINITE);

        std::cout << "Linked List size after removal: " << ll.size() << std::endl;
        std::cout << "Contains 1500 after removal: " << (ll.contains(1500) ? "Yes" : "No") << std::endl;
        std::cout << "Contains 2500 after removal: " << (ll.contains(2500) ? "Yes" : "No") << std::endl;

        if (ll_h1) CloseHandle(ll_h1);
        if (ll_h2) CloseHandle(ll_h2);
        if (ll_h3) CloseHandle(ll_h3);
    #else
        std::thread ll_t1(thread1_ll, &ll);
        std::thread ll_t2(thread2_ll, &ll);

        if(ll_t1.joinable()) ll_t1.join();
        if(ll_t2.joinable()) ll_t2.join();

        std::cout << "Linked List size: " << ll.size() << std::endl;
        std::cout << "Contains 1500: " << (ll.contains(1500) ? "Yes" : "No") << std::endl;
        std::cout << "Contains 2500: " << (ll.contains(2500) ? "Yes" : "No") << std::endl;

        // Test remove operation for linked list
        std::thread ll_t3(thread3_ll, &ll);
        if(ll_t3.joinable()) ll_t3.join();

        std::cout << "Linked List size after removal: " << ll.size() << std::endl;
        std::cout << "Contains 1500 after removal: " << (ll.contains(1500) ? "Yes" : "No") << std::endl;
        std::cout << "Contains 2500 after removal: " << (ll.contains(2500) ? "Yes" : "No") << std::endl;
    #endif

        std::cout << "[Validate] " << (ll.validate() ? "OK" : "FAIL") << std::endl;
    }

    // --------------------- [3] Jin Park: Lock-Free Linked List "stress test based on parameter" ---------------------
    {
        int numThreads = 4;
        int numRange   = 50000; // ex: 0 ~ 49999 range

        // ex: ./out.exe 8 100000 ...)
        if (argc >= 2) {
            try { // Add basic error checking for atoi
                numThreads = std::atoi(argv[1]);
                if (numThreads <= 0) numThreads = 4; // Default if invalid
            } catch (...) { numThreads = 4; }
        }
        if (argc >= 3) {
             try {
                numRange = std::atoi(argv[2]);
                if (numRange <= 0) numRange = 50000; // Default if invalid
            } catch (...) { numRange = 50000; }
        }

        std::cout << "\n[Lock-Free Linked List Stress Test]\n";
        std::cout << "Using " << numThreads << " threads, range " << numRange << std::endl;

        LockFreeLinkedList<int> ll;

        auto startTime = std::chrono::high_resolution_clock::now(); // Use high_resolution_clock

    #ifdef _WIN32
        std::vector<HANDLE> handles;
        handles.reserve(numThreads);
        int chunkSize = numRange / numThreads;
        for(int i = 0; i < numThreads; i++) {
            int st = i * chunkSize;
            int ed = (i == numThreads-1) ? numRange : (i+1)*chunkSize;
            // IMPORTANT: Allocate data on heap for each thread
            auto* rangeInfo = new LinkedListWorkerData{ &ll, st, ed };
            HANDLE h = CreateThread(NULL, 0, insertRange, rangeInfo, 0, NULL);
            if (h != NULL) {
                handles.push_back(h);
            } else {
                 std::cerr << "Error creating thread " << i << std::endl;
                 delete rangeInfo; // Clean up if thread creation failed
            }
        }
        if (!handles.empty()) {
            WaitForMultipleObjects(static_cast<DWORD>(handles.size()), handles.data(), TRUE, INFINITE);
        }
        for (auto& h : handles) {
            CloseHandle(h);
        }
    #else
        std::vector<std::thread> threads;
        threads.reserve(numThreads);
        int chunkSize = numRange / numThreads;
        for (int i = 0; i < numThreads; i++) {
            int st = i * chunkSize;
            int ed = (i == numThreads-1) ? numRange : (i+1)*chunkSize;
            threads.emplace_back(insertRange, &ll, st, ed);
        }
        for (auto &th : threads) {
            if(th.joinable()) th.join();
        }
    #endif

        auto endTime = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        std::cout << "Insertion done, total size: " << ll.size() << std::endl;
        std::cout << "Elapsed time: " << ms << " ms" << std::endl;
        std::cout << "[Validate] " << (ll.validate() ? "OK" : "FAIL") << std::endl;
    }

    return 0;
}
