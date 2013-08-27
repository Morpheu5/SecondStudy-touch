#include "MeasureWidget.h"

#include "TouchPoint.h"
#include "TouchTrace.h"

#include "SecondStudyApp.h"

#include <set>

using namespace ci;
using namespace ci::app;
using namespace std;

SecondStudy::MeasureWidget::MeasureWidget() : Widget() {
	_scale = 1.0f;
	_position = Vec2f(0.0f, 0.0f);
	_angle = 0.0f;
    _measureSize = pair<int, int>(5, 8);
	_noteBox = Rectf(0.0f, 0.0f, 30.0f, 30.0f);
	_boundingBox = Rectf(0.0f, 0.0f, _noteBox.getWidth() * _measureSize.second, _noteBox.getHeight() * _measureSize.first);
	_boundingBox -= _boundingBox.getSize() / 2.0f;
	
	_playIcon = Rectf(0.0f, 0.0f, _noteBox.getWidth()-10, _noteBox.getHeight()-10);
	_playIcon += _boundingBox.getUpperLeft() - Vec2f(0.0f, _noteBox.getHeight());
	
	_inletIcon = Rectf(0.0f, 0.0f, _noteBox.getWidth(), _noteBox.getHeight());
	_inletIcon += Vec2f(-_inletIcon.getWidth()-10.0f, _boundingBox.getCenter().y);
	
	_outletIcon = Rectf(0.0f, 0.0f, _noteBox.getWidth(), _noteBox.getHeight());
	_outletIcon += Vec2f(-_boundingBox.getWidth()+10.0f, _boundingBox.getCenter().y);
	
	_cursorOffset = Vec2f(0.0f, 0.0f);
	_cursor = Rectf(Vec2f(0.0f, 0.0f), Vec2f(_noteBox.getWidth(), 10.0f));
	_cursor += _boundingBox.getLowerLeft();
	
	// C major pentatonic
	_midiNotes.push_back(69);
	_midiNotes.push_back(67);
	_midiNotes.push_back(64);
	_midiNotes.push_back(62);
	_midiNotes.push_back(60);

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
	
	_playIcon = Rectf(0.0f, 0.0f, _noteBox.getWidth(), _noteBox.getHeight());
	_playIcon += _boundingBox.getUpperLeft() - Vec2f(0.0f, _noteBox.getHeight() + 10.0f);
	
	_inletIcon = Rectf(0.0f, 0.0f, _noteBox.getWidth(), _noteBox.getHeight());
	_inletIcon += Vec2f(_boundingBox.getUpperLeft().x - _inletIcon.getWidth() - 10.0f, -_inletIcon.getWidth()/2.0f);

	_outletIcon = Rectf(0.0f, 0.0f, _noteBox.getWidth(), _noteBox.getHeight());
	_outletIcon += Vec2f(_boundingBox.getUpperRight().x + 10.0f, -_outletIcon.getWidth()/2.0f);
	
	_cursorOffset = Vec2f(0.0f, 0.0f);
	_cursor = Rectf(Vec2f(0.0f, 0.0f), Vec2f(_noteBox.getWidth(), 10.0f));
	_cursor += _boundingBox.getLowerLeft();

	// C major pentatonic
	_midiNotes.push_back(69);
	_midiNotes.push_back(67);
	_midiNotes.push_back(64);
	_midiNotes.push_back(62);
	_midiNotes.push_back(60);
	
	notes = vector<vector<bool>>(columns, vector<bool>(rows, false));
}

void SecondStudy::MeasureWidget::draw() {
	gl::pushModelView();
    
	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));
    
	gl::multModelView(transform);
	
	gl::lineWidth(_scale);
	gl::color(1.0f, 1.0f, 1.0f, 0.25f);
	gl::drawSolidRect(_boundingBox);

	int cols = notes.size();
	int rows = notes[0].size();
	Vec2f origin = _boundingBox.getUpperLeft();
	for(int col = 0; col < cols; col++) {
		for(int row = 0; row < rows; row++) {
			//console() << "C: " << col << " R: " << row << endl;
			Rectf box = _noteBox + origin + Vec2f(col, row) * _noteBox.getSize();
			if(notes[col][row]) {
				gl::color(1.0f, 1.0f, 1.0f, 0.5f);
			} else {
				gl::color(1.0f, 1.0f, 1.0f, 0.0f);
			}
			gl::drawSolidRect(box);
			gl::color(1.0f, 1.0f, 1.0f, 1.0f);
			gl::drawStrokedRect(box);
			
			gl::drawStrokedRect(_playIcon);
			gl::drawStrokedRect(_inletIcon);
			gl::drawStrokedRect(_outletIcon);
		}
	}
	
	gl::color(1.0f, 1.0f, 1.0f, 0.75f);
	gl::drawSolidRect(_cursor + _cursorOffset);
	gl::drawStrokedRect(_cursor + _cursorOffset);
    
	gl::popModelView();
	gl::color(1.0f, 1.0f, 1.0f, 1.0f);
}

