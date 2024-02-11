#pragma once
#include <Windows.h>
#include "malutils.hpp"


namespace mtl{

template<typename T>
class singly_list {

	protected:
		size_t length;

	private:
		mtl::slNode<T> head;
		bool err = false;
		bool safeToFree = true; //used to prevent use-after-free exceptions for object cloning.


		T& findPrivate(size_t index) {

			mtl::slNode<T>* itr = &head;
			size_t count = 0;
			
			while (count != index) {
				itr = itr->next;
				count++;
			}

			return itr->value;
		}


		void appendPrivate(singly_list& other) {

			if (other.length < 1)
				return;

			mtl::slNode<T>* localItr = &head;
			mtl::slNode<T>* otherItr = &other.head;
			
			if (this->length < 1) {
				this->head.value = (T)(otherItr->value);
				otherItr = otherItr->next;
				length++;
			}

			while (localItr->next != nullptr) {
				localItr = localItr->next;
			}



			while (otherItr != nullptr){
				
				localItr->next = static_cast<mtl::slNode<T>*>(HeapAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					sizeof(mtl::slNode<T>))
				);

				if (err = (localItr->next == nullptr)) {
					return;
				}


				mtl::memcpy(localItr->next, otherItr, sizeof(mtl::slNode<T>));
				
				otherItr = otherItr->next;
				localItr = localItr->next;
				localItr->next = nullptr;

				this->length++;
			} 
		}


		bool compareListPrivate(singly_list<T>& other) {

			if (other.length != this->length) {
				return false;
			}

			for (size_t i = 0; i < this->length; i++) {
				if (other[i] != find(i)) {
					return false;
				}
			}

			return true;
		}


	public:


		/* Assignment */

		void operator=(singly_list<T>& other) {
			
			size_t i = 0;
			for (i; i < other.length; i++) {
				
				if (i < this->length) {
					this->change(i, other[i]);
				}

				else {
					this->push(other[i]);
				}
			}

			if(other.length < this->length)
				this->erase(i, this->length - i);
		}


		bool change(size_t index, T value) {

			if (index >= this->length) {
				return false;
			}

			mtl::slNode<T>* itr = &head;
			size_t cnt = 0;

			while (cnt != index) {
				cnt++;
				itr = itr->next;
			}

			itr->value = value;
			return true;
		}



		/* Insert Operations */

