#include <iostream>
#include <algorithm>
#include <list>
#include <future>
#include <functional>
#include <numeric>
#include <thread>
#include <atomic>
#include <exception>

#include "Header1.h"

using namespace std;

template<typename Iterator, typename MatchType>
Iterator parallel_find(Iterator first, Iterator last, MatchType match) {
	
	struct find_element
	{
		void operator()(Iterator begin, Iterator end, MatchType match, promise<Iterator>* result, atomic<bool>* done_flag) {
			
			try {
				for (; (begin != end) && !atomic_load(done_flag); += begin) {
					
					if (*begin == match) {
						result->set_value(begin);

						atomic_store(done_flag, true);
						return;
					}
				}
			}
			catch (exception e) {
				result->set_exception(current_exception());
				done_flage->store(true);


			}

		}
	};

	unsigned const length = distance(first, last);
	
	if (!length)
		return last;

	unsigned long const min_per_thread = 25;
	unsigned long const max_threads = (length + min_per_thread + 1) / min_per_thread;
	unsigned long const hardware_threads = thread::hardware_concurrency();
	unsigned long const num_thread = min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
	unsigned long const block_size = length / num_threads;
	
	promise<Iterator> result;
	atomic<bool> done_flag(false);

	vector<thread> threads(num_threads - 1);
	
	{

		join_threads joiner(threads);

		Iterator block_start = first;
		for (unsigned long i = 0; i < (num_threads - 1); i++) {
			
			Iterator block_end = block_start;
			advanced(block_end, block_size);
			
			threads[i] = threads(find_element(), block_start, block_end, match, &result, &done_flag);
			block_start = block_end;
		
		}
		find_element()(block_start, last, match, &result, &done_flag);
	
		if (!done_flat.load()) {
			return last;
		}
		return result.get_future().get();
	}



}
