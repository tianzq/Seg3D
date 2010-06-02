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

#ifndef CORE_RESOURCE_RESOURCELOCK_H
#define CORE_RESOURCE_RESOURCELOCK_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif 

// Boost includes
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

namespace Core
{

// CLASS ResourceLock
// This class protects a resource, such as a layer. This class uses a counter
// to keep track of how many times the resource is locked or unlocked.

// NOTE: This class is *not* like a mutex, it is intended to be used in a 
// synchronous context, which means one should first check whether a resource
// is available by is_locked() and then lock it if needed. Both the is_locked 
// and Lock function *should* be called only from the application thread.
// Only Unlock can be called from anywhere.

// NOTE: This class is only useful in the Action context and keeps track of when
// an action cannot be executed because a resource is busy.

// Forward declaration
class ResourceLock;
typedef boost::shared_ptr< ResourceLock > ResourceLockHandle;

class ResourceLockPrivate;
typedef boost::shared_ptr< ResourceLockPrivate > ResourceLockPrivateHandle;

// Class definition
class ResourceLock : public boost::noncopyable
{

	// -- constructor/destructor --
public:
	ResourceLock( const std::string& name );
	virtual ~ResourceLock();

	// -- name of the resource for error reporting --
public:
	// NAME:
	// Return the name of the resource
	std::string name() const;
	
	// -- lock /unlock --
private:
	// NOTE: lock and unlock are *private* and the static functions Lock and
	// Unlock should be used instead to lock and unlock the ResourceLock. These
	// functions take the handle to the resource, which they need in case the
	// unlock needs to forwarded to a different thread.

	// TRY_LOCK:
	// Lock the resource that this lock is protecting
	bool try_lock();

	// UNLOCK:
	// Unlock the resource and wake up any threads that are waiting
	void unlock();

	// -- wait for the resource --
public:
	// WAIT:
	// Wait for the resource to become available
	void wait();

	// IS_LOCKED:
	// Check whether the resource is available
	bool is_locked();

	// -- signals --
public:
	// RESOURCE_LOCKED_SIGNAL:
	// This signal is triggered whenever the resource is locked
	typedef boost::signals2::signal< void() > resource_locked_signal_type;
	resource_locked_signal_type resource_locked_signal_;

	// RESOURCE_UNLOCKED_SIGNAL:
	// This signal is triggered whenever the resource is unlocked
	typedef boost::signals2::signal< void() > resource_unlocked_signal_type;
	resource_unlocked_signal_type resource_unlocked_signal_;

	// -- internals --
private:
	ResourceLockPrivateHandle private_;
	
	// -- public lock and unlock functions --
public:
	// TRYLOCK:
	// Lock the resource that this lock is protecting
	static bool TryLock( ResourceLockHandle& resource_lock );

	// UNLOCK:
	// Unlock the resource and wake up any threads that are waiting
	static void Unlock( ResourceLockHandle& resource_lock );
};

} // end namespace Seg3D

#endif
