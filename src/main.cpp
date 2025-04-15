#ifdef _WIN32
    #include <windows.h>
#else
    #include <thread>
#endif

#include <iostream>
#include <chrono>
#include <cstdlib>

#include "include/skip_list.h"
#include "include/linked_list.h"

#ifdef _WIN32
    DWORD WINAPI thread1(LPVOID param) {
        SkipList<int>* skipList = static_cast<SkipList<int>*>(param);
        for (int i = 0; i < 1000; i++) {
            skipList->insert(i);
        }
        return 0;
    }

    DWORD WINAPI thread2(LPVOID param) {
        SkipList<int>* skipList = static_cast<SkipList<int>*>(param);
        for (int i = 1000; i < 2000; i++) {
            skipList->insert(i);
        }
        return 0;
    }

    DWORD WINAPI thread3(LPVOID param) {
        SkipList<int>* skipList = static_cast<SkipList<int>*>(param);
        for (int i = 0; i < 1000; i++) {
            skipList->remove(i);
        }
        return 0;
    }
#else
    void thread1(SkipList<int>* skipList) {
        for (int i = 0; i < 1000; i++) {
            skipList->insert(i);
        }
    }

    void thread2(SkipList<int>* skipList) {
        for (int i = 1000; i < 2000; i++) {
            skipList->insert(i);
        }
    }

    void thread3(SkipList<int>* skipList) {
        for (int i = 0; i < 1000; i++) {
            skipList->remove(i);
        }
    }
#endif

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
    DWORD WINAPI insertRange(LPVOID param) {
        auto* p = reinterpret_cast<std::pair<LockFreeLinkedList<int>*, std::pair<int,int>>*>(param);
        LockFreeLinkedList<int>* ll = p->first;
        int startVal = p->second.first;
        int endVal   = p->second.second;
        for (int i = startVal; i < endVal; i++) {
            ll->insert(i);
        }
        return 0;
    }
#else
    void insertRange(LockFreeLinkedList<int>* ll, int startVal, int endVal) {
        for (int i = startVal; i < endVal; i++) {
            ll->insert(i);
        }
    }
#endif

