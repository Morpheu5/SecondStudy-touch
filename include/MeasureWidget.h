#pragma once

#include "Widget.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace std;

#define MEASUREWIDGET_NOTELENGTH 0.25f

namespace SecondStudy {
	
	class TouchTrace;
    
	class MeasureWidget : public Widget {
		float _scale;
		Vec2f _position;
		float _angle;
		Rectf _noteBox;
		Rectf _boundingBox;
		
		Rectf _playIcon;
		Rectf _inletIcon;
		Rectf _outletIcon;
		
		Rectf _cursor;
		Anim<Vec2f> _cursorOffset;
		
		CueRef _cue;

        pair<int, int> _measureSize;
		
		vector<int> _midiNotes;
		
		void playNote(int n);

	public:
		vector<vector<bool>> notes;
		mutex notesMutex;
		
		MeasureWidget();
		MeasureWidget(Vec2f center, int rows, int columns);
        
		void draw();
		bool hit(Vec2f p);
		void tap(Vec2f p);
		void moveBy(Vec2f v);
        void zoomBy(float s);
        void rotateBy(float a);
		
		bool hitInlet(Vec2f p);
		bool hitOutlet(Vec2f p);
		
		const pair<int, int>& measureSize() const { return _measureSize; }
		
		void toggle(pair<int, int> note);
		void processStroke(const TouchTrace &trace);
	};
}