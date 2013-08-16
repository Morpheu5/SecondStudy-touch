#include "MusicStrokeGesture.h"

#include "TouchPoint.h"

SecondStudy::MusicStrokeGesture::MusicStrokeGesture() :
_widgetId(0) { }

SecondStudy::MusicStrokeGesture::MusicStrokeGesture(const TouchTrace& t, const unsigned long id) :
_stroke(t),
_widgetId(id) { }

SecondStudy::MusicStrokeGesture::~MusicStrokeGesture() { }

const SecondStudy::TouchTrace& SecondStudy::MusicStrokeGesture::stroke() const {
	return _stroke;
}

const bool SecondStudy::MusicStrokeGesture::isOnWidget() {
	return _widgetId != 0;
}

const unsigned long SecondStudy::MusicStrokeGesture::widgetId() const {
	return _widgetId;
}