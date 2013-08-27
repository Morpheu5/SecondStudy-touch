#pragma once

#include "Gesture.h"

namespace SecondStudy {
	
	class PinchGesture : public Gesture {
		ci::Vec2f _position;
		ci::Vec2f _distanceDelta;
		float _zoomDelta;
        float _angleDelta;
		unsigned long _widgetId;
		
	public:
		PinchGesture();

		PinchGesture(const ci::Vec2f& p, const ci::Vec2f& dd, const float zd, const float ad, const unsigned long id);
		
		~PinchGesture();
		
		const ci::Vec2f& position() const;
		const ci::Vec2f& distanceDelta() const;
		const float zoomDelta() const;
        const float angleDelta() const;

		const bool isOnWidget();
		const unsigned long widgetId() const;
	};
}