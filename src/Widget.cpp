#include "Widget.h"

unsigned long SecondStudy::Widget::s_id = 0;

SecondStudy::Widget::Widget() {
	// So that there can never be a widget with id 0
	_id = ++s_id;
}