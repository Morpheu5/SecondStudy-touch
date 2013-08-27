#include "ConnectionGesture.h"

#include "TouchPoint.h"

SecondStudy::ConnectionGesture::ConnectionGesture() :
_widgetId(0), _toWid(0) { }

SecondStudy::ConnectionGesture::ConnectionGesture(const unsigned long fromWid, const unsigned long toWid) :
_widgetId(fromWid), _toWid(toWid) {
	
}

SecondStudy::ConnectionGesture::~ConnectionGesture() { }

const bool SecondStudy::ConnectionGesture::isOnWidget() {
	return _widgetId != 0;
}

const unsigned long SecondStudy::ConnectionGesture::widgetId() const {
	return _widgetId;
}

const unsigned long SecondStudy::ConnectionGesture::fromWid() const {
	return _widgetId;
}

const unsigned long SecondStudy::ConnectionGesture::toWid() const {
	return _toWid;
}