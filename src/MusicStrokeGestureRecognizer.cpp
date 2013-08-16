#include "SecondStudyApp.h"

#include "TouchTrace.h"
#include "Gesture.h"
#include "MusicStrokeGesture.h"

#include "MusicStrokeGestureRecognizer.h"

#include "cinder/Vector.h"

using namespace ci;
using namespace ci::app;
using namespace std;

SecondStudy::MusicStrokeGestureRecognizer::MusicStrokeGestureRecognizer(shared_ptr<list<shared_ptr<Gesture>>> gestures, shared_ptr<mutex> mtx) {
	_gestures = gestures;
	_gesturesMutex = mtx;
}

void SecondStudy::MusicStrokeGestureRecognizer::processGroup(list<shared_ptr<TouchTrace>> group) {
	if(group.size() == 1) {
		auto trace = group.front();
		TheApp *theApp = (TheApp *)App::get();
		TouchPoint a = trace->touchPoints.front();
		TouchPoint b = trace->touchPoints.back();
		
		Vec2f ap = theApp->tuioToWindow(a.getPos());
		Vec2f bp = theApp->tuioToWindow(b.getPos());
		
	}
}