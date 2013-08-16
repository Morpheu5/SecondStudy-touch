#include "PinchGesture.h"

SecondStudy::PinchGesture::PinchGesture() :
_position(ci::Vec2f(0.0f, 0.0f)),
_distanceDelta(ci::Vec2f(0.0f, 0.0f)),
_zoomDelta(0.0f),
_angleDelta(0.0f),
_widgetId(0) { }

SecondStudy::PinchGesture::PinchGesture(const ci::Vec2f& p, const ci::Vec2f& dd, const float zd, const float ad, const unsigned long id) :
_position(p),
_distanceDelta(dd),
_zoomDelta(zd),
_angleDelta(ad),
_widgetId(id) { }

SecondStudy::PinchGesture::~PinchGesture() { }

const ci::Vec2f& SecondStudy::PinchGesture::position() const {
	return _position;
}

const ci::Vec2f& SecondStudy::PinchGesture::distanceDelta() const {
	return _distanceDelta;
}

const float SecondStudy::PinchGesture::zoomDelta() const {
	return _zoomDelta;
}

const float SecondStudy::PinchGesture::angleDelta() const {
	return _angleDelta;
}

const bool SecondStudy::PinchGesture::isOnWidget() {
	return _widgetId != 0;
}

const unsigned long SecondStudy::PinchGesture::widgetId() const {
	return _widgetId;
}