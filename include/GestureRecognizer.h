#pragma once

#include "Gesture.h"

using namespace std;

namespace SecondStudy {
	
	class TouchTrace;
	
	class GestureRecognizer {
	protected:
		shared_ptr<list<shared_ptr<Gesture>>> _gestures;
		shared_ptr<mutex> _gesturesMutex;

	public:
		GestureRecognizer() { }
		GestureRecognizer(shared_ptr<list<shared_ptr<Gesture>>> gestures, shared_ptr<mutex> mtx) :
			_gestures(gestures),
			_gesturesMutex(mtx) { }

		virtual void processGroup(list<shared_ptr<TouchTrace>> group) = 0;
	};

	class StaticGestureRecognizer : public GestureRecognizer {
	};

	class ProgressiveGestureRecognizer : public GestureRecognizer {
	};
}