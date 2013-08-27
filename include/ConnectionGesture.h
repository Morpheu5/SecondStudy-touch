#pragma once

#include "Gesture.h"
#include "TouchTrace.h"

namespace SecondStudy {
	
	class ConnectionGesture : public Gesture {
		unsigned long _widgetId;
		unsigned long _toWid;
		
	public:
		ConnectionGesture();
		ConnectionGesture(const unsigned long fromWid, const unsigned long toWid);
		
		~ConnectionGesture();
		
		const bool isOnWidget();
		const unsigned long widgetId() const;
		const unsigned long fromWid() const;
		const unsigned long toWid() const;
	};
}