#include "SecondStudyApp.h"

#include "TouchPoint.h"
#include "TouchTrace.h"

#include "MeasureWidget.h"

#include "TapGestureRecognizer.h"
#include "PinchGestureRecognizer.h"
#include "MusicStrokeGestureRecognizer.h"
#include "ConnectionGestureRecognizer.h"
#include "DisconnectionGestureRecognizer.h"
#include "LongTapGestureRecognizer.h"

#include "TapGesture.h"
#include "PinchGesture.h"
#include "MusicStrokeGesture.h"
#include "ConnectionGesture.h"
#include "DisconnectionGesture.h"
#include "LongTapGesture.h"

#include "Logger.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void SecondStudy::TheApp::setup() {
	auto filepath = getAppPath().remove_filename()/"logs";
	if(!fs::exists(filepath)) {
		fs::create_directories(filepath);
	}
	std::time_t t = std::time(nullptr);
	stringstream fn;
	fn << "SecondStudy_" << std::put_time(std::localtime(&t), "%Y-%m-%d_%H-%M-%S") << ".log";
	filepath /= fn.str();
	Logger::instance().init(filepath.string());
	Logger::instance().log("SecondStudy starting up...");
	
	_loggerThread = thread(bind(&Logger::run, &(Logger::instance())));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	auto renderer = std::static_pointer_cast<RendererGl>(getRenderer());
	//renderer->setAntiAliasing(0);
	
	stringstream ss;
	ss << "AA set to " << renderer->getAntiAliasing();
	Logger::instance().log(ss.str());
	console() << ss.str() << endl;

	_tuioClient.registerCursorAdded(this, &SecondStudy::TheApp::cursorAdded);
	_tuioClient.registerCursorUpdated(this, &SecondStudy::TheApp::cursorUpdated);
	_tuioClient.registerCursorRemoved(this, &SecondStudy::TheApp::cursorRemoved);
		
	_tuioClient.connect(); // Defaults to UDP:3333
	
	_sender = make_shared<osc::Sender>();
	_sender->setup("localhost", 3000);
	
	Logger::instance().log("OSC Sender setup to localhost:3000");
		
	setFrameRate(FPS);

	ss = stringstream();
	ss << "Running at " << FPS << " fps";
	Logger::instance().log(ss.str());
	ss.clear();
	
	setWindowSize(640, 480);

	shared_ptr<MeasureWidget> tw1 = make_shared<MeasureWidget>(0.30f * Vec2f(getWindowSize()), 5, 8);
	shared_ptr<MeasureWidget> tw2 = make_shared<MeasureWidget>(0.70f * Vec2f(getWindowSize()), 5, 8);
	_widgets.push_back(tw1);
	_widgets.push_back(tw2);
	
	list<shared_ptr<MeasureWidget>> s1;
	s1.push_back(tw1);
	list<shared_ptr<MeasureWidget>> s2;
	s2.push_back(tw2);
	_sequences.push_back(s1);
	_sequences.push_back(s2);

	_gesturesMutex = make_shared<mutex>();
	_gestures = make_shared<list<shared_ptr<Gesture>>>();

	_staticGRs.push_back(make_shared<TapGestureRecognizer>(_gestures, _gesturesMutex));
	_staticGRs.push_back(make_shared<MusicStrokeGestureRecognizer>(_gestures, _gesturesMutex));
	_staticGRs.push_back(make_shared<ConnectionGestureRecognizer>(_gestures, _gesturesMutex));
	_staticGRs.push_back(make_shared<DisconnectionGestureRecognizer>(_gestures, _gesturesMutex));
	_staticGRs.push_back(make_shared<LongTapGestureRecognizer>(_gestures, _gesturesMutex));
	
	_progressiveGRs.push_back(make_shared<PinchGestureRecognizer>(_gestures, _gesturesMutex));

	// At last, fire up the gesture engine...
	_gestureEngineShouldStop = false;
	_gestureEngine = thread(bind(&SecondStudy::TheApp::gestureEngine, this));
	// ... and the gesture processor.
	_gestureProcessorShouldStop = false;
	_gestureProcessor = thread(bind(&SecondStudy::TheApp::gestureProcessor, this));

	go = false;
	_marker = false;
	
	Logger::instance().log("SecondStudy ready to roll.");
}

