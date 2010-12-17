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

// ITK includes
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>

// Application includes
#include <Application/LayerManager/LayerManager.h>
#include <Application/StatusBar/StatusBar.h>
#include <Application/Filters/ITKFilter.h>
#include <Application/Filters/Actions/ActionCurvatureAnisotropicDiffusionFilter.h>

// REGISTER ACTION:
// Define a function that registers the action. The action also needs to be
// registered in the CMake file.
// NOTE: Registration needs to be done outside of any namespace
CORE_REGISTER_ACTION( Seg3D, CurvatureAnisotropicDiffusionFilter )

namespace Seg3D
{

bool ActionCurvatureAnisotropicDiffusionFilter::validate( Core::ActionContextHandle& context )
{
	// Check for layer existance and type information
	std::string error;
	if ( ! LayerManager::CheckLayerExistanceAndType( this->layer_id_.value(), 
		Core::VolumeType::DATA_E, error ) )
	{
		context->report_error( error );
		return false;
	}
	
	// Check for layer availability 
	Core::NotifierHandle notifier;
	if ( ! LayerManager::CheckLayerAvailability( this->layer_id_.value(), 
		this->replace_.value(), notifier ) )
	{
		context->report_need_resource( notifier );
		return false;
	}
		
	// If the number of iterations is lower than one, we cannot run the filter
	if( this->iterations_.value() < 1 )
	{
		context->report_error( "The number of iterations needs to be larger than zero." );
		return false;
	}
	
	// Conductance needs to be a positive number
	if( this->sensitivity_.value() < 0.0 )
	{
		context->report_error( "The sensitivity needs to be larger than zero." );
		return false;
	}
	
	// Validation successful
	return true;
}

// ALGORITHM CLASS
// This class does the actual work and is run on a separate thread.
// NOTE: The separation of the algorithm into a private class is for the purpose of running the
// filter on a separate thread.

class CurvatureAnisotropicDiffusionFilterAlgo : public ITKFilter
{

public:
	LayerHandle src_layer_;
	LayerHandle dst_layer_;
	
	int	iterations_;
	double sensitivity_;
	bool preserve_data_format_;

public:
	// RUN:
	// Implemtation of run of the Runnable base class, this function is called when the thread
	// is launched.

	// NOTE: The macro needs a data type to select which version to run. This needs to be
	// a member variable of the algorithm class.
	SCI_BEGIN_TYPED_ITK_RUN( this->src_layer_->get_data_type() )
	{
		// Define the type of filter that we use.
		typedef itk::CurvatureAnisotropicDiffusionImageFilter< 
			TYPED_IMAGE_TYPE, FLOAT_IMAGE_TYPE > filter_type;

		// Retrieve the image as an itk image from the underlying data structure
		// NOTE: This only does wrapping and does not regenerate the data.
		typename Core::ITKImageDataT<VALUE_TYPE>::Handle input_image; 
		this->get_itk_image_from_layer<VALUE_TYPE>( this->src_layer_, input_image );
				
		// Create a new ITK filter instantiation.	
		typename filter_type::Pointer filter = filter_type::New();
		
		// Relay abort and progress information to the layer that is executing the filter.
		this->observe_itk_filter( filter, this->dst_layer_ );

		// Setup the filter parameters that we do not want to change.
		filter->SetInput( input_image->get_image() );
		filter->SetInPlace( false );
		filter->SetNumberOfIterations( this->iterations_ );
		
		// We changed the behavior of this parameter to be more intuitive. It is now relative to
		// the dynamic range of the data. So a similar setting will behave similarly on a
		// different data set.
		double range = this->src_layer_->get_volume()->get_cum_value( 0.99) - 
			this->src_layer_->get_volume()->get_cum_value( 0.01 );
		filter->SetConductanceParameter( this->sensitivity_ * range * 0.01 );

		// Ensure we will have some threads left for doing something else
		this->limit_number_of_itk_threads( filter );

		// Run the actual ITK filter.
		// This needs to be in a try/catch statement as certain filters throw exceptions when they
		// are aborted. In that case we will relay a message to the status bar for information.
		try 
		{ 
			filter->Update(); 
		} 
		catch ( ... ) 
		{
			if ( this->check_abort() )
			{
				this->report_error( "Filter was aborted." );
				return;
			}
			this->report_error("Could not allocate enough memory.");
			return;
		}

		// As ITK filters generate an inconsistent abort behavior, we record our own abort flag
		// This one is set when the abort button is pressed and an abort is sent to ITK.
		if ( this->check_abort() ) return;
		
		// If we want to preserve the data type we convert the data before inserting it back.
		// NOTE: Conversion is done on the filter thread and insertion is done on the application
		// thread.
		if ( this->preserve_data_format_ )
		{
			this->convert_and_insert_itk_image_into_layer( this->dst_layer_, 
				filter->GetOutput(), this->src_layer_->get_data_type() );				
		}
		else
		{
			this->insert_itk_image_into_layer( this->dst_layer_, filter->GetOutput() );	
		}
	}
	SCI_END_TYPED_ITK_RUN()
	