bool SecondStudy::MeasureWidget::hit(Vec2f p) {
	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));
    
	Vec3f tp3 = transform.inverted().transformPoint(Vec3f(p));
	Vec2f tp(tp3.x, tp3.y);
	return (_boundingBox * _scale).contains(tp)
			|| (_playIcon * _scale).contains(tp)
			|| (_inletIcon * _scale).contains(tp)
			|| (_outletIcon * _scale).contains(tp);
}

bool SecondStudy::MeasureWidget::hitInlet(Vec2f p) {
	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));
    
	Vec3f tp3 = transform.inverted().transformPoint(Vec3f(p));
	Vec2f tp(tp3.x, tp3.y);
	return (_inletIcon * _scale).contains(tp);
}

bool SecondStudy::MeasureWidget::hitOutlet(Vec2f p) {
	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));
    
	Vec3f tp3 = transform.inverted().transformPoint(Vec3f(p));
	Vec2f tp(tp3.x, tp3.y);
	return (_outletIcon * _scale).contains(tp);
}

void SecondStudy::MeasureWidget::tap(Vec2f p) {
	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));
    
	Vec3f tp3 = transform.inverted().transformPoint(Vec3f(p));
	Vec2f tp(tp3.x, tp3.y);
	
	if((_playIcon * _scale).contains(tp)) {
		app::timeline().apply(&_cursorOffset, Vec2f(0.0f, 0.0f), 0);
		app::timeline().appendTo(&_cursorOffset, Vec2f(_boundingBox.getWidth() * (1.0f - 1.0f/notes.size()), 0.0f), MEASUREWIDGET_NOTELENGTH*notes.size());
		app::timeline().appendTo(&_cursorOffset, Vec2f(0.0f, 0.0f), MEASUREWIDGET_NOTELENGTH, EaseInOutSine());
		
		/*
		for(int i = 0; i < _size.first; i++) {
			_cue = app::timeline().add( bind(&Tangible::_play, this, i), app::timeline().getCurrentTime() + noteLength*i);
		}
		_cue->setAutoRemove(true);
		_cue->setLoop(false);
		 */
	}
}

void SecondStudy::MeasureWidget::playNote(int n) {
	for(int i = 0; i < notes[n].size(); i++) {
		if(notes[n][i]) {
			TheApp *theApp = (TheApp *)App::get();
			osc::Message m;
			m.setAddress("/playnote");
			m.addIntArg(_midiNotes[i]);
			theApp->sender()->sendMessage(m);
		}
	}
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

void SecondStudy::MeasureWidget::processStroke(const TouchTrace &trace) {
	Matrix44f transform;
	transform.translate(Vec3f(_position));
	transform.rotate(Vec3f(0.0f, 0.0f, _angle));
    
	TheApp *theApp = (TheApp *)App::get();
	
	// TODO Exclude strokes that start and finish outside of the actual box
	
	set<pair<int, int>> noteSet;
	for(auto& q : trace.touchPoints) {
		Vec2f p(theApp->tuioToWindow(q.getPos()));
		Vec3f tp3 = transform.inverted().transformPoint(Vec3f(p));
		Vec2f tp(tp3.x, tp3.y);
		if((_boundingBox * _scale).contains(tp)) {
			tp += _boundingBox.getLowerRight();
			tp /= _boundingBox.getSize();
			tp *= Vec2f(notes.size(), notes[0].size());
			Vec2i n = Vec2i(tp.x, tp.y);
			noteSet.insert(pair<int, int>(n.x, n.y));
		}
	}
	
	for(auto n : noteSet) {
		toggle(n);
	}
}