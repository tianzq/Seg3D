/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2009 Scientific Computing and Imaging Institute,
   University of Utah.

   
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/

// For the version numbers
#include "Seg3DConfiguration.h"

#include <Interface/AppInterface/AppInterface.h>

namespace Seg3D {

AppInterface::AppInterface(QApplication* app)
{
  // Set the window information
  // and set the version numbers
  setWindowTitle(QString("Seg3D Version ")+SEG3D_VERSION);
  setWindowIconText(QString("Seg3D"));
  setDocumentMode(true);
  resize(1280, 720);
  
  // Tell Qt where to doc the toolbars
  setCorner(Qt::TopLeftCorner,Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner,Qt::RightDockWidgetArea);
  setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomRightCorner,Qt::RightDockWidgetArea);
  
    

  viewer_interface_ =        new ViewerInterface(this);
  history_dock_window_ =     new HistoryDockWidget(this);
  project_dock_window_ =	 new ProjectDockWidget(this);
  tools_dock_window_ =		 new ToolsDockWidget(this);

  setCentralWidget(viewer_interface_);

// call function to create menu bar	

  createStatusBar();
	

  addDockWidget(Qt::LeftDockWidgetArea,history_dock_window_, Qt::Horizontal);
  addDockWidget(Qt::LeftDockWidgetArea,project_dock_window_, Qt::Horizontal);
  addDockWidget(Qt::LeftDockWidgetArea,tools_dock_window_, Qt::Horizontal);
	
	tabifyDockWidget(project_dock_window_, history_dock_window_);
	tabifyDockWidget(history_dock_window_, tools_dock_window_);
  

  
  application_menu_ = new AppMenu(this);
}

	
	void AppInterface::createStatusBar()
	{
		coordinatesLabel = new QLabel(" x: y: ");
		coordinatesLabel->setAlignment(Qt::AlignHCenter);
		coordinatesLabel->setMinimumSize(coordinatesLabel->sizeHint());
		coordinatesLabel->setStyleSheet("margin-left: 2px;");
		
		focusLabel = new QLabel("Focus: ");
		focusLabel->setIndent(3);
		
		statusBar()->addWidget(coordinatesLabel);
		statusBar()->addWidget(focusLabel, 1);
		statusBar()->setStyleSheet("padding-left: 2px;");
		
	}
	

AppInterface::~AppInterface()
{
}







} //end namespace
