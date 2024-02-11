#pragma once
#include <Windows.h>
#include "malutils.hpp"

namespace mtl {


template <typename T>
class dynamicArraySkeleton {

	protected:
		T* buffer;
		size_t length;
		size_t size;
		size_t capacity;
		bool err = false;



		void reAllocCapacity(size_t amnt, DWORD dwFlags) {

			capacity += amnt;

			buffer = static_cast<T*>(HeapReAlloc(GetProcessHeap(),
				dwFlags,
				buffer,
				capacity * sizeof(T)));
		}


		bool compare(T* other, size_t otherLength) {

			if (otherLength != length)
				return false;

			for (size_t i = 0; i < length; i++) {
				if (buffer[i] != other[i])
					return false;
			}

			return true;
		}


		template<typename ...Args>
		dynamicArraySkeleton(Args... args) {

			this->length = 0;
			this->size = 0;
			this->capacity = 0;

			T arr[] = { args... };
			for (T& i : arr) {
				length++;
			}

			size += sizeof(T) * length;
			capacity = length * 2;

			buffer = static_cast<T*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				capacity * sizeof(T) ));

			if (err = (buffer == nullptr))
				return;
			

			mtl::memcpy(buffer, arr, size);
		}

		dynamicArraySkeleton() : size(0), length(0) {
			
			capacity = 1;
			buffer = static_cast<T*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				sizeof(T)
			));
		}



	private:
		

	public:


		/* Find Operations */

		T& operator[](size_t index) {

			if (index >= length || !length)
				return *buffer;

			return this->buffer[index];
		}

		T& back() {
			if (!length)
				return *buffer; //buffer will never be null

			return buffer[length - 1];
		}

		T& front() {

			return buffer[0];
		}

		int exists(T value) {
			
			for (size_t i = 0; i < length; i++) {
				if (buffer[i] == value)
					return i;
			}

			return -1;
		}


		
		/* Deletion */

		T pop() {

			if (!length || !size)
				return (T)0x00;
		
			length--;
			size -= sizeof(T);
			return buffer[length];
		}

		void shrink_to_fit() {

			if (capacity < 2 || !buffer || capacity == length)
				return;
			
			capacity = length;
			buffer = static_cast<T*>(HeapReAlloc(
				GetProcessHeap(),
				0x00,
				buffer,
				length * sizeof(T)
			));
		}

		void clear() {
			
			length = 0;
			size = 0;
			capacity = 1;
		
			if (!buffer) {
				return;
			}

			buffer = static_cast<T*>(HeapReAlloc(
				GetProcessHeap(),
				0,
				buffer,
				sizeof(T)
			));

			if (err = (buffer == nullptr))
				return;
			
			buffer[0] = 0x00;
		}



		/* Getters */

		size_t max_capacity() {
			return this->capacity;
		}

		size_t len() {
			return this->length;
		}

		size_t size_bytes() {
			return this->size;
		}

		bool is_ok() {
			return !err;
		}

		bool empty() {
			return (!this->length);
		}


		/* Misc */
		void map(mtl::arrayFuncPtr<T> funcPtr) {

			if (!length || !size)
				return;

			for (size_t i = 0; i < length; i++) {
				funcPtr(buffer[i]);
			}
		}


		/* Delete  */
		
		~dynamicArraySkeleton() {

			if (this->buffer)
				HeapFree(GetProcessHeap(), 0, buffer);
		}
};





#define vecLen this->length
#define vecSize this->size
#define vecCap this->capacity
#define vecBuf this->buffer

template <typename T>
class vector : public dynamicArraySkeleton<T> {

	protected:

	private:

	public:


		/* Operators */

		bool operator==(vector<T>& other) {
			return this->compare(other.buffer, other.length);
		}

		bool operator+=(vector<T>& other) {
			return append(other);
		}

		template<typename ...Args>
		void operator+=(Args... args) {

			T arr[] = { args... };

			for (T& i : arr)
				push_back(i);
		}

		bool operator -=(size_t amnt) {

			if (amnt > vecLen)
				return false;

			return erase(vecLen - amnt, amnt);
		}



		/* Insertion */

