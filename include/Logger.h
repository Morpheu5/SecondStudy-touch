/*
 Copyright (c) 2013, Andrea Franceschini <andrea.franceschini@gmail.com>
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of The Open University (Milton Keynes, UK) nor the names of
   its contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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