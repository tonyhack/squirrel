//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 13:38:57.
// File name: sequence_allocator.h
//
// Description: 
// Define class SequenceAllocator.
// This sequence allocator use to allocate sequence number, after
// using of the sequence number, should be returned to the allocator.
//

#ifndef _squirrel_global__SEQUENCE__ALLOCATOR__H
#define _squirrel_global__SEQUENCE__ALLOCATOR__H

#include "core/base/noncopyable.h"
#include "core/base/mutex.h"
#include "core/base/scoped_mutex.h"
#include "core/base/types.h"

namespace squirrel {

namespace global {

/*
 * This SequenceAllocator is like the follows:
 *  res: current_response_sequence_.
 *  cur: current_sequence_.
 *
 *  ---+---+---+---+---+---+---+---+---+     +---
 *   0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | ... | 0  
 *  ---+---+---+---+---+---+---+---+---+     +---
 *       ^                       ^
 *       |->                     |->
 *      res                     cur
 *  
 * 
 *  <1> Allocate() ==> 8 :
 *  ---+---+---+---+---+---+---+---+---+     +---
 *   0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | ... | 0  
 *  ---+---+---+---+---+---+---+---+---+     +---
 *       ^                           ^
 *       |->                         |->
 *      res                         cur
 * 
 *  <2> Deallocate(1) :
 *  ---+---+---+---+---+---+---+---+---+     +---
 *   0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | ... | 0  
 *  ---+---+---+---+---+---+---+---+---+     +---
 *           ^                       ^
 *           |->                     |->
 *          res                     cur
 *
 * deallocate_sequences_:
 *  When the given deallocate sequence is not the sequence needed(here is 2),
 *  for example 6, this sequence(6) will be inserted into the deallocate list,
 *  and when "res" is moved to 5, the "res" will detected 6 has been deallocated,
 *  so "res" will point to position 7.
 * 
 */
template <typename Mutex, size_t HighWaterValue = 256 * 1024>
class SequenceAllocator : public core::Noncopyable {
  typedef std::set<core::uint64> DeallocateSet;
 public:
  explicit SequenceAllocator(core::uint64 init_sequence = 0)
    : current_sequence_(init_sequence), current_response_sequence_(0) {}
  ~SequenceAllocator() {}

  bool Allocate(core::uint64 &sequence) {
    core::ScopedMutex<Mutex> auto_mutex(&this->mutex_);
    if(this->deallocate_sequences_.size() >= HighWaterValue) {
      return false;
    } else {
      if(this->current_sequence_ + 1 == this->current_response_sequence_) {
        return false;
      } else {
        sequence = this->current_sequence_++;
        return true;
      }
    }
  }

  void Deallocate(core::uint64 sequence) {
    core::ScopedMutex<Mutex> auto_mutex(&this->mutex_);
    if(this->current_response_sequence_ == sequence) {
      ++this->current_response_sequence_;
      DeallocateSet::iterator iterator = this->deallocate_sequences_.find(
          this->current_response_sequence_);
      while(iterator != this->deallocate_sequences_.end()) {
        this->deallocate_sequences_.erase(iterator);
        iterator = this->deallocate_sequences_.find(
            ++this->current_response_sequence_);
      }
    } else {
      this->deallocate_sequences_.insert(sequence);
    }
  }

 private:
  core::uint64 current_sequence_;
  core::uint64 current_response_sequence_;

  DeallocateSet deallocate_sequences_;

  Mutex mutex_;
};

class NullMutex {
 public:
  NullMutex() {}
  ~NullMutex() {}
  void Lock() {}
  void Unlock() {}
};

// Nonthread-safe.
typedef SequenceAllocator<NullMutex> CommonSequenceAllocator;
// Thread-safe.
typedef SequenceAllocator<core::Mutex> MTSequenceAllocator;

}  // namespace global

}  // namespace squirrel

#endif  // _squirrel_global__SEQUENCE__ALLOCATOR__H

