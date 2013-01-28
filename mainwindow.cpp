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
#include <QtGui/QPushButton>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    glw = new QtGLWidget(this);

    QPushButton * initKinectBtn = new QPushButton(tr("start kinect"));
    QPushButton * startCaptureKinectBtn = new QPushButton(tr("start Capturing"));
    QVBoxLayout * toolBarLayout = new QVBoxLayout(this);
    QWidget * toolBar = new QWidget(this);
    toolBar->setLayout(toolBarLayout);
    toolBarLayout->addWidget(initKinectBtn);
    toolBarLayout->addWidget(startCaptureKinectBtn);

    QHBoxLayout * mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(glw);
    
    this->setLayout(mainLayout);
    this->setWindowTitle(tr("Kinect View"));
    this->resize(this->sizeHint());
    
    connect(initKinectBtn, SIGNAL(clicked()), glw, SLOT(startKinect())); 

}




