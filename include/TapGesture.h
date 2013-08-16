#pragma once

#include "Gesture.h"

#include "cinder/Vector.h"

namespace SecondStudy {
	
	class TapGesture : public Gesture {
		ci::Vec2f _position;
		unsigned long _widgetId;
		
	public:
		TapGesture() : _position(ci::Vec2f(0.0f, 0.0f)), _widgetId(0) { }
		TapGesture(const ci::Vec2f& p, const unsigned long id) : _position(p), _widgetId(id) { }
		
		~TapGesture() { }
		
		const ci::Vec2f& position() const { return _position; }
		const bool isOnWidget() { return _widgetId != 0; }
		const unsigned long widgetId() const { return _widgetId; }
	};
}