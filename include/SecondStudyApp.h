#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/Timeline.h"

#include "OscSender.h"
#include "OscListener.h"

#include "TuioClient.h"
#include "TuioCursor.h"

#include "TouchTrace.h"

#include "TapGestureRecognizer.h"
#include "TapGesture.h"

#include "PinchGestureRecognizer.h"
#include "PinchGesture.h"

#include "Widget.h"
#include "BoxWidget.h"
#include "MeasureWidget.h"

#define FPS 60

using namespace std;
using namespace ci;
using namespace ci::app;

namespace SecondStudy {

	class TheApp : public AppNative {
		
		vector<shared_ptr<ProgressiveGestureRecognizer>> _progressiveGRs;
		vector<shared_ptr<StaticGestureRecognizer>> _staticGRs;
		
		shared_ptr<list<shared_ptr<Gesture>>> _gestures;
		shared_ptr<mutex> _gesturesMutex;
		
		tuio::Client _tuioClient;
		
		string _hostname;
		int _port;
		shared_ptr<osc::Sender> _sender;
		
		map<int, shared_ptr<TouchTrace>> _traces;
		mutex _tracesMutex;
		
		vector<list<shared_ptr<TouchTrace>>> _groups;
		mutex _groupsMutex;
		
		list<list<shared_ptr<TouchTrace>>> _removedGroups;
		mutex _removedGroupsMutex;
		
		shared_ptr<list<shared_ptr<Widget>>> _widgets;
		shared_ptr<mutex> _widgetsMutex;
		
		thread _gestureEngine;
		bool _gestureEngineShouldStop;

		thread _gestureProcessor;
		bool _gestureProcessorShouldStop;

		bool go;
		
	public:
		void setup();
		void shutdown();
		void mouseDown( MouseEvent event );
        void keyDown(KeyEvent event);
		void update();
		void draw();
		
		void gestureEngine();
		void gestureProcessor();
		
		void cursorAdded(tuio::Cursor cursor);
		void cursorUpdated(tuio::Cursor cursor);
		void cursorRemoved(tuio::Cursor cursor);
		
		int findGroupForTrace(shared_ptr<TouchTrace> trace);
		
		Vec2f tuioToWindow(Vec2f p) { return p * Vec2f(getWindowSize()); }
        
        int numberOfTraces() { return _traces.size(); }
		
	};

}