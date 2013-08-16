#pragma once

#include "cinder/Vector.h"

using namespace ci;
using namespace std;

namespace SecondStudy {

	class Widget {
		static unsigned long s_id;

	protected:
		unsigned long _id;

	public:
		Widget();

		const unsigned long id() const { return _id; }

		virtual void draw() = 0;
		virtual bool hit(Vec2f p) = 0;

		virtual void tap(Vec2f p) = 0;
		virtual void moveBy(Vec2f v) = 0;
        virtual void zoomBy(float s) = 0;
        virtual void rotateBy(float a) = 0;
	};
}