#include <iostream>
#include <chrono>
#include <thread>

#include "thread_entites.hpp"


void functions::units::thread() {
	while (true) {
		try {
		//	functions::entites::third_person();
		}
		catch (const std::exception& e) {
			std::cerr << "Exception in thread_entites::thread: " << e.what() << std::endl;
		}
		catch (...) {
			std::cerr << "Unknown exception in thread_entites::thread" << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
	}
}