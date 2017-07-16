#ifndef ARRAY_HPP_201207231858
#define ARRAY_HPP_201207231858

namespace sparrow
{
	template<typename T, size_t N>
	class array
	{
	public:
		array(void) : 
		size_(0)
		{};

		size_t capacity(void) const {return N;};
		size_t size(void) const {return size_;};
		void size(const size_t s) {size_ = s;};

		T* ptr(void){return elems_;};
		const T* ptr(void) const{return elems_;};

		T& operator[](const size_t i){return *(elems_ + i);};
		const T& operator[](const size_t i) const {return *(elems_ + i);};

		bool push_back(const T &val){
			if(size_ >= N) return false;
			*(elems_ + size_) = val;
			++ size_;
		};
		void pop_back(void){
			-- size_;
		};
		T* back(void){
			if(size_ == 0) return NULL;
			return elems_ + (size_ - 1);
		};
		const T* back(void) const {
			if(size_ == 0) return NULL;
			return elems_ + (size_ - 1);
		};

		void clear(void){
			size_ = 0;
		};

	private:
		size_t	size_;
		T	elems_[N];
	};
}

#endif
