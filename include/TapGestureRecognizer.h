#pragma once

#include "SecondStudyApp.h"

#include "TouchTrace.h"
#include "GestureRecognizer.h"
#include "Gesture.h"
#include "TapGesture.h"

#include "cinder/Vector.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace SecondStudy {
	
	class TapGestureRecognizer : public StaticGestureRecognizer {
	public:
		TapGestureRecognizer(shared_ptr<list<shared_ptr<Gesture>>> gestures, shared_ptr<mutex> mtx);

		void processGroup(list<shared_ptr<TouchTrace>> group);
	};

}