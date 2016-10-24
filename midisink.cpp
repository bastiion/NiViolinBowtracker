#include "midisink.h"
#include <QtCore/QDebug>

MidiSink::MidiSink():
    connected(false),
    key(60)
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
    if(!connected) return;
    m_midi.noteOn(key, 0);

}

void MidiSink::bowEnd() 
{
    qDebug() << "noteOff";
    if(!connected) return;
    m_midi.noteOff(key, 0);
}
void MidiSink::changeKey(int _key)
{
    //key = _key;
}

void MidiSink::bow(float acceleration, float speed) 
{
}

const QMap<QString,QString> MidiSink::devices()
{
    return QMidiOut::devices();
}
bool MidiSink::connect(const QString& device)
{
    connected = m_midi.connect(device);
}

