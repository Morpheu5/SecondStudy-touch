#pragma once

#include "TuioCursor.h"

using namespace ci;
using namespace ci::tuio;

namespace SecondStudy {

	class TouchPoint : public Cursor {
	public:
		double timestamp;
	
		TouchPoint();
		TouchPoint(const Cursor& c);
	};
}