int main(int argc, char* argv[]) {
    std::cout << "CS355 Final Project" << std::endl;
    std::cout << "Authors: Jin Park and WonHyeong Chae" << std::endl;
    std::cout << "Date: 2025-03-04" << std::endl;
    std::cout << "Description: This is a simple program to test the lock-free linked list and skip list implementations." << std::endl;

    {

    // Sample test codes with multi-processed skip list
    SkipList<int> skipList;
    
#ifdef _WIN32
    HANDLE h1 = CreateThread(NULL, 0, thread1, &skipList, 0, NULL);
    HANDLE h2 = CreateThread(NULL, 0, thread2, &skipList, 0, NULL);
    
    WaitForSingleObject(h1, INFINITE);
    WaitForSingleObject(h2, INFINITE);

    std::cout << "Skip list size: " << skipList.size() << std::endl;
    std::cout << "Contains 1500: " << skipList.contains(1500) << std::endl;
    std::cout << "Contains 2500: " << skipList.contains(2500) << std::endl;
    
    // Test remove operation
    HANDLE h3 = CreateThread(NULL, 0, thread3, &skipList, 0, NULL);
    WaitForSingleObject(h3, INFINITE);
    
    std::cout << "Skip list size after removal: " << skipList.size() << std::endl;
    std::cout << "Contains 1500 after removal: " << skipList.contains(1500) << std::endl;
    std::cout << "Contains 2500 after removal: " << skipList.contains(2500) << std::endl;

    CloseHandle(h1);
    CloseHandle(h2);
    CloseHandle(h3);
#else
    std::thread t1(thread1, &skipList);
    std::thread t2(thread2, &skipList);
    
    t1.join();
    t2.join();

    std::cout << "Skip list size: " << skipList.size() << std::endl;
    std::cout << "Contains 1500: " << skipList.contains(1500) << std::endl;
    std::cout << "Contains 2500: " << skipList.contains(2500) << std::endl;
    
    // Test remove operation
    std::thread t3(thread3, &skipList);
    t3.join();
    
    std::cout << "Skip list size after removal: " << skipList.size() << std::endl;
    std::cout << "Contains 1500 after removal: " << skipList.contains(1500) << std::endl;
    std::cout << "Contains 2500 after removal: " << skipList.contains(2500) << std::endl;
#endif
    }

// --------------------- [2] Jin Park: Lock-Free Linked List "simple test" ---------------------

{
    std::cout << "\n[Lock-Free Linked List Test - Basic]\n";
    LockFreeLinkedList<int> ll;
#ifdef _WIN32
    HANDLE ll_h1 = CreateThread(NULL, 0, thread1_ll, &ll, 0, NULL);
    HANDLE ll_h2 = CreateThread(NULL, 0, thread2_ll, &ll, 0, NULL);

    WaitForSingleObject(ll_h1, INFINITE);
    WaitForSingleObject(ll_h2, INFINITE);

    std::cout << "Linked List size: " << ll.size() << std::endl;
    std::cout << "Contains 1500: " << (ll.contains(1500) ? "Yes" : "No") << std::endl;
    std::cout << "Contains 2500: " << (ll.contains(2500) ? "Yes" : "No") << std::endl;

    // Test remove operation for linked list
    HANDLE ll_h3 = CreateThread(NULL, 0, thread3_ll, &ll, 0, NULL);
    WaitForSingleObject(ll_h3, INFINITE);

    std::cout << "Linked List size after removal: " << ll.size() << std::endl;
    std::cout << "Contains 1500 after removal: " << (ll.contains(1500) ? "Yes" : "No") << std::endl;
    std::cout << "Contains 2500 after removal: " << (ll.contains(2500) ? "Yes" : "No") << std::endl;

    CloseHandle(ll_h1);
    CloseHandle(ll_h2);
    CloseHandle(ll_h3);
#else
    std::thread ll_t1(thread1_ll, &ll);
    std::thread ll_t2(thread2_ll, &ll);

    ll_t1.join();
    ll_t2.join();

    std::cout << "Linked List size: " << ll.size() << std::endl;
    std::cout << "Contains 1500: " << (ll.contains(1500) ? "Yes" : "No") << std::endl;
    std::cout << "Contains 2500: " << (ll.contains(2500) ? "Yes" : "No") << std::endl;

    // Test remove operation for linked list
    std::thread ll_t3(thread3_ll, &ll);
    ll_t3.join();

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
        numThreads = std::atoi(argv[1]);
    }
    if (argc >= 3) {
        numRange = std::atoi(argv[2]);
    }

    std::cout << "\n[Lock-Free Linked List Stress Test]\n";
    std::cout << "Using " << numThreads << " threads, range " << numRange << std::endl;

    LockFreeLinkedList<int> ll;

    auto startTime = std::chrono::steady_clock::now();

#ifdef _WIN32
    std::vector<HANDLE> handles;
    handles.reserve(numThreads);
    int chunkSize = numRange / numThreads;
    for(int i = 0; i < numThreads; i++) {
        int st = i * chunkSize;
        int ed = (i == numThreads-1) ? numRange : (i+1)*chunkSize;
        auto* rangeInfo = new std::pair<LockFreeLinkedList<int>*, std::pair<int,int>>{ &ll, {st, ed} };
        HANDLE h = CreateThread(NULL, 0, insertRange, rangeInfo, 0, NULL);
        handles.push_back(h);
    }
    for (auto& h : handles) {
        WaitForSingleObject(h, INFINITE);
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
        th.join();
    }
#endif

    auto endTime = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << "Insertion done, total size: " << ll.size() << std::endl;
    std::cout << "Elapsed time: " << ms << " ms" << std::endl;
    std::cout << "[Validate] " << (ll.validate() ? "OK" : "FAIL") << std::endl;
}

    return 0;
}
