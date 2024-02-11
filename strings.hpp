#pragma once
#include <Windows.h>
#include "malutils.hpp"


namespace mtl{



class string {

	private:
		char* buffer;
		size_t length;
		size_t size;
		bool err = false;


		string& appendPrivate(const char* str){
			
			this->size += mtl::strlen(str) * sizeof(char);

			buffer = static_cast<char*>(HeapReAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				this->buffer,
				this->size
			));

			if (err = (buffer == nullptr)) {
				return *this;
			}


			mtl::strcpy(&buffer[length], str);
			this->length += mtl::strlen(str);

			return *this;
		}

		void prependPrivate(const char* str) {

			size_t newStrLen = mtl::strlen(str);
			this->size += newStrLen * sizeof(char);

			char* newBuffer = static_cast<char*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				this->size));

			if (err = (newBuffer == nullptr)) {
				return;
			}


			mtl::strcpy_s(newBuffer, str, newStrLen);
			mtl::strcpy(&newBuffer[newStrLen], buffer);

			HeapFree(GetProcessHeap(), 0, buffer);
			buffer = newBuffer;
			this->length = mtl::strlen(buffer);
		}


		bool stringComparePrivate(char* str1, char* str2, size_t str1len, size_t str2len) {

			if (str1len != str2len) {
				return false;
			}
			
			for (size_t i = 0; i < str1len; i++) {
				if (str1[i] != str2[i])
					return false;
			}

			return true;
		}

		



	public:

					/* Error handling */

		bool is_ok() {
			return !err;
		}


					/* Getters */

		char* c_str() {
			return buffer;
		}

		size_t len() {
			return this->length;
		}

		size_t size_bytes() {
			return this->size;
		}



					/* concatenation */

		void append(string& other) {
			*this = appendPrivate(other.buffer);
		}

		void append(const char* str) {
			*this = appendPrivate(str);
		}

		string& operator+(const char* str) {
			return appendPrivate(str);
		}

		string& operator+(string& other) {
			return appendPrivate(other.buffer);
		}

		string& operator+=(const char* str) {
			return appendPrivate(str);
		}

		string& operator+=(string& other) {
			return appendPrivate(other.buffer);
		}


		void appendChar(char ch) {

			this->size += 1 * sizeof(char);

			buffer = static_cast<char*>(HeapReAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				this->buffer,
				this->size
			));

			if (err = (buffer == nullptr)) {
				return;
			}

			buffer[this->length] = ch;
			buffer[this->length + 1] = '\0';
			this->length++;
		}


		void prepend(const char* str) {
			prependPrivate(str);
		}

		void prepend(string& other) {
			prependPrivate(other.buffer);
		}



		
					/* Assignment */
		
		string& operator=(const string& other) {

			if (this != &other && other.buffer) {
				
				if (buffer != nullptr) {
					HeapFree(GetProcessHeap(), 0, buffer);
				}
				
				this->length = mtl::strlen(other.buffer);
				this->size = (length + 1) * sizeof(char);

				buffer = static_cast<char*>(HeapAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					this->size));


				if (err = (buffer == nullptr)) {
					return *this;
				}

				mtl::strcpy(buffer, other.buffer);
			}

			return *this;
		}


		string& operator=(const char* str) {

			if (buffer != str) {
				
				if (buffer != nullptr) {
					HeapFree(GetProcessHeap(), 0, buffer);
				}

				this->length = mtl::strlen(str);
				this->size = (length + 1) * sizeof(char);

				buffer = static_cast<char*>(HeapAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					this->size));


				if (err = (buffer == nullptr)) {
					return *this;
				}


				mtl::strcpy(buffer, str);
			}

			return *this;
		}
		


					/* Comparisons */

		bool isTheSame(string& other) {
			return stringComparePrivate(this->buffer, other.buffer, length, other.len());
		}

		bool isTheSame(const char* str) {
			return stringComparePrivate(this->buffer, const_cast<char*>(str), length, mtl::strlen(str));
		}

		bool operator==(string& other) {
			return stringComparePrivate(this->buffer, other.buffer, length, other.len());
		}

		bool operator==(const char* str) {
			return stringComparePrivate(this->buffer, const_cast<char*>(str), length, mtl::strlen(str));
		}

		bool operator!=(string& other) {
			return !stringComparePrivate(this->buffer, other.buffer, length, other.len());
		}

		bool operator!=(const char* str) {
			return !stringComparePrivate(this->buffer, const_cast<char*>(str), length, mtl::strlen(str));
		}



					/* Find Operations And Substrings */


		//returns the index of the substring.
		int find(const char* subString) { 

			for (size_t i = 0; i < this->length; i++) {

				if (operator[](i) == subString[0]) {


					bool subCondition = true;
					size_t subStrLen = mtl::strlen(subString);

					for (size_t j = 0, k = i; j < subStrLen && k < this->length; j++, k++) {

						if (subString[j] != operator[](k)) {
							subCondition = false;
							break;
						}
					}

					if (subCondition && (this->length - i) >= subStrLen) return i;
				}
			}

			return -1;
		}

		char& operator[](size_t index) {
			return buffer[index];
		}

		char& back() {
			return buffer[this->length - 1];
		}

		char& front() {
			return buffer[0];
		}

		size_t count(char ch) {
			size_t cnt = 0;

			for (size_t i = 0; i < this->length; i++) {
				if (buffer[i] == ch) cnt++;
			}
			return cnt;
		}




					/* Insertion, Deletion */

		bool insert(const char* str, size_t index) {

			//Edge cases
			if (!this->length || index > this->length - 1)
				return false;

			if (index == this->length - 1) {
				*this = appendPrivate(str);
				return true;
			}


			size_t insertStrLen = mtl::strlen(str);
			this->size += insertStrLen * sizeof(char);
			
			char* newBuffer = static_cast<char*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				this->size));

			if (err = (newBuffer == nullptr)) {
				return false;
			}


			mtl::strcpy_s(&newBuffer[index + 1], str, insertStrLen);
			mtl::strcpy_s(&newBuffer[index + insertStrLen + 1], &buffer[index + 1], this->length - index);
			mtl::strcpy_s(newBuffer, buffer, index + 1);

			HeapFree(GetProcessHeap(), 0, buffer);
			buffer = newBuffer;
			this->length = mtl::strlen(buffer);

			return true;
		}

		
		bool erase(size_t startIndex, size_t amountOfChars) {

			if (!this->length)
				return false;

			if (startIndex > this->length - 1) {
				return false;
			}

			if (startIndex + amountOfChars > this->length) {
				return false;
			}


			this->size -= amountOfChars * sizeof(char);
			char* newBuffer = static_cast<char*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				this->size
			));

			if (err = (newBuffer == nullptr)) {
				return false;
			}


			mtl::memcpy(newBuffer, buffer, startIndex * sizeof(char)); //copy front elements
			mtl::memcpy(&newBuffer[startIndex], &buffer[startIndex + amountOfChars], ((this->length - (startIndex + amountOfChars)) + 1) * sizeof(char)); //copy back elements

			HeapFree(GetProcessHeap(), 0, buffer);
			buffer = newBuffer;
			this->length -= amountOfChars;

			return true;
		}





					/* Misc */

		void clear() {

			if (buffer != nullptr) {
				
				HeapFree(GetProcessHeap(), 0, buffer);
				
				buffer = static_cast<char*>(HeapAlloc(
					GetProcessHeap(),
					0,
					sizeof(char)
				));

				if (err = (buffer == nullptr)) {
					return;
				}
				

				this->length = 1;
				this->size = sizeof(char);
				*buffer = '\0';
			}
		}


		bool assign(const char* str, size_t numOfChars) {

			if (!this->length)
				return false;

			size_t assignStrLen = mtl::strlen(str);

			if (numOfChars > assignStrLen + 1)
				return false;
			

			if (assignStrLen > this->length) {

				this->length = assignStrLen;
				this->size = (length + 1) * sizeof(char);


				buffer = static_cast<char*>(HeapReAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					buffer,
					this->size
				));

				if (err = (buffer == nullptr)) {
					return false;
				}
			}

			mtl::strcpy_s(buffer, str, numOfChars);
			return true;
		}

		bool assign(const char* str) {

			if (!this->length)
				return false;

			size_t assignStrLen = mtl::strlen(str);

			if (assignStrLen > this->length) {

				this->length = assignStrLen;
				this->size = (length + 1) * sizeof(char);


				buffer = static_cast<char*>(HeapReAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					buffer,
					this->size
				));

				if (err = (buffer == nullptr)) {
					return false;
				}
			}

			mtl::strcpy_s(buffer, str, assignStrLen);
			return true;
		}


		
					/* Constructors */
		
		string(const char* str = "") {

			this->length = mtl::strlen(str);
			this->size = (length + 1) * sizeof(char);


			buffer = static_cast<char*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY, 
				this->size));

			if (err = (buffer == nullptr)) {
				return;
			}

			mtl::strcpy(buffer, str);
		}

		string(const string& other) {

			this->length = mtl::strlen(other.buffer);
			this->size = (length + 1) * sizeof(char);


			buffer = static_cast<char*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				this->size));

			if (err = (buffer == nullptr)) {
				return;
			}

			mtl::strcpy(buffer, other.buffer);
		}


		/* Delete */

		~string() {
			if (buffer) {
				HeapFree(GetProcessHeap(), 0, buffer);
			}
		}
};











