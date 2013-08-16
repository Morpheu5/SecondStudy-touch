#pragma once

#include "Gesture.h"
#include "TouchTrace.h"

#include "cinder/Vector.h"

namespace SecondStudy {
	
	class MusicStrokeGesture : public Gesture {
		TouchTrace _stroke;
		unsigned long _widgetId;
		
	public:
		MusicStrokeGesture() : _widgetId(0) { }
		MusicStrokeGesture(const TouchTrace& t, const unsigned long id) : _stroke(t), _widgetId(id) { }
		
		~MusicStrokeGesture() { }
		
		const TouchTrace& stroke() const { return _stroke; }
		const bool isOnWidget() { return _widgetId != 0; }
		const unsigned long widgetId() const { return _widgetId; }
	};
}