void SecondStudy::TheApp::shutdown() {
	_gestureEngineShouldStop = true;
	_gestureProcessorShouldStop = true;
	Logger::instance().stop();
	_loggerThread.join();
	_gestureProcessor.join();
	_gestureEngine.join();
}

void SecondStudy::TheApp::mouseDown( MouseEvent event ) {
	
}

void SecondStudy::TheApp::keyDown(KeyEvent event) {
    switch(event.getChar()) {
        case KeyEvent::KEY_f: {
			setFullScreen(!isFullScreen());
			
			stringstream ss;
			ss << "TheApp::keyDown " << (isFullScreen() ? "FullScreen" : "Window" ) << " (width:" << getWindowSize().x << ", height:" << getWindowSize().y << ")";
			Logger::instance().log(ss.str());
			break;
		}
		case KeyEvent::KEY_m: {
			if(_marker) {
				Logger::instance().log("---- STOP ----");
				_marker = false;
			} else {
				Logger::instance().log("---- START ----");
				_marker = true;
			}
		}
    default: {
			break;
		}
    }
}

void SecondStudy::TheApp::update() {
	_sequencesMutex.lock();
	_sequences.remove_if( [](list<shared_ptr<MeasureWidget>> l) {
		return l.empty();
	});
	_sequencesMutex.unlock();
	
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
	gl::clear(ColorAf(0.0f, 0.0f, 0.0f, 1.0f));
	
	gl::color(0.0f, 0.0f, 0.0f);
	gl::drawSolidRect(getWindowBounds());
	
	gl::color(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(5.0f);
	
	_sequencesMutex.lock();
	for(auto& s : _sequences) {
		if(s.size() > 1) {
			for(auto it = s.begin(); it != prev(s.end()); ++it) {
				shared_ptr<MeasureWidget> a = *it;
				shared_ptr<MeasureWidget> b = *(next(it));

				Matrix44f at;
				at.translate(Vec3f(a->position()));
				at.rotate(Vec3f(0.0f, 0.0f, a->angle()));
				
				Matrix44f bt;
				bt.translate(Vec3f(b->position()));
				bt.rotate(Vec3f(0.0f, 0.0f, b->angle()));
				
				Vec3f ap3 = at.transformPoint(Vec3f(a->outletIcon().getCenter()));
				Vec3f bp3 = bt.transformPoint(Vec3f(b->inletIcon().getCenter()));
				
				Vec2f ap(ap3.x, ap3.y);
				Vec2f bp(bp3.x, bp3.y);

				gl::drawLine(ap, bp);
			}
		}
	}
	_sequencesMutex.unlock();

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
			if(trace->isVisible) {
				gl::color(1.0f, 1.0f, 1.0f, 0.25f);
			} else {
				float c = (trace->lifespan() / 10.0f) * 0.25f;
				gl::color(1.0f, 1.0f, 1.0f, c);
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

void SecondStudy::TheApp::resize() {
	//gl::setViewport(getWindowBounds());
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
		if(_gestures->size() > 0) {
			_gesturesMutex->lock();
			// This may very well be leaking stuff around. 'Tis no good.
			shared_ptr<Gesture> unknownGesture = _gestures->front();
			_gestures->pop_front();
			_gesturesMutex->unlock();

			if(dynamic_pointer_cast<TapGesture>(unknownGesture)) {
				shared_ptr<TapGesture> tap = dynamic_pointer_cast<TapGesture>(unknownGesture);
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
				
				stringstream ss;
				ss << "TheApp::gestureProcessor TapGesture (x:" << tap->position().x << ", y:" << tap->position().y << ", widget_id:" << tap->widgetId() << ")";
				Logger::instance().log(ss.str());
			}

			if(dynamic_pointer_cast<PinchGesture>(unknownGesture)) {
				shared_ptr<PinchGesture> pinch = dynamic_pointer_cast<PinchGesture>(unknownGesture);
				if(pinch->isOnWidget()) {
					_widgetsMutex.lock();
					auto wIt = find_if(_widgets.begin(), _widgets.end(),
						[&, this](shared_ptr<Widget> w) {
							return w->id() == pinch->widgetId();
						});
					if(wIt != _widgets.end()) {
						auto w = *wIt;
						w->moveBy(pinch->distanceDelta());
                        w->zoomBy(pinch->zoomDelta());
                        w->rotateBy(pinch->angleDelta());
					}
					_widgetsMutex.unlock();
					
					stringstream ss;
					ss << "TheApp::gestureProcessor PinchGesture (distance_delta:" << pinch->distanceDelta() << ", zoom_delta:" << pinch->zoomDelta() << ", angle_delta:" << pinch->angleDelta() << ", widget_id:" << pinch->widgetId() << ")";
					Logger::instance().log(ss.str());
				}
			}
			
			if(dynamic_pointer_cast<MusicStrokeGesture>(unknownGesture)) {
				shared_ptr<MusicStrokeGesture> stroke = dynamic_pointer_cast<MusicStrokeGesture>(unknownGesture);
				shared_ptr<MeasureWidget> measure = nullptr;
				_widgetsMutex.lock();
				shared_ptr<Widget> w = *find_if(_widgets.begin(), _widgets.end(),
								   [&, this] (shared_ptr<Widget> w) {
									   return w->id() == stroke->widgetId();
								   });
				if(w != nullptr) {
					measure = dynamic_pointer_cast<MeasureWidget>(w);
				}
				_widgetsMutex.unlock();
				if(measure != nullptr) {
					measure->processStroke(stroke->stroke());
					
					stringstream ss;
					ss << "TheApp::gestureProcessor MusicStrokeGesture (points:[";
					for(auto p : stroke->stroke().touchPoints) {
						ss << "(" << p.getPos().x << "," << p.getPos().y << ")";
					}
					ss << "])";
					Logger::instance().log(ss.str());
				}
			}
			
			if(dynamic_pointer_cast<ConnectionGesture>(unknownGesture)) {
				shared_ptr<ConnectionGesture> connection = dynamic_pointer_cast<ConnectionGesture>(unknownGesture);
				unsigned long fromWid = connection->fromWid();
				unsigned long toWid = connection->toWid();
				_sequencesMutex.lock();

				bool doTheSplice = true;
				list<list<shared_ptr<MeasureWidget>>>::iterator fsIt;
				list<shared_ptr<MeasureWidget>>::iterator fwIt;
				[&,this]() {
					for(auto sit = _sequences.begin(); sit != _sequences.end(); ++sit) {
						for(auto wit = sit->begin(); wit != sit->end(); ++wit) {
							if((*wit)->id() == fromWid) {
								// check if this precedes the destination in the same sequence
								for(auto i(wit); i != sit->begin(); --i) {
									if((*i)->id() == toWid) {
										doTheSplice = false;
										return;
									}
								}
								// cover head case
								if(sit->front()->id() == toWid) {
									doTheSplice = false;
									return;
								}
								fsIt = sit;
								fwIt = wit;
								return;
							}
						}
					}
				}();
				
				if(doTheSplice) {
					list<list<shared_ptr<MeasureWidget>>>::iterator tsIt;
					list<shared_ptr<MeasureWidget>>::iterator twIt;
					[&,this]() {
						for(auto sit = _sequences.begin(); sit != _sequences.end(); ++sit) {
							for(auto wit = sit->begin(); wit != sit->end(); ++wit) {
								if((*wit)->id() == toWid) {
									tsIt = sit;
									twIt = wit;
									return;
								}
							}
						}
					}();
				
					tsIt->splice(twIt, *fsIt, fsIt->begin(), next(fwIt));
				}
				_sequencesMutex.unlock();
				
				if(doTheSplice) {
					stringstream ss;
					ss << "TheApp::gestureProcessor ConnectionGesture (from:" << connection->fromWid() << " to:" << connection->toWid() << ")";
					Logger::instance().log(ss.str());
				}
			}
			
			if(dynamic_pointer_cast<DisconnectionGesture>(unknownGesture)) {
				shared_ptr<DisconnectionGesture> disc = dynamic_pointer_cast<DisconnectionGesture>(unknownGesture);
				unsigned long fromWid = disc->fromWid();
				unsigned long toWid = disc->toWid();
				bool log = false;
				_sequencesMutex.lock();
				[&,this]() {
					for(auto sit = _sequences.begin(); sit != _sequences.end(); ++sit) {
						for(auto wit = sit->begin(); wit != sit->end(); ++wit) {
							if((*wit)->id() == fromWid && (*next(wit))->id() == toWid) {
								list<shared_ptr<MeasureWidget>> newSeq(next(wit), sit->end());
								_sequences.push_back(newSeq);
								sit->erase(next(wit), sit->end());
								log = true;
								return;
							}
						}
					}
				}();
				_sequencesMutex.unlock();
				
				if(log) {
					stringstream ss;
					ss << "TheApp::gestureProcessor DisconnectionGesture (from:" << disc->fromWid() << " to:" << disc->toWid() << ")";
					Logger::instance().log(ss.str());
				}
			}
			
			if(dynamic_pointer_cast<LongTapGesture>(unknownGesture)) {
				shared_ptr<LongTapGesture> longtap = dynamic_pointer_cast<LongTapGesture>(unknownGesture);
				if(!longtap->isOnWidget()) {
					_widgetsMutex.lock();
					Vec2f p = longtap->position();
					auto w = make_shared<MeasureWidget>(p, 5, 8);
					Vec2f c = getWindowCenter();
					float a = atan2(p.y-c.y, p.x-c.x);
					w->angle(a - M_PI_2);
					_widgets.push_back(w);
					_widgetsMutex.unlock();
					
					_sequencesMutex.lock();
					list<shared_ptr<MeasureWidget>> s;
					s.push_back(w);
					_sequences.push_back(s);
					_sequencesMutex.unlock();
				}
				
				stringstream ss;
				ss << "TheApp::gestureProcessor LongTapGesture (x:" << longtap->position().x << " y:" << longtap->position().y << " widget_id:" << longtap->widgetId() << ")";
				Logger::instance().log(ss.str());
			}

			//console() << "unknownGesture.use_count() == " << unknownGesture.use_count() << endl;
		}
	}
}

void SecondStudy::TheApp::measureHasFinishedPlaying(int id) {
	_sequencesMutex.lock();
	for(auto sit = _sequences.begin(); sit != _sequences.end(); ++sit) {
		for(auto wit = sit->begin(); wit != sit->end(); ++wit) {
			if((*wit)->id() == id && next(wit) != sit->end()) {
				(*next(wit))->play();
			} else if((*wit)->id() == id && next(wit) == sit->end()) {
				(*sit->begin())->play();
			}
		}
	}
	_sequencesMutex.unlock();
}
	
void SecondStudy::TheApp::cursorAdded(tuio::Cursor cursor) {
	bool continued = false;
	int joined = -1;
	_tracesMutex.lock();
	for(auto t : _traces) {
		auto trace = t.second;
		if(!trace->isVisible && !trace->isDead() && tuioToWindow(trace->currentPosition()).distance(tuioToWindow(cursor.getPos())) <= 50.0f) {
			_traces[cursor.getSessionId()] = _traces[trace->getSessionId()];
			joined = trace->getSessionId(); // the old session id (therefore the old trace that has been resurrected)
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
	
	stringstream ss;
	ss	<< "TheApp::cursorAdded "
		<< "(x:" << cursor.getPos().x
		<< " y:" << cursor.getPos().y
		<< " session_id:" << cursor.getSessionId()
		<< " widget_id:" << _traces[cursor.getSessionId()]->widgetId
		<< " new_trace:" << (continued ? "n" : "y")
		<< " joined_trace:" << joined
		<< ")";
	Logger::instance().log(ss.str());
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
	
	stringstream ss;
	ss	<< "TheApp::cursorUpdated "
		<< "(x:" << cursor.getPos().x
		<< " y:" << cursor.getPos().y
		<< " session_id:" << cursor.getSessionId()
		<< " widget_id:" << _traces[cursor.getSessionId()]->widgetId
		<< ")";
	Logger::instance().log(ss.str());
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
	
	stringstream ss;
	ss	<< "TheApp::cursorRemoved "
		<< "(x:" << cursor.getPos().x
		<< " y:" << cursor.getPos().y
		<< " session_id:" << cursor.getSessionId()
		<< " widget_id:" << _traces[cursor.getSessionId()]->widgetId
		<< ")";
	Logger::instance().log(ss.str());
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

CINDER_APP_NATIVE( SecondStudy::TheApp, RendererGl(RendererGl::AA_MSAA_4) )
