#include <thread>
#include <stdio.h>
#include <vector>
#include <mutex>

#include "Tutorial.h"
void print() {
	static std::mutex myMutex;
	std::lock_guard<std::mutex> guard(myMutex);
	printf("Hello Thread\n");
	printf("I'm here...\n");
	printf("...not there.\n");
}

int main()
{
	Tutorial* m_pTutorial = new Tutorial();
	m_pTutorial->Run();

	std::vector<std::thread> threads;
	for (int i = 0; i < 50; i++) {
		threads.push_back(std::thread(print));
	}
	for (auto& thread : threads)
		thread.join();
	system("pause");
	return 0;
}