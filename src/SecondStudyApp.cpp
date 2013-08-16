#include "SecondStudyApp.h"

#include "MeasureWidget.h"
#include "TapGestureRecognizer.h"
#include "PinchGestureRecognizer.h"

#include "TouchPoint.h"
#include "TouchTrace.h"

#include "TapGesture.h"
#include "PinchGesture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void SecondStudy::TheApp::setup() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_tuioClient.registerCursorAdded(this, &SecondStudy::TheApp::cursorAdded);
	_tuioClient.registerCursorUpdated(this, &SecondStudy::TheApp::cursorUpdated);
	_tuioClient.registerCursorRemoved(this, &SecondStudy::TheApp::cursorRemoved);
		
	/*
	_tuioClient.registerObjectAdded(this, &SecondStudy::TheApp::objectAdded);
	_tuioClient.registerObjectUpdated(this, &SecondStudy::TheApp::objectUpdated);
	_tuioClient.registerObjectRemoved(this, &SecondStudy::TheApp::objectRemoved);
		*/
		
	_tuioClient.connect(); // Defaults to UDP:3333
		
	setFrameRate(FPS);
	setWindowSize(640, 480);

	//_widgets.push_back(make_shared<MeasureWidget>(0.20f * Vec2f(getWindowSize()), 5, 8));
	_widgets.push_back(make_shared<MeasureWidget>(0.50f * Vec2f(getWindowSize()), 5, 8));
	//_widgets.push_back(make_shared<MeasureWidget>(0.80f * Vec2f(getWindowSize()), 5, 8));

	_gesturesMutex = make_shared<mutex>();
	_gestures = make_shared<list<shared_ptr<Gesture>>>();
	
	//_widgetsMutex = make_shared<mutex>();
	//_widgets = make_shared<list<shared_ptr<Widget>>>();
		
	_staticGRs.push_back(make_shared<TapGestureRecognizer>(_gestures, _gesturesMutex));
	_progressiveGRs.push_back(make_shared<PinchGestureRecognizer>(_gestures, _gesturesMutex));

	// At last, fire up the gesture engine
	_gestureEngineShouldStop = false;
	_gestureEngine = thread(bind(&SecondStudy::TheApp::gestureEngine, this));

	_gestureProcessorShouldStop = false;
	_gestureProcessor = thread(bind(&SecondStudy::TheApp::gestureProcessor, this));

	go = false;
}

void SecondStudy::TheApp::shutdown() {
	_gestureEngineShouldStop = true;
	_gestureProcessorShouldStop = true;
	_gestureProcessor.join();
	_gestureEngine.join();
}

void SecondStudy::TheApp::mouseDown( MouseEvent event ) {
}

void SecondStudy::TheApp::keyDown(KeyEvent event) {
    switch(event.getChar()) {
        case KeyEvent::KEY_f: {
        setFullScreen(!isFullScreen());
        break;
    }
    default: {
        break;
    }
    }
}

void SecondStudy::TheApp::update() {
	//console() << gesture << " :: " << cursor << endl;
	_tracesMutex.lock();
	for(auto t : _traces) {
		t.second->update();
	}
	for(auto i = _traces.begin(); i != _traces.end(); ) {
		if(!i->second->isVisible && i->second->isDead()) {
			i = _traces.erase(i);
		} else {
			++i;
		}
	}
	_tracesMutex.unlock();

	_groupsMutex.lock();
	for(auto &g : _groups) {
		bool r = [g]() {
			for(auto t : g) {
				if(!t->isDead()) {
					return false;
				}
			}
			return true;
		}();
		if(r) {
			_removedGroupsMutex.lock();
			_removedGroups.push_back(g);
			_removedGroupsMutex.unlock();
			g.clear();
		}
	}
	_groups.erase(remove_if(_groups.begin(), _groups.end(),
		[](list<shared_ptr<TouchTrace>> l) {
			return l.empty();
	}), _groups.end()); // This is not much intuitive, maybe. It has to do with re-sorting stuff in a range.

    /*
	if(true) {
		for(auto &g : _groups) {
			for(auto pgr : _progressiveGRs) {
				pgr->processGroup(g);
			}
		}
		go = false;
	}
     */
	_groupsMutex.unlock();

	// this_thread::sleep_for(chrono::milliseconds(50));

	// Don't worry, this works. Unbelievable, though.
	// console() << _groups.size() << " groups" << endl;
	// console() << _traces.size() << " traces" << endl;
}

