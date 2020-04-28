#pragma once
#include<iostream>
namespace MY {
	template<typename T>
	class MyOwnVector;
	template<typename T>
	class vector_iterator {
	public:
		friend class MyOwnVector<T>;

		vector_iterator() = default;
		vector_iterator(const vector_iterator<T>& iterator) :my_elem(iterator.my_elem) {}
		vector_iterator& operator=(const vector_iterator<T>& iterator) {
			my_elem = iterator->my_elem;
			return *this;
		}
		vector_iterator& operator++() {
			++my_elem;
			return *this;
		}
		vector_iterator& operator++(int) {
			vector_iterator<T>* tmp = new vector_iterator<T>(*this);
			++my_elem;
			return *tmp;
		}
		vector_iterator& operator--() {
			--my_elem;
			return *this;
		}
		vector_iterator& operator--(int) {
			vector_iterator<T> tmp = this;
			--my_elem;
			return tmp;
		}
		vector_iterator& operator+(int n) {
			return vector_iterator<T>(my_elem + n);
		}
		vector_iterator& operator-(int n) {
			return vector_iterator<T>(my_elem - n);
		}
		vector_iterator& operator+=(int n) {
			my_elem += n;
			return *this;
		}
		vector_iterator& operator-=(int n) {
			my_elem -= n;
			return *this;
		}

		bool operator==(const vector_iterator<T>& iterator) {
			return my_elem == iterator.my_elem;
		}
		bool operator!=(const vector_iterator<T>& iterator) {
			return my_elem != iterator.my_elem;
		}
		bool operator>(const vector_iterator<T>& iterator) {
			return my_elem > iterator.my_elem;
		}
		bool operator<(const vector_iterator<T>& iterator) {
			return my_elem < iterator.my_elem;
		}
		bool operator>=(const vector_iterator<T>& iterator) {
			return my_elem >= iterator.my_elem;
		}
		bool operator<=(const vector_iterator<T>& iterator) {
			return my_elem <= iterator.my_elem;
		}

		T& operator*() {
			return *my_elem;
		}
		T* operator->() {
			return my_elem;
		}

		int operator-(const vector_iterator& iterator) {
			return my_elem - iterator.my_elem;
		}
		T& operator[](int n) {
			return (*(my_elem+n));
		}

	private:	
		vector_iterator(const T* pointer) :my_elem(const_cast<T*>(pointer)) {
		
		}
		T* my_elem;
	};
	template<typename T>
	class MyOwnVector
	{
	public:
		MyOwnVector() :my_size(0), my_capacity(0), my_data(nullptr) {};
		MyOwnVector(std::size_t size, const T& value) :my_capacity(size), my_size(size), my_data(static_cast<T*>(operator new (size * sizeof(T)))) {
			for (std::size_t i = 0; i < size; i++) {
				new(my_data + i)T(value);
			}
		}
		MyOwnVector(const MyOwnVector<T>& vector) :my_capacity(vector.my_capacity), my_size(vector.my_size), my_data(static_cast<T*>(operator new (vector.my_size * sizeof(T)))) {
			for (std::size_t i = 0; i < my_size; i++) {
				new(my_data + i)T(vector.my_data[i]);
			}
		}
		MyOwnVector(MyOwnVector<T>&& vector) :my_capacity(vector.my_capacity), my_size(vector.my_size), my_data(vector.my_data) {
			vector.my_capacity = 0;
			vector.my_size = 0;
			vector.my_data = nullptr;
		}
		MyOwnVector(std::initializer_list<T> list) :my_capacity(list.size()), my_size(list.size()), my_data(static_cast<T*>(operator new (list.size * sizeof(T)))) {
			std::size_t i = 0;
			for (auto& x : list) {
				new(my_data + i)T(x);
			}
		}
		~MyOwnVector() {
			for (std::size_t i = 0; i < my_size; i++) {
				(my_data + i)->~T();
			}
			my_capacity = 0;
			my_size = 0;
			operator delete(my_data);
		}
		std::size_t get_size() const { return my_size; }
		std::size_t get_capacity() const { return my_capacity; }
		T& operator[](std::size_t index) {
			if (index >= my_size) {
				throw std::out_of_range("Index out of range");
			}
			return my_data[index];
		}
		MyOwnVector& operator =(const MyOwnVector<T>& vector) {
			if (&vector != this) {
				my_capacity = vector.my_capacity;
				my_size = vector.my_size;
				for (std::size_t i = 0; i < my_size; i++) {
					(my_data + i)->~T();
				}
				if (my_capacity < vector.my_size) {
					operator delete(my_data);
					my_data = static_cast<T*>(operator new (my_size * sizeof(T)));
				}
				for (std::size_t i = 0; i < my_size; i++) {
					new(my_data + i) T(vector.my_data[i]);
				}
			}
			return *this;
		}
		MyOwnVector& operator =(MyOwnVector<T>&& vector) {
			if (&vector != this) {
				delete[] my_data;
				my_capacity = vector.my_capacity;
				my_size = vector.my_size;
				my_data = vector.my_data;
				vector.my_data = nullptr;
			}
			return *this;
		}
		void push_back(const T& elem) {
			push_back_forwarding(elem);
		}
		void push_back(T&& elem) {
			push_back_forwarding(std::move(elem));
		}

		void pop_back() {
			(my_data + my_size)->~T();
			--my_size;
		}
		void resize(std::size_t size) {
			if (size < this->my_size) {
				T* temp = static_cast<T*>(operator new(size * sizeof(T)));
				for (std::size_t i = 0; i < my_capacity; i++) {
					new(temp + i) T(std::move(my_data[i]));
				}
				my_capacity = size;
				delete[] my_data;
				my_data = temp;
			}
			if (size > my_size) {
				reserve(size);
			}
			this->my_size = size;
		}
		void resize(std::size_t size, const T& value) {
			if (size < my_size) {
				resize(size);
				return;
			}
			if (size > this->my_size) {
				reserve(size);
				for (std::size_t i = my_size; i < size; i++) {
					new(my_data + i)T(std::move(value));
				}
			}
			my_size = size;
		}
		void reserve(std::size_t size) {
			if (size > my_capacity) {
				std::size_t temp_capacity = size;
				T* temp = static_cast<T*>(operator new(temp_capacity * sizeof(T)));
				for (std::size_t i = 0; i < my_size; i++) {
					new(temp + i)T(std::move(my_data[i]));
				}
				for (std::size_t i = 0; i < my_size; i++) {
					(my_data + i)->~T();
				}
				operator delete(my_data);
				my_capacity = temp_capacity;
				my_data = temp;
			}
		}
		void clear() {
			for (std::size_t i = 0; i < my_size; i++) {
				my_data[i].~T();
			}
			my_size = 0;
		}
		bool empty() {
			return (my_size == 0);
		}
		const T* begin() {
			return const_cast<const T*>(my_data);
		}
		const T* end() {
			return const_cast<const T*>(my_data + my_size);
		}
		vector_iterator<T>& get_iterator() {
			return *(new vector_iterator<T>(begin()));
		}
	private:
		std::size_t my_capacity;
		std::size_t my_size;
		T* my_data;
		template<typename U>
		void push_back_forwarding(U&& elem) {
			if (my_capacity == my_size) {
				std::size_t temp_capacity = my_capacity == 0 ? 1 : my_capacity * 2;
				reserve(temp_capacity);
			}
			new(my_data + my_size)T(std::forward<U>(elem));
			++my_size;
		}
	};
}
