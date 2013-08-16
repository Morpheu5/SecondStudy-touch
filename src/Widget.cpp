#include "Widget.h"

unsigned long SecondStudy::Widget::s_id = 0;

SecondStudy::Widget::Widget() {
	_id = ++s_id;
}