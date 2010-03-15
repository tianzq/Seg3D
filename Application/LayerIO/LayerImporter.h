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

#ifndef APPLICATION_LAYERIO_LAYERIMPORTER_H
#define APPLICATION_LAYERIO_LAYERIMPORTER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif 

// STL includes
#include <vector>
#include <string>
#include <map>

// Boost includes
#include <boost/filesystem.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

// Utils includes
#include <Utils/DataBlock/DataType.h>
#include <Utils/Core/EnumClass.h>
#include <Utils/Core/Log.h>

// Application includes
#include <Application/Layer/Layer.h>

namespace Seg3D
{

// LayerImporterMode class definition
SCI_ENUM_CLASS
(
	LayerImporterMode,

	// -- DATA --
	// Convert the data into a data volume
	DATA_E = 0,

	// -- SINGLE_MASK --
	// Convert the data such that every value that is not zero is assumed to indicate the
	// mask.
	SINGLE_MASK_E = 2,
	
	// -- BIT_MASKS --
	// Every bitplane that has a bit set will be assumed to be a separate mask
	BITPLANE_MASK_E   = 3,

	// -- LABEL_MASKS --
	// Each separate number is interpreted as a separate mask
	LABEL_MASK_E = 4	
)

std::string ExportToString( LayerImporterMode mode );
bool ImportFromString( const std::string& import_type_string, LayerImporterMode& mode );

class LayerImporter;
typedef boost::shared_ptr< LayerImporter > LayerImporterHandle;

class LayerImporter : public boost::noncopyable
{
public:
	// Definitions for different importer behavior for mask
	enum LayerMaskImporterMode {

	};

	// -- Constructor/Destructor --
public:
	// Construct a new layer file importer
	LayerImporter(const std::string& filename);

	// Virtual destructor for memory management of derived classes
	virtual ~LayerImporter();

	// These functions define the properties of the importer and are implemented
	// by the SCI_IMPORTER_TYPE macro

	// Name of the importer (to be listed in the file filter dialog)
	virtual std::string name() const = 0;
	// The file types (extentions) that the importer handles
	virtual std::string file_types() const = 0;
	// Priority flag that resolves which importer needs to be used if multiple are available for
	// a similar file type
	virtual unsigned int priority() const = 0;

	// GET_FILENAME:
	// Get the filename that this importer is importing
	std::string get_filename();
	
	// GET_BASE_FILENAME
	// Get the filename without path and without extension
	std::string get_base_filename();
	
	// -- internals of the importer -- 
protected:
	// FILENAME:
	std::string filename_;
	
	// -- Import a file --
public:
	// IMPORT_HEADER:
	// Import all the information needed to translate the header and metadata information, but not
	// necessarily read the whole file. NOTE: Some external packages do not support reading a header
	// and hence these importers should read the full file here.
	virtual bool import_header();
	
	// IMPORT_DATA:
	// Import all the of the file including the data.
	virtual bool import_data(); 

	// -- Data type information --
public:

	// GET_GRID_TRANSFORM:
	// Get the grid transform of the grid that we are importing
	virtual Utils::GridTransform get_grid_transform();

	// GET_DATA_TYPE:
	// Get the type of data that is being imported
	virtual Utils::DataType get_data_type();

	// -- Import the data as a specific type --	
public:	
	// HAS_IMPORTER_MODE:
	// Test whether the importer a specific importer mode
	virtual bool has_importer_mode( LayerImporterMode mode );

	// IMPORT_LAYER
	// Import the layer from the file
	virtual bool import_layer( std::vector<LayerHandle>& layers, LayerImporterMode mode );

	// -- Error handling --
public:
	// GET_ERROR:
	// Get the last error recorded in the importer
	std::string get_error() const 
	{ 
		return error_; 
	}

protected:
	// SET_ERROR:
	// Set the error message
	void set_error( const std::string& error )
	{
		error_ = error;
	}

private:
	std::string error_;

};

// SCI_IMPORTER_TYPE:
// This macro should be defined at the top of each LayerImporter. It renders 
// code that allows both the class as well as the object to determine what its 
// properties are. By defining class specific static functions the class 
// properties can be queried without instantiating the object. On the other
// hand you want to query these properties from the object as well, even when
// we only have a pointer to the base object. Hence we need virtual functions
// as well. 

// Note: one would expect to use virtual static functions, but those are not
// allowed in C++, hence this macro ensures a consistent definition.

#define SCI_IMPORTER_TYPE(importer_name,importer_file_types,importer_priority) \
  public: \
    static std::string  Name()       { return importer_name; }\
    static std::string  FileTypes()  { return importer_file_types; }\
    static unsigned int Priority()   { return importer_priority; }\
    \
    virtual std::string  name() const       { return Name(); } \
    virtual std::string  file_types() const { return FileTypes(); } \
    virtual unsigned int priority() const   { return Priority(); }

} // end namespace seg3D

#endif
