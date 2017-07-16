#ifndef OFFSET_PTR_HPP_201203091240
#define OFFSET_PTR_HPP_201203091240

#include <cstdlib>

namespace sparrow
{
	template<typename T>
	class offset_ptr
	{
		union{
			std::ptrdiff_t	v_offset;
			T*				align_t;

		} offset_;

	private:
		void set_offset(const void *p){
			if(NULL == p){
				offset_.v_offset = 0;
			}else{
				offset_.v_offset = (const char*)p - (const char*)this;
			}
		};

	public:
		typedef offset_ptr<T>  this_type;

	public:
		offset_ptr(void){
			set_offset(NULL);
		};
		offset_ptr(const T *ptr){
			set_offset(ptr);
		};

		template<typename U>
		offset_ptr(const U *ptr){
			set_offset(ptr);
		};

		offset_ptr(const this_type &rhs){
			set_offset(rhs.get());
		};

		offset_ptr& operator=(const T *ptr){
			set_offset(ptr);
			return *this;
		};

		offset_ptr& operator=(const this_type& rhs){
			set_offset(rhs.get());
			return *this;
		};

		template<typename U>
		offset_ptr& operator=(const U *ptr){
			set_offset(ptr);
			return *this;
		};
		template<typename U>
		offset_ptr& operator=(const offset_ptr<U> &optr){
			set_offset(optr.get());
			return *this;
		};


		const T* get(void) const {
			if(0 == offset_.v_offset) return NULL;
			return (const T*)((const char*)this + offset_.v_offset);
		};

		T* get(void){
			if(0 == offset_.v_offset) return NULL;
			return (T*)((char*)this + offset_.v_offset);
		};

		T* operator->(void){
			return get();
		};

		const T* operator->(void) const{
			return get();
		};

		T& operator*(void){
			return *get();
		};

		const T& operator*(void) const{
			return *get();
		};

		const T& operator[](size_t idx) const{
			return *(get() + idx);
		};
		T& operator[](size_t idx){
			return *(get() + idx);	
		};

		T* operator++(void){
			T* ptr = get();
			++ptr;
			set_offset(ptr);
			return ptr;
		};

		T* operator++(int){
			T *ptr = get();
			++*this;
			return ptr;
		};

		const T* operator+(int offset) const{
			const T *ptr = get();
			return (ptr + offset);
		};
		T* operator+(int offset){
			T *ptr = get();
			return (ptr + offset);
		};

		T* operator--(void){
			T *ptr = get();
			--ptr;
			set_offset(ptr);
			return ptr;
		};
		T* operator--(int){
			T *ptr = get();
			--*this;
			return ptr;
		};
		const T* operator-(int offset) const{
			return (get() - offset);
		};
		T* operator-(int offset){
			return (get() - offset);
		};

		template<typename U>
		bool operator>(const offset_ptr<U> &r) const {
			return (get() > r.get());
		};
		template<typename U>
		bool operator>(const U *p) const{
			return (get() > p);
		};

		template<typename U>
		bool operator<(const offset_ptr<U> &r) const {
			return (get() < r.get());
		};
		template<typename U>
		bool operator<(const U *p) const{
			return (get() < p);
		};

		/*bool operator==(const this_type &r){
			return (get() == r.get());
		};

		bool operator==(const T *p){
			return (get() == p);
		};*/

		template<typename U>
		bool operator==(const offset_ptr<U> &r) const{
			return (get() == r.get());
		};

		template<typename U>
		bool operator==(const U *p) const{
			return (get() == p);
		};

		template<typename U>
		bool operator!=(const offset_ptr<U> &r) const {
			return (get() != r.get());
		};
		template<typename U>
		bool operator!=(const U *p) const{
			return (get() != p);
		};
	};
}

#endif
