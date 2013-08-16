#include "TouchPoint.h"

using namespace ci;

SecondStudy::TouchPoint::TouchPoint() {
	timestamp = app::getElapsedSeconds();
}

SecondStudy::TouchPoint::TouchPoint(const tuio::Cursor& c) : tuio::Cursor(c) {
	timestamp = app::getElapsedSeconds();
}