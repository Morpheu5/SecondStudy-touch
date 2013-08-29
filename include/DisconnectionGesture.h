#pragma once

#include "Gesture.h"
#include "TouchTrace.h"

namespace SecondStudy {
	
	class DisconnectionGesture : public Gesture {
		unsigned long _widgetId;
		unsigned long _toWid;
		
	public:
		DisconnectionGesture();
		DisconnectionGesture(const unsigned long fromWid, const unsigned long toWid);
		
		~DisconnectionGesture();
		
		const bool isOnWidget();
		const unsigned long widgetId() const;
		const unsigned long fromWid() const;
		const unsigned long toWid() const;
	};
}