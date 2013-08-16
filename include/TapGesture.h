#pragma once

#include "Gesture.h"

#include "cinder/Vector.h"

namespace SecondStudy {
	
	class TapGesture : public Gesture {
		ci::Vec2f _position;
		unsigned long _widgetId;
		
	public:
		TapGesture();
		TapGesture(const ci::Vec2f& p, const unsigned long id);
		
		~TapGesture();
		
		const ci::Vec2f& position() const;
		const bool isOnWidget();
		const unsigned long widgetId() const;
	};
}