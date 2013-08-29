#include "LongTapGesture.h"

using namespace ci;

SecondStudy::LongTapGesture::LongTapGesture() : _position(Vec2f(0.0f, 0.0f)), _widgetId(0) { }
SecondStudy::LongTapGesture::LongTapGesture(const Vec2f& p, const unsigned long id) : _position(p), _widgetId(id) { }

SecondStudy::LongTapGesture::~LongTapGesture() { }

const Vec2f& SecondStudy::LongTapGesture::position() const {
	return _position;
}

const bool SecondStudy::LongTapGesture::isOnWidget() {
	return _widgetId != 0;
}

const unsigned long SecondStudy::LongTapGesture::widgetId() const {
	return _widgetId;
}