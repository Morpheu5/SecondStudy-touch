#pragma once

#include <list>
#include "cinder/Vector.h"
#include "TouchPoint.h"

using namespace ci;
using namespace std;

namespace SecondStudy {

	class TouchTrace {
		int _lifespan;

	public:
		enum class State {
			TOUCH_DOWN,
			TOUCH_MOVING,
			TOUCH_STILL,
			TOUCH_UP
		} state;
		
		list<TouchPoint> touchPoints;
		bool isVisible;
		unsigned long widgetId;

		TouchTrace();
		~TouchTrace();

		int getSessionId();
		Vec2f currentPosition();
		Vec2f previousPosition();

		int lifespan();
		bool isDead();
		void update();
		void resurrect();
		bool isOnWidget();

		// TODO State info should be added to the cursors
		void addCursorDown(TouchPoint p);
		void cursorMove(TouchPoint p);
		void addCursorUp(TouchPoint p);
	};

}