		bool push(T val) {

			mtl::slNode<T>* pNewNode = nullptr;
			mtl::slNode<T>* itr = &head;


			auto chkState = [&](bool state) {
				if (state)
					this->length++;
				else if (!state && pNewNode != nullptr)
					HeapFree(GetProcessHeap(), 0, pNewNode);

				return state;
			};



			if (this->length == 0) {
				head.value = val;
				return chkState(true);
			}


			pNewNode = static_cast<mtl::slNode<T>*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				sizeof(mtl::slNode<T>)
			));

			if (this->err = (pNewNode == nullptr)) {
				return chkState(false);
			}



			while (itr->next) {
				itr = itr->next;
			}

			pNewNode->value = val;
			pNewNode->next = nullptr;
			itr->next = pNewNode;

			return chkState(true);
		}


		//inserts directly after the index given
		bool insert(size_t index, T value) {

			if (this->length == 0 || index > (this->length - 1)) {
				return false;
			}

			if (index == this->length - 1) {
				return push(value);
			}


			size_t cnt = 0;
			mtl::slNode<T>* itr = &head;
			
			while (cnt != index) {
				itr = itr->next;
				cnt++;
			}
			

			mtl::slNode<T>* newNode = static_cast<mtl::slNode<T>*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				sizeof(mtl::slNode<T>)
			));

			if (err = (newNode == nullptr)) {
				return false;
			}

		
			newNode->value = value; //insert in between iterator and old next
			newNode->next = itr->next;
			itr->next = newNode;
			this->length++;

			return true;
		}


		void append(singly_list& other) {
			appendPrivate(other);
		}


		void operator+=(singly_list& other) {
			appendPrivate(other);
		}


		template<typename ...Args>
		void append(Args... args) {

			singly_list<T> listAppend(args...);
			appendPrivate(listAppend);
		}



		/* Find Operations */

		T& operator[](size_t index) {

			if (index > this->length - 1)
				return findPrivate(this->length - 1);
			
			return findPrivate(index);
		}


		T find(size_t index) {

			if (index > this->length - 1)
				return findPrivate(this->length - 1);

			return findPrivate(index);
		}


		// returns index if element exists, otherwise -1
		int exists(T value) {

			mtl::slNode<T>* itr = &head;
			size_t index = 0;

			while (itr->value != value && itr->next != nullptr) {
				itr = itr->next;
				index++;
			}

			return(itr->value != value ? -1 : index);
		}


		mtl::slNode<T>* getIndicePtr(size_t index) {

			if (index >= this->length)
				return nullptr;

			size_t cnt = 0;
			mtl::slNode<T>* itr = &head;
			while (cnt != index) {
				itr = itr->next;
				cnt++;
			}

			return itr;
		}


		T front() {
			return head.value;
		}


		T back() {
			if (length < 1)
				return 0x00;

			return operator[](length - 1);
		}


		mtl::slNode<T>* backPtr() {
			if (length < 1)
				return &head;

			return getIndicePtr(this->length - 1);
		}


		mtl::slNode<T>* headPtr() {
			return &head;
		}



		/* Misc */

		size_t len() {
			return this->length;
		}


		bool is_ok() {
			return !err;
		}


		bool reverse() {

			//Local
			mtl::slNode<T>* itr = &head;
			mtl::slNode<T>** stack = nullptr;
			
			singly_list<T> newList = { };
			size_t cnt = 0;

			

			//Cleanup
			auto chkState = [&](bool state) {
				if (stack)
					HeapFree(GetProcessHeap(), 0, stack);

				if (state) {
					*this = newList;
					newList.safeToFree = false;
				}
				
				return state;
			};



			if (this->length <= 1) {
				return chkState(false);
			}
			
			stack = static_cast<mtl::slNode<T>**>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				sizeof(void*) * this->length
			));

			if (err = (stack == nullptr)) {
				return chkState(false);
			}



			while (itr->next != nullptr) { //reach the end of the current list
				stack[cnt] = itr;
				itr = itr->next;
				cnt++;
			}

			while (cnt) {
				newList.push(itr->value);
				cnt--;

				itr = stack[cnt];
			}

			newList.push(itr->value);
			return chkState(true);
		}


		bool sort() {

			T* arr = nullptr;
			singly_list<T> sorted;

			if (this->length <= 1)
				return false;
			


			arr = static_cast<T*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				sizeof(T) * this->length
			));

			if (err = (arr == nullptr))
				return false;



			for (size_t i = 0; i < this->length; i++) {
				arr[i] = this->find(i);
			}

			mtl::sort<T>(arr, this->length);

			for (size_t i = 0; i < this->length; i++)
				sorted.push(arr[i]);

			*this = sorted;
			sorted.safeToFree = false;

			HeapFree(GetProcessHeap(), 0, arr);
			return true;
		}


		void clear() {
			if (length <= 1) {
				head.value = (T)0x00;
				length = 0;
				return;
			}

			erase(1, this->length - 1);
			head.value = (T)0x00;
			length = 0;
		}



		/* Comparison */

		bool operator==(singly_list<T>& other) {
			return compareListPrivate(other);
		}

		bool isTheSame(singly_list<T>& other) {
			return compareListPrivate(other);
		}



		/* Constructors */

		template<typename... Args>
		singly_list(Args... args) {

			T initList[] = { args...};
			this->length = 0;

			bool firstElement = true;
			for (T& i : initList) {
				
				if (firstElement) {

					head.value = (T)i;
					head.next = nullptr;
					this->length++;
					firstElement = false;
				}

				else {
					push(i);
				}
			}
		}

		singly_list() : length(0) {}



		/* delete */

		//NOTE: head cannot be erased.
		bool erase(size_t index, size_t amntToErase) {

			if (this->length == 0 || index >= this->length || index < 1 || this->length - index < amntToErase) {
				return false;
			}

			mtl::slNode<T>* itr = getIndicePtr(index);
			mtl::slNode<T>* setNullLink = getIndicePtr(index - 1);
			this->length -= amntToErase;

			while (amntToErase--) {
				void* temp = itr;
				itr = itr->next;
				HeapFree(GetProcessHeap(), 0, temp);
			}

			setNullLink->next = itr;
			return true;
		}


		T pop() {

			if (this->length < 1)
				return;

			if (this->length == 1) {
				head.value = (T)0x00;
				this->length = 0;
				return;
			}

			mtl::slNode<T>* node = getIndicePtr(length - 2);
			
			T toRetrn = node->next->value;
			HeapFree(GetProcessHeap(), 0, node->next);
			
			node->next = nullptr;
			this->length--;
			return toRetrn;
		}
		

		~singly_list() {

			if (this->length <= 1 || !safeToFree) {
				return;
			}
			
			if (head.next->next == nullptr) { //prevent free attempts on non-heap memory
				HeapFree(GetProcessHeap(), 0, head.next);
				head.next = nullptr;
				return;
			}

			mtl::slNode<T>* itr = head.next;

			size_t i = 1;
			while (itr->next != nullptr) {

				void* temp = itr;
				itr = itr->next;

				HeapFree(GetProcessHeap(), 0, temp);
			}

			if (itr)
				HeapFree(GetProcessHeap(), 0, itr);
		}
};


}