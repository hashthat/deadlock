#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <barrier>
#include <atomic>
// Mutex
// https://www.tutorialspoint.com/operating_system/mutex_in_os.htm
// 
// barrier
// https://en.cppreference.com/cpp/thread/barrier
//
// atomic
// https://en.cppreference.com/cpp/atomic/atomic
// 
// Chrono
// https://www.geeksforgeeks.org/cpp/chrono-in-c/

using namespace std;

// Using two shared resources

mutex mtx_A;
mutex mtx_B;

atomic<bool> thread1{false};
atomic<bool> thread2{false};

// barrier sync_point(2); (Example)

void task1() {
	// Thread 1 acquires mtx_A.
	mtx_A.lock();
	cout << "Thread 1 aquired mtx_A\n";

	this_thread::sleep_for(chrono::seconds(3));
	
	
	thread1 = true;

	while (!thread2.load()) {
		this_thread::yield();
	}
	
	
	// waiting for thread 2 to aquire mtx_B
	// sync_point.arrive_and_wait();

	// DEADLOCK
	cout << "Thread 1 waiting for mtx_B\n";
	mtx_B.lock();
	// the clever piece of this exersize is that thread 1 will never be complete
	// as the program is deadlocked where each resource is occupied.
	// cout << "Thread 1 completed!\n";

	mtx_B.unlock();
	mtx_A.unlock();
}

void task2() {
	mtx_B.lock();

	cout << "Thread 2 aquired mtx_B\n";

	this_thread::sleep_for(chrono::seconds(3));
	// wait until thread 1 aquired mtx_A
	// sync_point.arrive_and_wait();
	
	// DEADLOCK
	
	thread2 = true;

	while (!thread1.load()) {
		this_thread::yield();
	}
	cout << "Thread 2 waiting for mtx_A\n";
	mtx_A.lock();
	// program is deadlocked, therefore the program will never actually execute.
	// cout << "Thread 2 completed!\n";

	mtx_A.unlock();
	mtx_B.unlock();
}

int main()
{
	thread t1(task1);
	thread t2(task2);
	// Join never returns as the threads are currently blocked.
	
	t1.join();
	t2.join();

	return 0;
}



