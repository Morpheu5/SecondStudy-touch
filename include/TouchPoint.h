#pragma once

#include "cinder/app/App.h"
#include "TuioCursor.h"

namespace SecondStudy {

	class TouchPoint : public ci::tuio::Cursor {
	public:
		double timestamp;
	
		TouchPoint();
		TouchPoint(const ci::tuio::Cursor& c);
	};
}