void SecondStudy::TheApp::draw() {
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

	_widgetsMutex.lock();
	for(auto w : _widgets) {
		w->draw();
	}
	_widgetsMutex.unlock();
		
	// Let's draw the traces as they are being created
	_tracesMutex.lock();
	_groupsMutex.lock();
	for(int i = 0; i < _groups.size(); i++) {
		for(auto trace : _groups[i]) {
			float h = (i%7)/7.0f;
			//console() << (i%7) << " :: " << h << endl;
			if(trace->isVisible) {
				gl::color(ColorAf(ColorModel::CM_HSV, h, 0.5f, 1.0f, 1.0f));
			} else {
				float c = trace->lifespan() / 30.0f;
				gl::color(ColorAf(ColorModel::CM_HSV, h, 0.5f, 1.0f, c));
			}
			if(trace->touchPoints.size() > 1) {
				for(auto cursorIt = trace->touchPoints.begin(); cursorIt != prev(trace->touchPoints.end()); ++cursorIt) {
					Vec2f a = tuioToWindow(cursorIt->getPos());
					Vec2f b = tuioToWindow(next(cursorIt)->getPos());
					gl::lineWidth(2.0f);
					gl::drawLine(a, b);
				}
			}
			if(trace->isVisible) {
				gl::color(ColorAf(ColorModel::CM_HSV, h, 0.5f, 1.0f, 0.25f));
				gl::drawSolidCircle(tuioToWindow(trace->currentPosition()), 8.0f);
				gl::drawSolidCircle(tuioToWindow(trace->currentPosition()), 50.0f);
			} else {
				gl::drawSolidCircle(tuioToWindow(trace->currentPosition()), 4.0f);
			}
		}
	}
	_groupsMutex.unlock();
	_tracesMutex.unlock();
}

void SecondStudy::TheApp::gestureEngine() {
	while(!_gestureEngineShouldStop) {
		// this_thread::sleep_for(chrono::milliseconds(50));
		// PROGRESSIVEs can be dealt with using a signal. For now I'll keep the thing in the methods below

		// STATIC
		list<shared_ptr<TouchTrace>> group;
		_removedGroupsMutex.lock();
		if(_removedGroups.size() > 0) {
			group = _removedGroups.front();
			_removedGroups.pop_front();
		}
		_removedGroupsMutex.unlock();

		if(group.size() > 0) {
			for(auto sgr : _staticGRs) {
				sgr->processGroup(group);
			}
		}
	}
}

void SecondStudy::TheApp::gestureProcessor() {
	while(!_gestureProcessorShouldStop) {
		// this_thread::sleep_for(chrono::milliseconds(50));
		//console() << "Gestures: " << _gestures->size() << endl;
		if(_gestures->size() > 0) {
			_gesturesMutex->lock();
			// This may very well be leaking stuff around. 'Tis no good.
			shared_ptr<Gesture> unknownGesture = _gestures->front();
			_gestures->pop_front();
			_gesturesMutex->unlock();

			if(dynamic_pointer_cast<TapGesture>(unknownGesture)) {
				shared_ptr<TapGesture> tap = dynamic_pointer_cast<TapGesture>(unknownGesture);
				//console() << "GP >> TAP (" << tap->position().x << ", " << tap->position().y << ")" << endl;
				if(tap->isOnWidget()) {
					_widgetsMutex.lock();
					auto wIt = find_if(_widgets.begin(), _widgets.end(),
						[&, this](shared_ptr<Widget> w) {
							return w->id() == tap->widgetId();
						});
					if(wIt != _widgets.end()) {
						auto w = *wIt;
						w->tap(tap->position());
					}
					_widgetsMutex.unlock();
				}
			}

			if(dynamic_pointer_cast<PinchGesture>(unknownGesture)) {
				shared_ptr<PinchGesture> pinch = dynamic_pointer_cast<PinchGesture>(unknownGesture);
				//console() << "GP >> PINCH" << endl;
				if(pinch->isOnWidget()) {
					_widgetsMutex.lock();
					auto wIt = find_if(_widgets.begin(), _widgets.end(),
						[&, this](shared_ptr<Widget> w) {
							return w->id() == pinch->widgetId();
						});
					if(wIt != _widgets.end()) {
						auto w = *wIt;
						//console() << "   >> DD >> " << pinch->distanceDelta().length() << endl;
						w->moveBy(pinch->distanceDelta());
                        w->zoomBy(pinch->zoomDelta());
                        w->rotateBy(pinch->angleDelta());
					}
					_widgetsMutex.unlock();
				}
			}

			//console() << "unknownGesture.use_count() == " << unknownGesture.use_count() << endl;
		}
	}
}
	
