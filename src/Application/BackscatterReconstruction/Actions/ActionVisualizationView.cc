/*
 For more information, please see: http://software.sci.utah.edu
 
 The MIT License
 
 Copyright (c) 2013 Scientific Computing and Imaging Institute,
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

#include <Core/Action/ActionDispatcher.h>
#include <Core/Action/ActionFactory.h>
#include <Core/State/Actions/ActionSet.h>

// Application Includes
#include <Application/BackscatterReconstruction/Actions/ActionVisualizationView.h>

#include <Application/ToolManager/Actions/ActionOpenTool.h>
#include <Application/ViewerManager/ViewerManager.h>

// REGISTER ACTION:
// Define a function that registers the action. The action also needs to be
// registered in the CMake file.
CORE_REGISTER_ACTION( Seg3D, VisualizationView )

namespace Seg3D
{
  
  bool ActionVisualizationView::validate( Core::ActionContextHandle& context )
  {
    return true; // validated
  }
  
  bool ActionVisualizationView::run( Core::ActionContextHandle& context,
                                     Core::ActionResultHandle& result )
  {
    // TODO: change to tool&dock widget customization
    Core::ActionSet::Dispatch( Core::Interface::GetWidgetActionContext(),
      ViewerManager::Instance()->layout_state_, ViewerManager::VIEW_1AND3_C );
    
    return true;
  }
  
  void ActionVisualizationView::Dispatch( Core::ActionContextHandle context )
  {
    ActionVisualizationView* action = new ActionVisualizationView;
    Core::ActionDispatcher::PostAction( Core::ActionHandle( action ), context );
  }
  
} // end namespace Seg3D
