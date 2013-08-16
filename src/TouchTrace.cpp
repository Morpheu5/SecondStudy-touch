#include "TouchTrace.h"

using namespace ci;
using namespace ci::app;

SecondStudy::TouchTrace::TouchTrace() {
	state = State::TOUCH_DOWN;
	isVisible = true;
	widgetId = 0;
	_lifespan = 10;
}

SecondStudy::TouchTrace::~TouchTrace() {
	touchPoints.clear();
}

int SecondStudy::TouchTrace::getSessionId() {
	return touchPoints.back().getSessionId();
}

Vec2f SecondStudy::TouchTrace::currentPosition() {
	return touchPoints.back().getPos();
}

Vec2f SecondStudy::TouchTrace::previousPosition() {
	if(touchPoints.size() > 1) {
		return prev(touchPoints.end(), 2)->getPos();
	} else {
		return currentPosition();
	}
}

int SecondStudy::TouchTrace::lifespan() {
	return _lifespan;
}

bool SecondStudy::TouchTrace::isDead() {
	return _lifespan == 0;
}

void SecondStudy::TouchTrace::update() {
	if(!isVisible) {
		_lifespan--;
	}
}

void SecondStudy::TouchTrace::resurrect() {
	_lifespan = 10;
	isVisible = true;
}

bool SecondStudy::TouchTrace::isOnWidget() {
	return widgetId != 0;
}

// TODO State info should be added to the cursors
void SecondStudy::TouchTrace::addCursorDown(SecondStudy::TouchPoint p) {
	touchPoints.push_back(p);
	state = State::TOUCH_DOWN;
}

void SecondStudy::TouchTrace::cursorMove(SecondStudy::TouchPoint p) {
	//Vec2f q = touchPoints.back().getPos();
	touchPoints.push_back(p);
	if(p.getSpeed().length() < 0.025f) {
		state = State::TOUCH_STILL;
	} else {
		state = State::TOUCH_MOVING;
	}
}

void SecondStudy::TouchTrace::addCursorUp(SecondStudy::TouchPoint p) {
	touchPoints.push_back(p);
	state = State::TOUCH_UP;
}