void SecondStudy::TheApp::cursorAdded(tuio::Cursor cursor) {
	bool continued = false;
	int joined = -1;
	_tracesMutex.lock();
	for(auto t : _traces) {
		auto trace = t.second;
		if(!trace->isVisible && !trace->isDead() && tuioToWindow(trace->currentPosition()).distance(tuioToWindow(cursor.getPos())) <= 50.0f) {
			_traces[cursor.getSessionId()] = _traces[trace->getSessionId()];
			joined = trace->getSessionId();
			_traces[cursor.getSessionId()]->resurrect();
			_traces[cursor.getSessionId()]->cursorMove(cursor);
			continued = true;
			break;
		}
	}

	if(continued) {
		if(joined > 0) {
			_traces.erase(joined);
		}
	} else {
		// This is a brand new trace, we have to do stuff!
		_traces[cursor.getSessionId()] = make_shared<TouchTrace>();
		_traces[cursor.getSessionId()]->addCursorDown(cursor);

		// Check if it's on a widget
		_widgetsMutex.lock();
		// This is done in reverse order because I say so.
		for(auto it = _widgets.rbegin(); it != _widgets.rend(); ++it) {
			auto w = *it;
			if(w->hit(tuioToWindow(cursor.getPos()))) {
				_traces[cursor.getSessionId()]->widgetId = w->id();
				break;
			}
		}
		_widgetsMutex.unlock();

		_groupsMutex.lock();
		int g = findGroupForTrace(_traces[cursor.getSessionId()]);
		if(g == -1) {
			list<shared_ptr<TouchTrace>> l;
			l.push_back(_traces[cursor.getSessionId()]);
			_groups.push_back(l);
		} else {
			_groups[g].push_back(_traces[cursor.getSessionId()]);
		}
		_groupsMutex.unlock();
	}
	_tracesMutex.unlock();
    
    for(auto &g : _groups) {
        for(auto pgr : _progressiveGRs) {
            pgr->processGroup(g);
        }
    }
}
	
void SecondStudy::TheApp::cursorUpdated(tuio::Cursor cursor) {
	go = true;
	_tracesMutex.lock();
	_traces[cursor.getSessionId()]->cursorMove(cursor);
	_tracesMutex.unlock();
    
    for(auto &g : _groups) {
        for(auto pgr : _progressiveGRs) {
            pgr->processGroup(g);
        }
    }
}
	
void SecondStudy::TheApp::cursorRemoved(tuio::Cursor cursor) {
	go = true;
	_tracesMutex.lock();
	_traces[cursor.getSessionId()]->addCursorUp(cursor);
	_traces[cursor.getSessionId()]->isVisible = false;
	_tracesMutex.unlock();
    
    for(auto &g : _groups) {
        for(auto pgr : _progressiveGRs) {
            pgr->processGroup(g);
        }
    }
}

int SecondStudy::TheApp::findGroupForTrace(shared_ptr<TouchTrace> trace) {
	// First: group traces on the same widget
	for(int i = 0; i < _groups.size(); i++) {
		for(auto otherTrace : _groups[i]) {
			if(trace->widgetId == otherTrace->widgetId && otherTrace->widgetId != 0) {
				return i;
			}
		}
	}

	// Second: if that failed, group with nearby traces
	for(int i = 0; i < _groups.size(); i++) {
		auto traces = _groups[i];
		for(auto otherTrace : traces) {
			if(tuioToWindow(otherTrace->currentPosition()).distance(tuioToWindow(trace->currentPosition())) < 50.0f) {
				return i;
			}
		}
	}

	// Last: if all previous grouping attempts failed, just give up
	return -1;
}

CINDER_APP_NATIVE( SecondStudy::TheApp, RendererGl )
