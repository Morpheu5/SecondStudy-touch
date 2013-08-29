#include "DisconnectionGestureRecognizer.h"

#include "TouchPoint.h"
#include "TouchTrace.h"
#include "DisconnectionGesture.h"

#include "Widget.h"
#include "MeasureWidget.h"

#include "SecondStudyApp.h"

using namespace ci;
using namespace ci::app;
using namespace std;

SecondStudy::DisconnectionGestureRecognizer::DisconnectionGestureRecognizer(shared_ptr<list<shared_ptr<Gesture>>> gestures, shared_ptr<mutex> mtx) {
	_gestures = gestures;
	_gesturesMutex = mtx;
}

void SecondStudy::DisconnectionGestureRecognizer::processGroup(list<shared_ptr<TouchTrace>> group) {
	if(group.size() == 1) {
		auto trace = group.front();
		TheApp *theApp = (TheApp *)App::get();
		TouchPoint ap = trace->touchPoints.front();
		TouchPoint bp = trace->touchPoints.back();
		
		Vec2f a = theApp->tuioToWindow(ap.getPos());
		Vec2f b = theApp->tuioToWindow(bp.getPos());
		
		theApp->sequencesMutex().lock();
		
		for(auto sit = theApp->sequences().begin(); sit != theApp->sequences().end(); ++sit) {
			if(sit->size() > 1) {
				for(auto wit = sit->begin(); wit != prev(sit->end()); ++wit) {
					Vec2f fwpos = (*wit)->position();
					float fwang = (*wit)->angle();
					Vec2f twpos = (*next(wit))->position();
					float twang = (*next(wit))->angle();
					
					Vec3f olpos = Vec3f((*wit)->outletIcon().getCenter());
					Vec3f ilpos = Vec3f((*next(wit))->inletIcon().getCenter());
					
					Matrix44f fwt;
					fwt.translate(Vec3f(fwpos));
					fwt.rotate(Vec3f(0.0f, 0.0f, fwang));
					
					Vec3f olpost = fwt.transformPoint(olpos);
					Vec2f c = Vec2f(olpost.x, olpost.y);
					
					Matrix44f twt;
					twt.translate(Vec3f(twpos));
					twt.rotate(Vec3f(0.0f, 0.0f, twang));
					
					Vec3f ilpost = twt.transformPoint(ilpos);
					Vec2f d = Vec2f(ilpost.x, ilpost.y);
					
					float A1 = (a.y - b.y) / (a.x - b.x);
					float A2 = (c.y - d.y) / (c.x - d.x);
					float b1 = a.y - A1 * a.x;
					float b2 = c.y - A2 * c.x;
					
					if(abs(A1 - A2) > FLT_EPSILON) {
						float px = (b2 - b1) / (A1 - A2);
						Vec2f p(px, A1 * px + b1);
						
						// Now, to see if p is contained within both bounding boxes...
						Rectf ab(min(a.x, b.x), min(a.y, b.y), max(a.x, b.x), max(a.y, b.y));
						Rectf cd(min(c.x, d.x), min(c.y, d.y), max(c.x, d.x), max(c.y, d.y));
						if(ab.contains(p) && cd.contains(p)) {
							shared_ptr<DisconnectionGesture> g = make_shared<DisconnectionGesture>((*wit)->id(), (*next(wit))->id());
							_gesturesMutex->lock();
							_gestures->push_back(g);
							_gesturesMutex->unlock();
						}
					}
				}
			}
		}
		
		theApp->sequencesMutex().unlock();
	}
}