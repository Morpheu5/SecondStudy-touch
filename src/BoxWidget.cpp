#include "BoxWidget.h"

using namespace ci;

SecondStudy::BoxWidget::BoxWidget() : SecondStudy::Widget() {
	_scale = 1.0f;
	_board = Rectf(-80.0f, -60.0f, 80.0f, 60.0f);
	_position = Vec2f(0.0f, 0.0f);
	_angle = 0.0f;
}

SecondStudy::BoxWidget::BoxWidget(Vec2f center) : SecondStudy::Widget() {
	_scale = 1.0f;
	_board = Rectf(-80.0f, -60.0f, 80.0f, 60.0f);
	_angle = 0.0f;
	_position = center;
}

void SecondStudy::BoxWidget::draw() {
	gl::pushModelView();

	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));

	gl::multModelView(transform);

	gl::color(1.0f, 1.0f, 1.0f, 0.25f);
	gl::drawSolidRect(_board * _scale);

	gl::color(1.0f, 1.0f, 1.0f, 1.0f);
	gl::lineWidth(_scale);
	gl::drawStrokedRect(_board * _scale);

	gl::popModelView();
}

bool SecondStudy::BoxWidget::hit(Vec2f p) {
	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));

	Vec3f tp3 = transform.inverted().transformPoint(Vec3f(p));
	Vec2f tp(tp3.x, tp3.y);
	return (_board * _scale).contains(tp);
}

void SecondStudy::BoxWidget::tap(Vec2f p) {
	//_scale += 0.1f;
}

void SecondStudy::BoxWidget::moveBy(Vec2f v) {
	_position += v;
}

void SecondStudy::BoxWidget::zoomBy(float s) {
    // _scale += s;
}

void SecondStudy::BoxWidget::rotateBy(float a) {
    _angle += a;
}