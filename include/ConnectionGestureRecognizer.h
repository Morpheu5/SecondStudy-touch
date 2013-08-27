#pragma once

#include "GestureRecognizer.h"

using namespace std;

namespace SecondStudy {
	
	class Gesture;
	class TouchTrace;
	
	class ConnectionGestureRecognizer : public StaticGestureRecognizer {
	public:
		ConnectionGestureRecognizer(shared_ptr<list<shared_ptr<Gesture>>> gestures, shared_ptr<mutex> mtx);
		
		void processGroup(list<shared_ptr<TouchTrace>> group);
	};
	
}