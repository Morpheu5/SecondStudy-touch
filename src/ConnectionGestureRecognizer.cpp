#include "ConnectionGestureRecognizer.h"

#include "TouchPoint.h"
#include "TouchTrace.h"
#include "ConnectionGesture.h"

#include "Widget.h"
#include "MeasureWidget.h"

#include "SecondStudyApp.h"

using namespace ci;
using namespace ci::app;
using namespace std;

SecondStudy::ConnectionGestureRecognizer::ConnectionGestureRecognizer(shared_ptr<list<shared_ptr<Gesture>>> gestures, shared_ptr<mutex> mtx) {
	_gestures = gestures;
	_gesturesMutex = mtx;
}

void SecondStudy::ConnectionGestureRecognizer::processGroup(list<shared_ptr<TouchTrace>> group) {
	if(group.size() == 1) {
		auto trace = group.front();
		TheApp *theApp = (TheApp *)App::get();
		TouchPoint a = trace->touchPoints.front();
		TouchPoint b = trace->touchPoints.back();
		
		Vec2f ap = theApp->tuioToWindow(a.getPos());
		Vec2f bp = theApp->tuioToWindow(b.getPos());
		
		theApp->widgetsMutex().lock();
		int fromWid = 0;
		for(auto w : theApp->widgets()) {
			if(w->id() == trace->widgetId) {
				if(dynamic_pointer_cast<MeasureWidget>(w)) {
					shared_ptr<MeasureWidget> tmp = dynamic_pointer_cast<MeasureWidget>(w);
					if(tmp->hitOutlet(ap)) {
						fromWid = tmp->id();
						tmp.reset();
						break;
					}
				}
			}
		}
		
		int toWid = 0;
		for(auto w : theApp->widgets()) {
			if(dynamic_pointer_cast<MeasureWidget>(w)) {
				shared_ptr<MeasureWidget> tmp = dynamic_pointer_cast<MeasureWidget>(w);
				if(tmp->hitInlet(bp)) {
					toWid = tmp->id();
					tmp.reset();
					break;
				}
			}
		}
		theApp->widgetsMutex().unlock();
		
		if(fromWid != 0 && toWid != 0 && fromWid != toWid) {
			shared_ptr<ConnectionGesture> g = make_shared<ConnectionGesture>(fromWid, toWid);
			_gesturesMutex->lock();
			_gestures->push_back(g);
			_gesturesMutex->unlock();
		}
	}
}