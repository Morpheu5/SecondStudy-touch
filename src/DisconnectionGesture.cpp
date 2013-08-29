#include "DisconnectionGesture.h"

#include "TouchPoint.h"

SecondStudy::DisconnectionGesture::DisconnectionGesture() :
_widgetId(0), _toWid(0) { }

SecondStudy::DisconnectionGesture::DisconnectionGesture(const unsigned long fromWid, const unsigned long toWid) :
_widgetId(fromWid), _toWid(toWid) {
	
}

SecondStudy::DisconnectionGesture::~DisconnectionGesture() { }

const bool SecondStudy::DisconnectionGesture::isOnWidget() {
	return _widgetId != 0;
}

const unsigned long SecondStudy::DisconnectionGesture::widgetId() const {
	return _widgetId;
}

const unsigned long SecondStudy::DisconnectionGesture::fromWid() const {
	return _widgetId;
}

const unsigned long SecondStudy::DisconnectionGesture::toWid() const {
	return _toWid;
}