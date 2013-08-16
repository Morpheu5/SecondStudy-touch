#pragma once

#include "Widget.h"

using namespace ci;
using namespace std;

namespace SecondStudy {
	
	class TouchTrace;
    
	class MeasureWidget : public Widget {
		float _scale;
		Vec2f _position;
		float _angle;
		Rectf _noteBox;
		Rectf _boundingBox;

        pair<int, int> _measureSize;

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
		
		const pair<int, int>& measureSize() const { return _measureSize; }
		
		void toggle(pair<int, int> note);
		void processStroke(const TouchTrace &trace);
	};
}