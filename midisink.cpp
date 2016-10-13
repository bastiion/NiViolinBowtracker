#include "midisink.h"

MidiSink::MidiSink():
{

    QMap<QString, QString> vals = QMidiOut::devices();
    // if(vals.length() > 0) {
    //     m_midi.connect(/* one of the keys (IDs) from `devices()` */);
    // }
}

void MidiSink::bowStart(BowDirection _direction) 
{

}

void MidiSink::bowEnd(BowDirection _direction) 
{
}

void MidiSink::bow(float acceleration, float speed) 
{
}