class wstring {

private:
	wchar_t* buffer;
	size_t length;
	size_t size;
	bool err = false;


	wstring& appendPrivate(const wchar_t* str) {

		this->size += mtl::wcstrlen(str) * sizeof(wchar_t);

		buffer = static_cast<wchar_t*>(HeapReAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			this->buffer,
			this->size
		));

		if (err = (buffer == nullptr)) {
			return *this;
		}


		mtl::wcstrcpy(&buffer[length], str);
		this->length += mtl::wcstrlen(str);

		return *this;
	}


	void prependPrivate(const wchar_t* str) {

		size_t newwcstrlen = mtl::wcstrlen(str);
		this->size += newwcstrlen * sizeof(wchar_t);

		wchar_t* newBuffer = static_cast<wchar_t*>(HeapAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			this->size));

		if (err = (newBuffer == nullptr)) {
			return;
		}


		mtl::wcstrcpy_s(newBuffer, str, newwcstrlen);
		mtl::wcstrcpy(&newBuffer[newwcstrlen], buffer);

		HeapFree(GetProcessHeap(), 0, buffer);
		buffer = newBuffer;
		this->length = mtl::wcstrlen(buffer);
	}


	bool wstringComparePrivate(wchar_t* str1, wchar_t* str2, size_t str1len, size_t str2len) {

		if (str1len != str2len) {
			return false;
		}

		for (size_t i = 0; i < str1len; i++) {
			if (str1[i] != str2[i])
				return false;
		}

		return true;
	}





