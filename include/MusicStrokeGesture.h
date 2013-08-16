#pragma once

#include "Gesture.h"
#include "TouchTrace.h"

namespace SecondStudy {
	
	class MusicStrokeGesture : public Gesture {
		TouchTrace _stroke;
		unsigned long _widgetId;
		
	public:
		MusicStrokeGesture();
		MusicStrokeGesture(const TouchTrace& t, const unsigned long id);
		
		~MusicStrokeGesture();
		
		const TouchTrace& stroke() const;
		const bool isOnWidget();
		const unsigned long widgetId() const;
	};
}