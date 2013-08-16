#include "TapGesture.h"

SecondStudy::TapGesture::TapGesture() : _position(ci::Vec2f(0.0f, 0.0f)), _widgetId(0) { }
SecondStudy::TapGesture::TapGesture(const ci::Vec2f& p, const unsigned long id) : _position(p), _widgetId(id) { }

SecondStudy::TapGesture::~TapGesture() { }

const ci::Vec2f& SecondStudy::TapGesture::position() const {
	return _position;
}

const bool SecondStudy::TapGesture::isOnWidget() {
	return _widgetId != 0;
}

const unsigned long SecondStudy::TapGesture::widgetId() const {
	return _widgetId;
}