/*
 * mainwindow.cpp - Mainwindow to embed the OpenGL Window...
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>

#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>

#include <QtCore/QDebug>
#include "mainwindow.h"

MainWindow::MainWindow()
{
    glw = new QtGLWidget(this);
    m_midiSink = new MidiSink();

    QPushButton * initKinectBtn = new QPushButton(tr("start kinect"));
    QPushButton * startCaptureKinectBtn = new QPushButton(tr("start Capturing"));
    QPushButton * openCVTest = new QPushButton(tr("test OpenCV"));


    QLabel * houghThresholdLabel = new QLabel(tr("Threshold"));
    QLabel * maxLineGapLabel = new QLabel(tr("Max line gap"));
    QLabel * minLineLengthLabel = new QLabel(tr("Min line length"));

    QSpinBox * houghThresholdSpin = new QSpinBox;
    houghThresholdSpin->setRange(0,500);
    houghThresholdSpin->setValue(100);
    QDoubleSpinBox * maxLineGapSpin = new QDoubleSpinBox;
    maxLineGapSpin->setRange(0.0, 100.0);
    maxLineGapSpin->setValue(20.0);
    QDoubleSpinBox * minLineLengthSpin = new QDoubleSpinBox;
    minLineLengthSpin->setRange(0.0, 100.0);
    minLineLengthSpin->setValue(80.0);
    QLabel * toggleDebugCannyLbl = new QLabel(tr("enable Canny Edge Window"));
    QCheckBox * toggleDebugCannyChk = new QCheckBox();
    QLabel * toggleDebugHoughLbl = new QLabel(tr("enable Hough Edge Window"));
    QCheckBox * toggleDebugHoughChk = new QCheckBox();
    QLabel * toggleMeanAverageLbl = new QLabel(tr("use mean instead of average \n velocity calculation"));
    QCheckBox * toggleMeanAverageChk = new QCheckBox();
    toggleMeanAverageChk->setCheckState(Qt::Checked);

    QGridLayout * houghGroupLayout = new QGridLayout(this);
    QGroupBox * houghSettingGroup = new QGroupBox(tr("Hough Lines Settings"));
    houghSettingGroup->setLayout(houghGroupLayout);
    houghGroupLayout->addWidget(houghThresholdLabel, 0, 0);
    houghGroupLayout->addWidget(houghThresholdSpin, 0, 1);
    houghGroupLayout->addWidget(maxLineGapLabel, 1, 0);
    houghGroupLayout->addWidget(maxLineGapSpin, 1, 1);
    houghGroupLayout->addWidget(minLineLengthLabel, 2, 0);
    houghGroupLayout->addWidget(minLineLengthSpin, 2, 1);
    houghGroupLayout->addWidget(toggleDebugCannyLbl, 3, 0);
    houghGroupLayout->addWidget(toggleDebugCannyChk, 3, 1);
    houghGroupLayout->addWidget(toggleDebugHoughLbl, 4, 0);
    houghGroupLayout->addWidget(toggleDebugHoughChk, 4, 1);
    houghGroupLayout->addWidget(toggleMeanAverageLbl, 5, 0);
    houghGroupLayout->addWidget(toggleMeanAverageChk, 5, 1);
    
    QLabel * depthFilterLabel = new QLabel(tr("Filter hand depth"));
    QCheckBox * depthFilterChk = new QCheckBox();
    depthFilterChk->setCheckState(Qt::Checked);
    
    QGridLayout * kinectGroupLayout = new QGridLayout(this);
    QGroupBox * kinectSettingGroup = new QGroupBox(tr("Kinect Settings"));
    kinectSettingGroup->setLayout(kinectGroupLayout);
    kinectGroupLayout->addWidget(depthFilterLabel, 0 , 0);
    kinectGroupLayout->addWidget(depthFilterChk, 0 , 1);

    QPushButton * midiConnectionRefreshBtn = new QPushButton(tr("refresh"));
    midiConnectionListW = new QListWidget(this);
    refreshMidiConnections();
    
    QLabel * controlerLbl = new QLabel(tr("Bow speed"));
    speedControlerNumSpin = new QSpinBox;
    speedControlerNumSpin->setRange(0,127);
    speedControlerNumSpin->setValue(7);
    speedControlerValueSpin = new QSpinBox;
    speedControlerValueSpin->setRange(0,255);
    speedControlerValueSpin->setValue(0);
    QPushButton * speedTestSendBtn = new QPushButton(tr("send"));

    QGridLayout * controlerGroupLayout = new QGridLayout(this);
    QGroupBox * controlerSettingGroup = new QGroupBox(tr("Controler Settings"));
    controlerSettingGroup->setLayout(controlerGroupLayout);
    controlerGroupLayout->addWidget(controlerLbl, 0,0);
    controlerGroupLayout->addWidget(speedControlerNumSpin, 0,1);
    controlerGroupLayout->addWidget(speedControlerValueSpin, 0,2);
    controlerGroupLayout->addWidget(speedTestSendBtn, 0,3);


    QVBoxLayout * midiGroupLayout = new QVBoxLayout(this);
    QGroupBox * midiGroup = new QGroupBox(tr("Midi Connection"));
    midiGroup->setLayout(midiGroupLayout);
    midiGroupLayout->addWidget(midiConnectionRefreshBtn);
    midiGroupLayout->addWidget(midiConnectionListW);

    QVBoxLayout * toolBarLayout = new QVBoxLayout(this);
    QWidget * toolBar = new QWidget(this);
    toolBar->setLayout(toolBarLayout);
    toolBarLayout->addWidget(initKinectBtn);
    toolBarLayout->addWidget(startCaptureKinectBtn);
    toolBarLayout->addWidget(openCVTest);
    toolBarLayout->addWidget(houghSettingGroup);
    toolBarLayout->addWidget(kinectSettingGroup);
    toolBarLayout->addWidget(controlerSettingGroup);
    toolBarLayout->addWidget(midiGroup);

    QHBoxLayout * mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(glw);
    
    this->setLayout(mainLayout);
    this->setWindowTitle(tr("Kinect View"));
    this->resize(this->sizeHint());
    
    connect(initKinectBtn, SIGNAL(clicked()), glw, SLOT(startKinect()));
    connect(startCaptureKinectBtn, SIGNAL(clicked()), glw, SLOT(startCapture()));
    connect(openCVTest, SIGNAL(clicked()), glw, SLOT(testOpenCV()));

    connect(houghThresholdSpin, SIGNAL(valueChanged(int)), glw, SLOT(setHoughTreshold(int)));
    connect(maxLineGapSpin, SIGNAL(valueChanged(double)), glw, SLOT(setMaxLineGap(double)));
    connect(minLineLengthSpin, SIGNAL(valueChanged(double)), glw, SLOT(setMinLineLength(double)));
    connect(toggleDebugCannyChk, SIGNAL(stateChanged(int)), glw, SLOT(toggleDebugCanny(int)));
    connect(toggleDebugHoughChk, SIGNAL(stateChanged(int)), glw, SLOT(toggleDebugHough(int)));
    connect(toggleMeanAverageChk, SIGNAL(stateChanged(int)), glw, SLOT(toggleMeanAverage(int)));

    connect(depthFilterChk, SIGNAL(stateChanged(int)), glw, SLOT(toggleDepthFilter(int)));

    connect(midiConnectionRefreshBtn, SIGNAL(clicked()), this, SLOT(refreshMidiConnections()));
    QSpinBox * speedControlerNumSpin = new QSpinBox;
    speedControlerNumSpin->setRange(0,127);
    speedControlerNumSpin->setValue(7);
    connect(midiConnectionListW, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(connectMidi(QListWidgetItem*)));

    connect(speedTestSendBtn, SIGNAL(clicked()), this, SLOT(sendTestControlParameter()));

    connect(glw, SIGNAL(upBowStart(float,float)), this, SLOT(upBowStart(float, float)));
    connect(glw, SIGNAL(downBowStart(float,float)), this, SLOT(downBowStart(float, float)));
    connect(glw, SIGNAL(bowing(float,float)), this, SLOT(changeNoteParameter(float, float)));
    connect(glw, SIGNAL(bowStop()), this, SLOT(bowStop()));
    connect(glw, SIGNAL(angleChange(int)), this, SLOT(changeNoteKey(int)));

   

}
void MainWindow::upBowStart(float _velo, float _acc)
{
    m_midiSink->bowStart(MidiSink::UpBow);
}
void MainWindow::downBowStart(float _velo, float _acc)
{
    m_midiSink->bowStart(MidiSink::DownBow);
}
void MainWindow::bowStop()
{
    m_midiSink->bowEnd();
}

void MainWindow::changeNoteParameter(float _velo, float _asc)
{
    int velocity = qAbs(qRound(_velo * 400));

    int num = speedControlerNumSpin->value();
    speedControlerValueSpin->setValue(velocity);
    m_midiSink->controlChange(num,  velocity > 255 ? 255 : velocity );
}



void MainWindow::changeNoteKey(int angle)
{
    if(angle > 40)
        m_midiSink->changeKey(76);
    else if(angle > 28)
        m_midiSink->changeKey(69);
    else if(angle > 16)
        m_midiSink->changeKey(62);
    else if(angle > 0)
        m_midiSink->changeKey(55);

}
void MainWindow::sendTestControlParameter()
{
    int num = speedControlerNumSpin->value();
    int value = speedControlerValueSpin->value();
    m_midiSink->controlChange(num, value);
}

void MainWindow::connectMidi(QListWidgetItem* item)
{
    const QString device = item->data(Qt::UserRole).toString();
    m_midiSink->connect(device);
    qDebug() << device;
}


void MainWindow::refreshMidiConnections()
{
    while(midiConnectionListW->count() > 0) {
        QListWidgetItem * item = midiConnectionListW->takeItem(0);
        delete item;
    }
    int row = 0;
    const QMap<QString,QString>& devices = m_midiSink->devices();
    for(auto midiConnection: devices.keys()) {
        row++;
        QListWidgetItem * midiConnItem = new QListWidgetItem;
        midiConnItem->setText(devices.value(midiConnection));
        midiConnItem->setData(Qt::UserRole, QVariant( midiConnection));
        midiConnectionListW->insertItem(row,midiConnItem);
        
    }

}



