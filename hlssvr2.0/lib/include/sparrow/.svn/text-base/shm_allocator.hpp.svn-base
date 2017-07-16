#ifndef SHM_ALLOCATOR_HPP_201203081132
#define SHM_ALLOCATOR_HPP_201203081132

#include <cstdlib>
#include "offset_ptr.hpp"
#include <stdint.h>
#include <limits>

namespace sparrow
{
	template<typename T, typename mem_type>
	class shm_allocator
	{
	public:

       // type definitions
	   typedef T				   value_type;
       typedef offset_ptr<T>       pointer;
       typedef const offset_ptr<T> const_pointer;
       typedef T&       reference;
       typedef const T& const_reference;
       typedef std::size_t    size_type;
       typedef std::ptrdiff_t difference_type;

	   typedef offset_ptr<mem_type>		mem_pointer;

	   typedef shm_allocator<T, mem_type> this_type;

       // rebind allocator to type U
       template <class U>
       struct rebind {
           typedef shm_allocator<U, mem_type> other;
       };

	    // return address of values
       pointer address (reference value) const {
           return pointer(&value);
       };
       const_pointer address (const_reference value) const {
           return pointer(&value);
       };

       /* constructors and destructor
        * - nothing to do because the allocator has no state
        */
	   shm_allocator(void){};
	   shm_allocator(mem_type &mem) : mem_(&mem)
	   {
       };
	   shm_allocator(const this_type &rhs) : mem_(rhs.mem_)
	   {
       };
       template <class U>
	   shm_allocator(const shm_allocator<U, mem_type> &rhs) : mem_(rhs.mem_)
	   {
       };
       ~shm_allocator(void) throw() {
       };

	   this_type& operator=(const this_type &rhs){
			if(this == &rhs) return *this;
			mem_	= rhs.mem_;
			return *this;
	   };

	   template<typename U>
	   this_type& operator=(const shm_allocator<U, mem_type> &rhs){
			mem_ = rhs.mem_;
			return *this;
	   };

       // return maximum number of elements that can be allocated
       size_type max_size () const throw() {
           return std::numeric_limits<std::size_t>::max() / sizeof(T);
       };

	   // allocate but don't initialize num elements of type T
       pointer allocate (size_type num, const void* = 0) {
           // print message and allocate memory with global new
		   return pointer((T*)mem_->malloc(num * sizeof(T)));
       };

       // initialize elements of allocated storage p with value value
       void construct (pointer p, const T& value) {
           // initialize memory with placement new
           new((void*)p.get())T(value);
       };

       // destroy elements of initialized storage p
       void destroy (pointer p) {
           // destroy objects by calling their destructor
           p->~T();
       };

       // deallocate storage p of deleted elements
       void deallocate (pointer p, size_type num) {
           // print message and deallocate memory with global delete
		   mem_->free((void*)p.get());
       };
	   
	private:
		mem_pointer	mem_;

		template<typename U, typename mem_type_u>
		friend class shm_allocator;
	};

   // return that all specializations of this allocator are interchangeable
   template <class T1, class MT1, class T2, class MT2>
   bool operator== (const shm_allocator<T1, MT1>&,
                    const shm_allocator<T2, MT2>&) throw() {
       return true;
   }
   template <class T1, class MT1, class T2, class MT2>
   bool operator!= (const shm_allocator<T1, MT1>&,
                    const shm_allocator<T2, MT2>&) throw() {
       return false;
   }
}

#endif


