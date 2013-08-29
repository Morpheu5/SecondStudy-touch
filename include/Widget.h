#pragma once

using namespace ci;

namespace SecondStudy {

	class Widget {
		static unsigned long s_id;

	protected:
		float _scale;
		Vec2f _position;
		float _angle;
		unsigned long _id;

	public:
		Widget();

		const unsigned long id() const { return _id; }
		const float scale() const { return _scale; }
		const Vec2f position() const { return _position; }
		const float angle() const { return _angle; }
		
		void position(Vec2f p) { _position = p; }
		void angle(float a) { _angle = a; }

		virtual void draw() = 0;
		virtual bool hit(ci::Vec2f p) = 0;

		virtual void tap(ci::Vec2f p) = 0;
		virtual void moveBy(ci::Vec2f v) = 0;
        virtual void zoomBy(float s) = 0;
        virtual void rotateBy(float a) = 0;
	};
}