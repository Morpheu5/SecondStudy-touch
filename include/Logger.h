#pragma once

#include <fstream>
#include <sstream>
#include <list>
#include <mutex>
#include <chrono>

using namespace std;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::steady_clock;

namespace SecondStudy {
	
	class Logger {
		string _filename;
		ofstream _logFile;
		
		list<string> _logQueue;
		mutex _logQueueMutex;
		
		bool _run;
		
	public:
		Logger() { }
		
		void init(string f) {
			_run = true;
			_filename = f;
			
			_logFile = ofstream(f, std::ios::out);
		}
		
		void log(string s) {
			_logQueueMutex.lock();

			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock::now().time_since_epoch()).count();

			stringstream t("");
			t << "[" << ms << "] " << s << endl;
			
			_logQueue.push_back(t.str());
			
			_logQueueMutex.unlock();
		}
		
		void stop() {
			_run = false;
			_logFile.close();
		}
		
		void run() {
			while(_run) {
				_logQueueMutex.lock();
				for(string s : _logQueue) {
					_logFile << s;
					_logFile.flush();
				}
				_logQueue.clear();
				_logQueueMutex.unlock();
			}
		}
		
		static Logger& instance() {
			static Logger _instance;
			return _instance;
		}
	};
}