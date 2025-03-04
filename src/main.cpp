#ifdef _WIN32
    #include <windows.h>
#else
    #include <thread>
#endif

#include <iostream>

#include "include/skip_list.h"

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

int main() {
    std::cout << "CS355 Final Project" << std::endl;
    std::cout << "Authors: Jin Park and WonHyeong Chae" << std::endl;
    std::cout << "Date: 2025-03-04" << std::endl;
    std::cout << "Description: This is a simple program to test the lock-free linked list and skip list implementations." << std::endl;

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

    return 0;
}
