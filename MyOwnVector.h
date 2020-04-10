#pragma once
#include<iostream>
template<typename T>
class MyOwnVector
{
public:
	MyOwnVector() :size(0), capacity(0), data(nullptr) {};
	MyOwnVector(std::size_t size, const T& value) :capacity(size), size(size), data(static_cast<T*>(operator new (size * sizeof(T)))) {
		for (std::size_t i = 0; i < size; i++) {
			new(data+i)T(std::move(value));
		}
	}
	MyOwnVector(const MyOwnVector<T>& vector) :capacity(vector.capacity), size(vector.size), data(static_cast<T*>(operator new (vector.size*sizeof(T)))) {
		for (std::size_t i = 0; i < size; i++) {
			new(data+i)T(std::move(vector.data[i]));
		}
	}
	MyOwnVector(MyOwnVector<T>&& vector) :capacity(vector.capacity), size(vector.size), data(vector.data) {
		vector.data = nullptr;
	}
	MyOwnVector(std::initializer_list<T> list) :capacity(list.size()), size(list.size()), data(static_cast<T*>(operator new (list.size * sizeof(T)))) {
		std::size_t i = 0;
		for (auto x: list) {
			new(data + i)T(x);
		}
	}
	~MyOwnVector() {
		delete[] data;
	}
	std::size_t get_size() const { return size; }
	std::size_t get_capacity() const { return capacity; }
	T& operator[](std::size_t index) {
		if (index >= size) {
			throw std::out_of_range("Index out of range");
		}
		return data[index];
	}
	MyOwnVector& operator =(const MyOwnVector<T>& vector) {
		if (&vector != this) {
			delete[] data;
			capacity = vector.capacity;
			size = vector.size;
			data = static_cast<T*>(operator new (size*sizeof(T)));
			for (std::size_t i = 0; i < size; i++) {
				 new(data+i) T(std::move(vector.data[i]));
			}
		}
		return *this;
	}
	MyOwnVector& operator =(MyOwnVector<T>&& vector) {
		if (&vector != this) {
			delete[] data;
			capacity = vector.capacity;
			size = vector.size;
			data = vector.data;
			vector.data = nullptr;
		}
		return *this;
	}
	void push_back(const T& elem) {
		if (capacity == size) {
			std::size_t temp_capacity = capacity == 0 ? 1 : capacity * 2;
			T* temp = static_cast<T*>(operator new(temp_capacity*sizeof(T)));
			for (std::size_t i = 0; i < capacity; i++) {
				new(temp+i) T(std::move(elem));
			}
			capacity = temp_capacity;
			delete[] data;
			data = temp;
		}
		new(data+(size++))T(std::move(elem));
	}

	void resize(std::size_t size) {
		if (size < this->size) {
			T* temp = static_cast<T*>(operator new(size * sizeof(T)));
			for (std::size_t i = 0; i < capacity; i++) {
				new(temp + i) T(std::move(data[i]));
			}
			capacity = size;
			delete[] data;
			data = temp;
		}
		if (size > this->size) {
			reserve(size);
			/*for (std::size_t i = this->size; i < size; i++) {
				new(data+i)T();
			}*/
		}
		this->size = size;
	}
	void resize(std::size_t size, const T& value) {
		if (size < this->size) {
			resize(size);
			return;
		}
		if (size > this->size) {
			reserve(size);
			for (std::size_t i = this->size; i < size; i++) {
				new(data+i)T(std::move(value));
			}
		}
		this->size = size;
	}
	void reserve(std::size_t size) {
		if (size > capacity) {
			std::size_t temp_capacity = size;
			T* temp = static_cast<T*>(operator new(temp_capacity * sizeof(T)));
			for (std::size_t i = 0; i < this->size; i++) {
				new(temp+i)T(std::move(data[i]));
			}
			delete[] data;
			capacity = temp_capacity;
			data = temp;
		}
	}
	void clear() {
		for (std::size_t i = 0; i < size;i++) {
			data[i].~T();
		}
		size = 0;
		data = nullptr;
	}
	bool empty() {
		return (size==0);
	}
	const T* begin() {
		return std::_Const_cast<const T*>(data);
	}
	const T* end() {
		return std::_Const_cast<const T*>(data + size);
	}
private:
	std::size_t capacity;
	std::size_t size;
	T* data;
};

