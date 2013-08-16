#pragma once

#include "cinder/app/App.h"
#include "TuioCursor.h"

using namespace ci;

namespace SecondStudy {

	class TouchPoint : public tuio::Cursor {
	public:
		double timestamp;
	
		TouchPoint();
		TouchPoint(const tuio::Cursor& c);
	};

}