		bool push_back(T value) {

			vecLen++;
			vecSize += sizeof(T);

			while (vecCap < vecLen) {
				this->reAllocCapacity(vecCap, HEAP_ZERO_MEMORY);

				if (this->err = (vecBuf == nullptr))
					return false;
			}

			vecBuf[vecLen - 1] = value;
			return true;
		}

		
		bool emplace_back(T value) {

			vecLen++;
			vecSize += sizeof(T);

			while (vecCap < vecLen) {
				this->reAllocCapacity(vecCap, HEAP_REALLOC_IN_PLACE_ONLY);
			}

			if (this->err = (vecBuf == nullptr))
				return false;

			vecBuf[vecLen - 1] = value;
			return true;
		}


		bool insert(size_t index, T value) {

			if (vecLen < 1 || index > (vecLen - 1))
				return false;

			if (index == vecLen - 1)
				return push_back(value);
	

			vecLen++;
			vecSize += sizeof(T);
			if (vecCap < vecLen) {
				this->reAllocCapacity(vecCap, HEAP_ZERO_MEMORY);
			}


			size_t i = vecLen - 1;
			for (i; i != index; i--) {
				vecBuf[i] = vecBuf[i - 1];
			}

			vecBuf[i] = value;
		}


		bool append(vector<T>& other) {

			while (vecLen + other.length > vecCap) {
				this->reAllocCapacity(vecCap, HEAP_ZERO_MEMORY);

				if (this->err = (vecBuf == nullptr))
					return false;
			}

			for (size_t i = vecLen, k = 0; k < other.length-1; i++, k++)
				vecBuf[i] = other.buffer[k];
			
			return true;
		}


		template<typename ...Args>
		void append(Args... args) {

			T arr[] = { args... };
			
			for (T& i : arr)
				push_back(i);
		}


		bool prepend(vector<T>& other) {

			vecLen += other.length;

			while (vecCap < vecLen)
				vecCap *= 2;

			T* newBuffer = static_cast<T*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				vecCap * sizeof(T)
			));

			if (this->err = (newBuffer == nullptr))
				return false;


			mtl::memcpy(newBuffer, other.buffer, other.size);
			mtl::memcpy(&newBuffer[other.length], vecBuf, vecSize);
			vecSize += other.size;

			HeapFree(GetProcessHeap(), 0, vecBuf);
			vecBuf = newBuffer;
			
			return true;
		}


		/* Deletion */

		bool erase(size_t index, size_t amntToErase) {

			if (vecLen == 0 || index >= vecLen || vecLen - index < amntToErase)
				return false;


			T* newBuffer = static_cast<T*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				vecCap * sizeof(T)));
			
			if (this->err = (newBuffer == nullptr))
				return false;
		
			
			size_t cnt = amntToErase;
			for (size_t i = 0, k = 0; i < vecLen; i++) {

				if (i >= index && cnt) {
					cnt--;
					continue;
				}

				newBuffer[k] = vecBuf[i];
				k++;
			}

			vecSize -= amntToErase * sizeof(T);
			vecLen -= amntToErase;

			HeapFree(GetProcessHeap(), 0, vecBuf);
			vecBuf = newBuffer;
			return true;
		}



		/* Misc */

		void reverse(){
			
			if (vecLen < 1)
				return;


			T* newBuffer = static_cast<T*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				vecCap * sizeof(T)
			));

			if (this->err = (newBuffer == nullptr))
				return;


			for (signed int i = vecLen - 1, k = 0; i >= 0; i--, k++) {
				newBuffer[k] = vecBuf[i];
			}

			HeapFree(GetProcessHeap(), 0, vecBuf);
			vecBuf = newBuffer;
		}


		bool assign(mtl::vector<T>& other, size_t amountToAssign) {

			if (amountToAssign > vecLen || other.length < 1 || amountToAssign > other.length)
				return false;

			for (size_t i = 0; i < amountToAssign; i++) {
				vecBuf[i] = other.buffer[i];
			}
		}


		void sort() {
			mtl::sort<T>(vecBuf, vecLen);
		}



		/* Constructors */

		vector() : dynamicArraySkeleton<T>() {}

		template<typename ...Args>
		vector(Args...args) : dynamicArraySkeleton<T>(args...) {}
};






