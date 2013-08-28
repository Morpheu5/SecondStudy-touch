#pragma once

#include "Widget.h"

using namespace ci;

namespace SecondStudy {

	class BoxWidget : public Widget {
		Rectf _board;

	public:
		BoxWidget();
		BoxWidget(Vec2f center);

		void draw();
		bool hit(Vec2f p);
		void tap(Vec2f p);
		void moveBy(Vec2f v);
        void zoomBy(float s);
        void rotateBy(float a);
	};
}