#pragma once

#include "Gesture.h"

namespace SecondStudy {
	
	class LongTapGesture : public Gesture {
		ci::Vec2f _position;
		unsigned long _widgetId;
		
	public:
		LongTapGesture();
		LongTapGesture(const ci::Vec2f& p, const unsigned long id);
		
		~LongTapGesture();
		
		const ci::Vec2f& position() const;
		const bool isOnWidget();
		const unsigned long widgetId() const;
	};
}