#pragma once

using namespace ci;

namespace SecondStudy {

	class Widget {
		static unsigned long s_id;

	protected:
		unsigned long _id;

	public:
		Widget();

		const unsigned long id() const { return _id; }

		virtual void draw() = 0;
		virtual bool hit(ci::Vec2f p) = 0;

		virtual void tap(ci::Vec2f p) = 0;
		virtual void moveBy(ci::Vec2f v) = 0;
        virtual void zoomBy(float s) = 0;
        virtual void rotateBy(float a) = 0;
	};
}