	// GET_FITLER_NAME:
	// The name of the filter, this information is used for generating new layer labels.
	virtual std::string get_filter_name() const
	{
		return "AnisotropicDiffusion Filter";
	}
	
	// GET_LAYER_PREFIX:
	// This function returns the name of the filter. The latter is prepended to the new layer name, 
	// when a new layer is generated. 
	virtual std::string get_layer_prefix() const
	{
		return "CurvAnisoDiff";	
	}	
};


bool ActionCurvatureAnisotropicDiffusionFilter::run( Core::ActionContextHandle& context, 
	Core::ActionResultHandle& result )
{
	// Create algorithm
	boost::shared_ptr<CurvatureAnisotropicDiffusionFilterAlgo> algo(
		new CurvatureAnisotropicDiffusionFilterAlgo );

	// Copy the parameters over to the algorithm that runs the filter
	algo->iterations_ = this->iterations_.value();
	algo->sensitivity_ = this->sensitivity_.value();
	algo->preserve_data_format_ = this->preserve_data_format_.value();

	// Find the handle to the layer
	if ( !(	algo->find_layer( this->layer_id_.value(), algo->src_layer_ ) ) )
	{
		return false;
	}

	if ( this->replace_.value() )
	{
		// Copy the handles as destination and source will be the same
		algo->dst_layer_ = algo->src_layer_;
		// Mark the layer for processing.
		algo->lock_for_processing( algo->dst_layer_ );	
	}
	else
	{
		// Lock the src layer, so it cannot be used else where
		algo->lock_for_use( algo->src_layer_ );
		
		// Create the destination layer, which will show progress
		algo->create_and_lock_data_layer_from_layer( algo->src_layer_, algo->dst_layer_ );
	}

	// Return the id of the destination layer.
	result = Core::ActionResultHandle( new Core::ActionResult( algo->dst_layer_->get_layer_id() ) );

	// Build the undo-redo record
	algo->create_undo_redo_record( context, this->shared_from_this() );

	// Start the filter.
	Core::Runnable::Start( algo );

	return true;
}


void ActionCurvatureAnisotropicDiffusionFilter::Dispatch( Core::ActionContextHandle context, 
	std::string layer_id, int iterations, double sensitivity, 
	bool preserve_data_format, bool replace )
{	
	// Create a new action
	ActionCurvatureAnisotropicDiffusionFilter* action = 
		new ActionCurvatureAnisotropicDiffusionFilter;

	// Setup the parameters
	action->layer_id_.value() = layer_id;
	action->iterations_.value() = iterations;
	action->sensitivity_.value() = sensitivity;
	action->preserve_data_format_.value() = preserve_data_format;
	action->replace_.value() = replace;

	// Dispatch action to underlying engine
	Core::ActionDispatcher::PostAction( Core::ActionHandle( action ), context );
}
	
} // end namespace Seg3D