#define stackLen this->length
#define stackSize this->size
#define stackCap this->capacity
#define stackBuf this->buffer

template <typename T>
class stack : public dynamicArraySkeleton<T> {

	protected:

	private:

	public:

		void emplace_push(T value) {

			stackSize += sizeof(T);
			stackLen++;

			while (stackCap < stackLen)
				this->reAllocCapacity(vecCap, HEAP_REALLOC_IN_PLACE_ONLY);

			stackBuf[stackLen - 1] = value;

			this->err = (stackBuf == nullptr);
		}

		stack() : dynamicArraySkeleton() {}

		template<typename ...Args>
		stack(Args...args) : dynamicArraySkeleton<T>(args...) {}
};




template <typename T>
class queue {

	private:
		size_t length;
		size_t capacity;
		size_t size;
		T* buffer;
		bool err = false;

	public:
		
		/* Operators */

		T& operator[](size_t index) {
			if (index >= length || !length)
				return *buffer;

			return buffer[length - 1 - index];
		}


		/* Find Operations */

		T& front() {
			if (!length || !size)
				return (T)0x00;

			return buffer[length - 1];
		}


		T& back() {
			if (!length || !size)
				return (T)0x00;

			return *buffer;
		}


		int exists(T value) {

			if (!length || !size)
				return -1;

			for (size_t i = length - 1, j = 0; i >= 0; i--, j++) {
				if (buffer[i] == value)
					return j;
			}

			return -1;
		}


		/* Insertion */

		//Insert an element at the front of the queue.
		void push(T value) {

			length++;
			size += sizeof(T);

			while (capacity < length) {

				capacity *= 2;
				buffer = static_cast<T*>(HeapReAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					buffer,
					capacity * sizeof(T)
				));

				if (err = (buffer == nullptr))
					return;
			}

			buffer[length - 1] = value;
		}


		T pop() {

			if (!length || !size)
				return (T)0x00;

			T valueToRetn = *buffer;

			for (size_t i = 0; i < this->length; i++) {
				buffer[i] = buffer[i + 1];
			}

			length--;
			size -= sizeof(T);
			return valueToRetn;
		}



		/* Misc */

		bool operator==(queue<T>& other) {
			if (!other.length || other.length != this->length)
				return false;

			for (size_t i = 0; i < length; i++) {
				if (other.buffer[i] != this->buffer[i])
					return false;
			}

			return true;
		}


		void clear() {
			if (!length || !size)
				return;

			HeapFree(GetProcessHeap(), 0, buffer);

			this->buffer = static_cast<T*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				sizeof(T)
			));
		
			length = 0;
			capacity = 1;
			size = 0;
		}


		void shrink_to_fit() {
			
			if (!length || !size)
				return;

			capacity = length;
			this->buffer = static_cast<T*>(HeapReAlloc(
				GetProcessHeap(),
				0,
				this->buffer,
				capacity * sizeof(T)
			));

			err = (buffer == nullptr);
		}



		/* Getters */

		size_t max_capacity() {
			return this->capacity;
		}

		size_t len() {
			return this->length;
		}

		size_t size_bytes() {
			return this->size;
		}

		bool is_ok() {
			return !err;
		}

		bool empty() {
			return (!this->length);
		}

		

		/* Constructors */

		template<typename ...Args>
		queue(Args... args) {

			length = 0;
			capacity = 0;
			size = 0;

			T arr[] = { args... };

			for (T& i : arr)
				length++;
		

			size = length * sizeof(T);
			capacity = length * 2;
			buffer = static_cast<T*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				capacity * sizeof(T)
			));


			size_t count = length - 1;
			for (T& i : arr) {
				buffer[count] = i;
				count--;
			}

			err = (buffer == nullptr);
		}


		queue() : length(0), size(0) {

			capacity = 1;
			buffer = static_cast<T*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				sizeof(T)
			));

			err = (buffer == nullptr);
		}


		~queue() {
			if (buffer)
				HeapFree(GetProcessHeap(), 0, buffer);
		}
};

}