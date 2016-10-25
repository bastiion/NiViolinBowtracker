#include "midisink.h"
#include <QtCore/QDebug>

MidiSink::MidiSink():
    connected(false),
    key(60),
    velocity(100),
    isNoteOn(false)
{

     //QMap<QString, QString> vals = QMidiOut::devices();
    // if(vals.length() > 0) {
    //     m_midi.connect(/* one of the keys (IDs) from `devices()` */);
    // }
}
MidiSink::~MidiSink()
{
    if(connected)
        m_midi.disconnect();
}

void MidiSink::bowStart(BowDirection _direction) 
{
    qDebug() << "noteOn";
    noteOn(key);

}
void MidiSink::noteOn(int _key)
{
    if(!connected) return;
    isNoteOn = true;
    m_midi.noteOn(_key, 0);
}

void MidiSink::bowEnd() 
{
    qDebug() << "noteOff";
    noteOff(key);
}
void MidiSink::noteOff(int _key)
{
    isNoteOn = false;
    if(!connected) return;
    m_midi.noteOff(_key, 0);
}

void MidiSink::changeKey(int _key)
{
    if(_key == key)
        return;
    if(isNoteOn) {
        noteOff(key);
        noteOn(_key);
    }
    key = _key;
}

void MidiSink::controlChange(int _num, int _value) 
{
    if(!connected) return;
    m_midi.controlChange(0, _num, _value);
}

const QMap<QString,QString> MidiSink::devices()
{
    return QMidiOut::devices();
}
bool MidiSink::connect(const QString& device)
{
    connected = m_midi.connect(device);
}

