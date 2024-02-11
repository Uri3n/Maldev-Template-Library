#pragma once
#include <Windows.h>
#include "malutils.hpp"

#define WE_DONT_MANAGE_OUR_MEMORY_SON


namespace mtl {

	template<typename T>
	class smart_pointer{
		
		private:
			T* buffer;


			void reassignPrivate(T* replacementBuffer, bool freeOldBuffer = true) {

				if (buffer && freeOldBuffer)
					HeapFree(GetProcessHeap(), 0, buffer);

				buffer = replacementBuffer;
			}

		public:


			/* Operators */

			T* operator->() {
				return buffer;
			}

			T& operator[](size_t index){
				return buffer[index];
			}

			operator bool() {
				return (buffer != nullptr);
			}

			T& operator*() {
				return *buffer;
			}

			void operator=(T* replacementBuffer) {
				reassignPrivate(replacementBuffer, false);
			}

			void operator=(T value) { reassign(value, false); }


			/* Reassignment */

			void reassign(T value, bool freeOldBuffer = true) {
				
				T* replacement = static_cast<T*>(HeapAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					sizeof(T)
				));

				*replacement = value;
				reassignPrivate(replacement, freeOldBuffer);
			}

			void reassign(T* heapPointer, bool freeOldBuffer = true) {
				reassignPrivate(heapPointer, freeOldBuffer);
			}


			/* Misc */

			T* get() {
				return buffer;
			}


			/* Constructors */

			smart_pointer(T value) {
				
				buffer = static_cast<T*>(HeapAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					sizeof(T)
				));

				*buffer = value;
			}

			smart_pointer(T* ptr) : buffer(ptr) {}


			/* Deletion */

			void release() {
				if (buffer) {
					HeapFree(GetProcessHeap(), 0, buffer);
					buffer = nullptr;
				}
			}

			~smart_pointer() {
				if (buffer)
					HeapFree(GetProcessHeap(), 0, buffer);
			}
	};
}