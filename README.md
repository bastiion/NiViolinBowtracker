# NiViolinBowtracker


A violin bow tracker, that uses the OpenNi-framework for Kinect and other depth sensors to track a violin bow and
send midi events.

## Installation

You need to install all OpenNi and NITE Dependencies.
Please follow this guide for a Debian based operating system:

http://www.20papercups.net/programming/kinect-on-ubuntu-with-openni/

Additionaly Core and Gui Qt-4 Development files need to be installed

    git submodule init
    git submodule update
    mkdir build
    cd build
    cmake ..
    make -j${CORES}


## Usage

1. Start bowtracker binary
2. Press Start Kinect
3. Press Start Caputering
4. Perform Wave Gesture with bow hand
5. Start playing ;-)

You can select a Midi Interface from the list of Midi Devices in order
to send Midi-Events

## Notes

Keep in mind that this is a quick prototype. Crashes may occur at any time. Under difficult lightening
conditions tracking results could be poor.
Additional comments will follow soon.
