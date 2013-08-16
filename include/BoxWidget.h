#pragma once

#include "cinder/Vector.h"
#include "cinder/Rect.h"
#include "cinder/gl/gl.h"


#include "Widget.h"

using namespace ci;
using namespace std;

namespace SecondStudy {

	class BoxWidget : public Widget {
		float _scale;
		Rectf _board;
		Vec2f _position;
		float _angle;

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