public:

	/* Error handling */

	bool is_ok() {
		return !err;
	}


	/* Getters */

	wchar_t* c_str() {
		return buffer;
	}

	size_t len() {
		return this->length;
	}

	size_t size_bytes() {
		return this->size;
	}



	/* concatenation */

	void append(wstring& other) {
		*this = appendPrivate(other.buffer);
	}

	void append(const wchar_t* str) {
		*this = appendPrivate(str);
	}

	wstring& operator+(const wchar_t* str) {
		return appendPrivate(str);
	}

	wstring& operator+(wstring& other) {
		return appendPrivate(other.buffer);
	}

	wstring& operator+=(const wchar_t* str) {
		return appendPrivate(str);
	}

	wstring& operator+=(wstring& other) {
		return appendPrivate(other.buffer);
	}


	void appendchar(wchar_t ch) {

		this->size += 1 * sizeof(wchar_t);

		buffer = static_cast<wchar_t*>(HeapReAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			this->buffer,
			this->size
		));

		if (err = (buffer == nullptr)) {
			return;
		}

		buffer[this->length] = ch;
		buffer[this->length + 1] = L'\0';
		this->length++;
	}


	void prepend(const wchar_t* str) {
		prependPrivate(str);
	}

	void prepend(wstring& other) {
		prependPrivate(other.buffer);
	}




	/* Assignment */

	wstring& operator=(const wstring& other) {

		if (this != &other && other.buffer) {

			if (buffer != nullptr) {
				HeapFree(GetProcessHeap(), 0, buffer);
			}

			this->length = mtl::wcstrlen(other.buffer);
			this->size = (length + 1) * sizeof(wchar_t);

			buffer = static_cast<wchar_t*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				this->size));


			if (err = (buffer == nullptr)) {
				return *this;
			}

			mtl::wcstrcpy(buffer, other.buffer);
		}

		return *this;
	}


	wstring& operator=(const wchar_t* str) {

		if (buffer != str) {

			if (buffer != nullptr) {
				HeapFree(GetProcessHeap(), 0, buffer);
			}

			this->length = mtl::wcstrlen(str);
			this->size = (length + 1) * sizeof(wchar_t);

			buffer = static_cast<wchar_t*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				this->size));


			if (err = (buffer == nullptr)) {
				return *this;
			}


			mtl::wcstrcpy(buffer, str);
		}

		return *this;
	}



	/* Comparisons */

	bool isTheSame(wstring& other) {
		return wstringComparePrivate(this->buffer, other.buffer, length, other.len());
	}

	bool isTheSame(const wchar_t* str) {
		return wstringComparePrivate(this->buffer, const_cast<wchar_t*>(str), length, mtl::wcstrlen(str));
	}

	bool operator==(wstring& other) {
		return wstringComparePrivate(this->buffer, other.buffer, length, other.len());
	}

	bool operator==(const wchar_t* str) {
		return wstringComparePrivate(this->buffer, const_cast<wchar_t*>(str), length, mtl::wcstrlen(str));
	}

	bool operator!=(wstring& other) {
		return !wstringComparePrivate(this->buffer, other.buffer, length, other.len());
	}

	bool operator!=(const wchar_t* str) {
		return !wstringComparePrivate(this->buffer, const_cast<wchar_t*>(str), length, mtl::wcstrlen(str));
	}



	/* Find Operations */

	int find(const wchar_t* substring) {

		for (size_t i = 0; i < this->length; i++) {

			if (operator[](i) == substring[0]) {


				bool subCondition = true;
				size_t substringLen = mtl::wcstrlen(substring);

				for (size_t j = 0, k = i; j < substringLen && k < this->length; j++, k++) {

					if (substring[j] != operator[](k)) {
						subCondition = false;
						break;
					}
				}

				if (subCondition && (this->length - i) >= substringLen) return i;
			}
		}
		return -1;
	}

	wchar_t& operator[](size_t index) {
		return buffer[index];
	}

	wchar_t& back() {
		return buffer[this->length - 1];
	}

	wchar_t& front() {
		return buffer[0];
	}

	size_t count(wchar_t ch) {
		size_t cnt = 0;

		for (size_t i = 0; i < this->length; i++) {
			if (buffer[i] == ch) cnt++;
		}
		return cnt;
	}




	/* Insertion, Deletion */

	bool insert(const wchar_t* str, size_t index) {

		if (!this->length || index > this->length - 1)
			return false;

		if (index == this->length - 1) {
			*this = appendPrivate(str);
			return true;
		}


		size_t insertwcstrlen = mtl::wcstrlen(str);
		this->size += insertwcstrlen * sizeof(wchar_t);

		wchar_t* newBuffer = static_cast<wchar_t*>(HeapAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			this->size));

		if (err = (newBuffer == nullptr)) {
			return false;
		}


		mtl::wcstrcpy_s(&newBuffer[index + 1], str, insertwcstrlen);
		mtl::wcstrcpy_s(&newBuffer[index + insertwcstrlen + 1], &buffer[index + 1], this->length - index);
		mtl::wcstrcpy_s(newBuffer, buffer, index + 1);

		HeapFree(GetProcessHeap(), 0, buffer);
		buffer = newBuffer;
		this->length = mtl::wcstrlen(buffer);

		return true;
	}


	bool erase(size_t startIndex, size_t amountOfwchar_ts) {

		if (!this->length || startIndex > this->length - 1) {
			return false;
		}

		if (startIndex + amountOfwchar_ts > this->length) {
			return false;
		}


		this->size -= amountOfwchar_ts * sizeof(wchar_t);
		wchar_t* newBuffer = static_cast<wchar_t*>(HeapAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			this->size
		));

		if (err = (newBuffer == nullptr)) {
			return false;
		}


		mtl::memcpy(newBuffer, buffer, startIndex * sizeof(wchar_t));
		mtl::memcpy(&newBuffer[startIndex], &buffer[startIndex + amountOfwchar_ts], ((this->length - (startIndex + amountOfwchar_ts)) + 1) * sizeof(wchar_t));

		HeapFree(GetProcessHeap(), 0, buffer);
		buffer = newBuffer;
		this->length -= amountOfwchar_ts;

		return true;
	}





	/* Misc */

	void clear() {

		if (buffer != nullptr) {

			HeapFree(GetProcessHeap(), 0, buffer);

			buffer = static_cast<wchar_t*>(HeapAlloc(
				GetProcessHeap(),
				0,
				sizeof(wchar_t)
			));

			if (err = (buffer == nullptr)) {
				return;
			}


			this->length = 1;
			this->size = sizeof(wchar_t);
			*buffer = L'\0';
		}
	}


	bool assign(const wchar_t* str, size_t numOfwchar_ts) {

		if (!this->length)
			return false;

		size_t assignwcstrlen = mtl::wcstrlen(str);

		if (numOfwchar_ts > assignwcstrlen + 1)
			return false;


		if (assignwcstrlen > this->length) {

			this->length = assignwcstrlen;
			this->size = (length + 1) * sizeof(wchar_t);


			buffer = static_cast<wchar_t*>(HeapReAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				buffer,
				this->size
			));

			if (err = (buffer == nullptr)) {
				return false;
			}
		}

		mtl::wcstrcpy_s(buffer, str, numOfwchar_ts);
		return true;
	}


	bool assign(const wchar_t* str) {

		if (!this->length)
			return false;

		size_t assignwcstrlen = mtl::wcstrlen(str);

		if (assignwcstrlen > this->length) {

			this->length = assignwcstrlen;
			this->size = (length + 1) * sizeof(wchar_t);


			buffer = static_cast<wchar_t*>(HeapReAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				buffer,
				this->size
			));

			if (err = (buffer == nullptr)) {
				return false;
			}
		}

		mtl::wcstrcpy_s(buffer, str, assignwcstrlen);
		return true;
	}



	/* Constructors */

	wstring(const wchar_t* str = L"") {

		this->length = mtl::wcstrlen(str);
		this->size = (length + 1) * sizeof(wchar_t);


		buffer = static_cast<wchar_t*>(HeapAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			this->size));

		if (err = (buffer == nullptr)) {
			return;
		}

		mtl::wcstrcpy(buffer, str);
	}

	wstring(const wstring& other) {

		this->length = mtl::wcstrlen(other.buffer);
		this->size = (length + 1) * sizeof(wchar_t);


		buffer = static_cast<wchar_t*>(HeapAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			this->size));

		if (err = (buffer == nullptr)) {
			return;
		}

		mtl::wcstrcpy(buffer, other.buffer);
	}


	/* Delete */

	~wstring() {
		if (buffer) {
			HeapFree(GetProcessHeap(), 0, buffer);
		}
	}
};



}