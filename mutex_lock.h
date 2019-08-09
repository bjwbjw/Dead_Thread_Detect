#ifndef NEW_SYS_MUTEX_LOCK_H
#define NEW_SYS_MUTEX_LOCK_H
#include <map>
#include <pthread.h>
#include <iostream>

//using namespace std;

std::map<unsigned long, unsigned long> g_map_tid_waitres_holdres;
class Mutex {
 public:
	Mutex() {
		hold_tid_ = 0;
		pthread_mutex_init(&session_, nullptr);
	}
	~Mutex() {
		pthread_mutex_destroy(&session_);
	}

 private:
	Mutex(const Mutex &);
	void operator=(const Mutex &);
	bool RecurCheckDeadlock(unsigned long, unsigned long,
				unsigned long, std::map<unsigned long, unsigned long> &);

 public:
	void lock();
	void unlock();

 public:
	pthread_mutex_t   session_;
	pthread_t         hold_tid_;
};

void Mutex::lock() {
	if (hold_tid_ != 0 && hold_tid_ != pthread_self()) {
		std::cout << "current thread:" << pthread_self()
				<< " want to get mutex locked by thread:" << hold_tid_ << std::endl;
		g_map_tid_waitres_holdres[pthread_self()] = hold_tid_;
	}

	auto map_tid_waitres_holdres = g_map_tid_waitres_holdres;
	if (RecurCheckDeadlock(pthread_self(), pthread_self(), hold_tid_, map_tid_waitres_holdres)) {
		std::cout << "current thread: " << pthread_self() << " deadlock" << std::endl;
	}
	pthread_mutex_lock(&session_);
	hold_tid_ = pthread_self();
	for (auto it = g_map_tid_waitres_holdres.begin();
						it != g_map_tid_waitres_holdres.end(); it++) {
		if (it->first == hold_tid_) {
			g_map_tid_waitres_holdres.erase(it);
			break;
		}
	}
}

void Mutex::unlock() {
    pthread_mutex_unlock(&session_);
    pthread_mutex_destroy(&session_);
	hold_tid_ = 0;
}

bool Mutex::RecurCheckDeadlock(
		unsigned long first_key,
		unsigned long key,
		unsigned long val,
		std::map<unsigned long, unsigned long> &map_tid_waitres_holdres) {
	bool ret = false;
	for (auto it = map_tid_waitres_holdres.begin();
	     it != map_tid_waitres_holdres.end(); it++) {
		if (key == it->first && val == it->second) {
			map_tid_waitres_holdres.erase(it);
			break;
		}
	}
	for (auto it = map_tid_waitres_holdres.begin();
	     it != map_tid_waitres_holdres.end(); it++) {
		if (it->first == val) {
			if (first_key == it->second)
				return true;
			return RecurCheckDeadlock(first_key, it->first, it->second, map_tid_waitres_holdres);
		}
	}
	return ret;
}

#endif //NEW_SYS_MUTEX_LOCK_H
