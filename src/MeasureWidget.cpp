#include "MeasureWidget.h"

#include "cinder/app/App.h"

using namespace ci::app;

SecondStudy::MeasureWidget::MeasureWidget() : Widget() {
	_scale = 1.0f;
	_position = Vec2f(0.0f, 0.0f);
	_angle = 0.0f;
    _measureSize = pair<int, int>(5, 8);
	_noteBox = Rectf(0.0f, 0.0f, 30.0f, 30.0f);
	_boundingBox = Rectf(0.0f, 0.0f, _noteBox.getWidth() * _measureSize.second, _noteBox.getHeight() * _measureSize.first);
	_boundingBox -= _boundingBox.getSize() / 2.0f;
	
	notes = vector<vector<bool>>(1, vector<bool>(1, false));
}

SecondStudy::MeasureWidget::MeasureWidget(Vec2f center, int rows, int columns) : Widget(),
_position(center),
_measureSize(pair<int, int>(columns, rows)) {
	
    _scale = 1.0f;
	_angle = 0.0f;
	_noteBox = Rectf(0.0f, 0.0f, 30.0f, 30.0f);
	_boundingBox = Rectf(0.0f, 0.0f, _noteBox.getWidth() * columns, _noteBox.getHeight() * rows);
	_boundingBox -= _boundingBox.getSize() / 2.0f;
	
	notes = vector<vector<bool>>(columns, vector<bool>(rows, false));
	console() << notes.size() << " " << notes[0].size() << endl;
}

void SecondStudy::MeasureWidget::draw() {
	gl::pushModelView();
    
	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));
    
	gl::multModelView(transform);
	
	gl::lineWidth(_scale);
	gl::color(1.0f, 1.0f, 1.0f, 0.1f);
	gl::drawSolidRect(_boundingBox);

	int cols = notes.size();
	int rows = notes[0].size();
	Vec2f origin = _boundingBox.getUpperLeft();
	for(int col = 0; col < cols; col++) {
		for(int row = 0; row < rows; row++) {
			//console() << "C: " << col << " R: " << row << endl;
			Rectf box = _noteBox + origin + Vec2f(col, row) * _noteBox.getSize();
			if(notes[col][row]) {
				gl::color(1.0f, 1.0f, 1.0f, 1.0f);
			} else {
				gl::color(1.0f, 1.0f, 1.0f, 0.0f);
			}
			gl::drawSolidRect(box);
			gl::color(1.0f, 1.0f, 1.0f, 1.0f);
			gl::drawStrokedRect(box);
		}
	}
    
	gl::popModelView();
}

bool SecondStudy::MeasureWidget::hit(Vec2f p) {
	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));
    
	Vec3f tp3 = transform.inverted().transformPoint(Vec3f(p));
	Vec2f tp(tp3.x, tp3.y);
	return (_boundingBox * _scale).contains(tp);
}

void SecondStudy::MeasureWidget::tap(Vec2f p) {
	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));
    
	Vec3f tp3 = transform.inverted().transformPoint(Vec3f(p));
	Vec2f tp(tp3.x, tp3.y);
	tp += _boundingBox.getLowerRight();
	tp /= _boundingBox.getSize();
	tp *= Vec2f(notes.size(), notes[0].size());
	Vec2i n = Vec2i(tp.x, tp.y);

	console() << "T : " << n << endl;
	toggle(pair<int, int>(n.x, n.y));
}

void SecondStudy::MeasureWidget::moveBy(Vec2f v) {
	_position += v;
}

void SecondStudy::MeasureWidget::zoomBy(float s) {
    // _scale += s;
}

void SecondStudy::MeasureWidget::rotateBy(float a) {
    _angle += a;
}

void SecondStudy::MeasureWidget::toggle(pair<int, int> note) {
	if(note.first >= 0 && note.first < notes.size() && note.second >= 0 && note.second < notes[0].size()) {
		if(notes[note.first][note.second]) {
			notes[note.first][note.second] = false;
		} else {
			for(auto a : notes[note.first]) {
				a = false;
			}
			notes[note.first][note.second]
			= true;
		}
	}
}

void SecondStudy::MeasureWidget::processTrace(const TouchTrace &trace) {
	console() << "Processing" << endl;
}