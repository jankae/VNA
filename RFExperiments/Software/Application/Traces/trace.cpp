#include "trace.h"

using namespace std;

Trace::Trace(QString name, QColor color)
    : _name(name),
      _color(color),
      _liveType(LivedataType::Overwrite),
      visible(true),
      paused(false),
      touchstone(false)
{

}

Trace::~Trace()
{
    emit deleted(this);
}

void Trace::clear() {
    if(paused) {
        return;
    }
    _data.clear();
    emit cleared(this);
    emit dataChanged();
}

void Trace::addData(Trace::Data d) {
    if(paused) {
        return;
    }
    // add or replace data in vector while keeping it sorted with increasing frequency
    auto lower = lower_bound(_data.begin(), _data.end(), d, [](const Data &lhs, const Data &rhs) -> bool {
        return lhs.frequency < rhs.frequency;
    });
    if(lower == _data.end()) {
        // highest frequency yet, add to vector
        _data.push_back(d);
    } else if(lower->frequency == d.frequency) {
        switch(_liveType) {
        case LivedataType::Overwrite:
            // replace this data element
            *lower = d;
            break;
        case LivedataType::MaxHold:
            // replace this data element
            if(abs(d.S) > abs(lower->S)) {
                *lower = d;
            }
            break;
        case LivedataType::MinHold:
            // replace this data element
            if(abs(d.S) < abs(lower->S)) {
                *lower = d;
            }
            break;
        }

    } else {
        // insert at this position
        _data.insert(lower, d);
    }
    emit dataAdded(this, d);
    emit dataChanged();
}

void Trace::setName(QString name) {
    _name = name;
    emit nameChanged();
}

void Trace::fillFromTouchstone(Touchstone &t, int parameter, QString filename)
{
    if(parameter >= t.ports()*t.ports()) {
        throw runtime_error("Parameter for touchstone out of range");
    }
    clear();
    setTouchstoneParameter(parameter);
    setTouchstoneFilename(filename);
    for(unsigned int i=0;i<t.points();i++) {
        auto tData = t.point(i);
        Data d;
        d.frequency = tData.frequency;
        d.S = t.point(i).S[parameter];
        addData(d);
    }
    // check if parameter is square (e.i. S11/S22/S33/...)
    parameter++;
    bool isSquare = false;
    for (int i = 1; i * i <= parameter; i++) {

        // If (i * i = n)
        if ((parameter % i == 0) && (parameter / i == i)) {
            isSquare = true;
            break;
        }
    }
    if(isSquare == 1) {
        reflection = true;
    } else {
        reflection = false;
    }
    touchstone = true;
}

void Trace::fromLivedata(Trace::LivedataType type, LiveParameter param)
{
    touchstone = false;
    _liveType = type;
    _liveParam = param;
    if(param == LiveParameter::S11 || param == LiveParameter::S22) {
        reflection = true;
    } else {
        reflection = false;
    }
}

void Trace::setColor(QColor color) {
    if(_color != color) {
        _color = color;
        emit colorChanged(this);
    }
}

void Trace::setVisible(bool visible)
{
    if(visible != this->visible) {
        this->visible = visible;
        emit visibilityChanged(this);
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

bool Trace::isReflection()
{
    return reflection;
}

QString Trace::getTouchstoneFilename() const
{
    return touchstoneFilename;
}

void Trace::setTouchstoneFilename(const QString &value)
{
    touchstoneFilename = value;
}

int Trace::getTouchstoneParameter() const
{
    return touchstoneParameter;
}

void Trace::setTouchstoneParameter(int value)
{
    touchstoneParameter = value;
}