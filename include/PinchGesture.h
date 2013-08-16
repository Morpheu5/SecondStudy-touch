#pragma once

#include "Gesture.h"

#include "cinder/Vector.h"

namespace SecondStudy {
	
	class PinchGesture : public Gesture {
		ci::Vec2f _position;
		ci::Vec2f _distanceDelta;
		float _zoomDelta;
        float _angleDelta;
		unsigned long _widgetId;
		
	public:
		PinchGesture() :
			_position(ci::Vec2f(0.0f, 0.0f)),
			_distanceDelta(ci::Vec2f(0.0f, 0.0f)),
			_zoomDelta(0.0f),
            _angleDelta(0.0f),
			_widgetId(0) { }

		PinchGesture(const ci::Vec2f& p, const ci::Vec2f& dd, const float zd, const float ad, const unsigned long id) :
			_position(p),
			_distanceDelta(dd),
			_zoomDelta(zd),
            _angleDelta(ad),
			_widgetId(id) { }
		
		~PinchGesture() { }
		
		const ci::Vec2f& position() const { return _position; }
		const ci::Vec2f& distanceDelta() const { return _distanceDelta; }
		const float zoomDelta() const { return _zoomDelta; }
        const float angleDelta() const { return _angleDelta; }

		const bool isOnWidget() { return _widgetId != 0; }
		const unsigned long widgetId() const { return _widgetId; }
	};
}