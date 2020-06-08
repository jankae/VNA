#include "trace.h"

Trace::Trace(QString name)
    : _name(name),
      _color(Qt::darkYellow),
      liveType(LivedataType::Overwrite),
      visible(true),
      paused(false),
      touchstone(false)
{

}

Trace::~Trace()
{
    emit deleted();
}

void Trace::clear() {
    _data.clear();
    emit cleared();
}

void Trace::addData(double frequency, Trace::Data d) {
    _data[frequency] = d;
    emit dataAdded(frequency, d);
}

void Trace::setName(QString name) {
    _name = name;
}

void Trace::fillFromTouchstone(Touchstone &t)
{
    clear();
    for(unsigned int i=0;i<t.points();i++) {
        auto tData = t.point(i);
        Data d;
        d.S11 = tData.S[0];
        if(t.ports() > 1) {
            d.S12 = tData.S[1];
            d.S21 = tData.S[2];
            d.S22 = tData.S[3];
        }
        addData(tData.frequency, d);
    }
    touchstone = true;
}

void Trace::fromLivedata(Trace::LivedataType type)
{
    touchstone = false;
    liveType = type;
}

void Trace::setColor(QColor color) {
    _color = color;
}

void Trace::setVisible(bool visible)
{
    if(visible != this->visible) {
        this->visible = visible;
        emit visibilityChanged();
    }
}

bool Trace::isVisible()
{
    return visible;
}

bool Trace::pause()
{
    paused = true;
}

bool Trace::resume()
{
    paused = false;
}

bool Trace::isPaused()
{
    return paused;
}

bool Trace::isTouchstone()
{
    return touchstone;
}
