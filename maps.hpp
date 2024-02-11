#pragma once
#include <Windows.h>
#include "malutils.hpp"


namespace mtl{

template<typename K, typename V>
class ordered_map {

	private:
		size_t keyCount = 0;
		mtl::omNode<K, V> root;
		
		mtl::omNode<K, V>** toFree = nullptr;
		size_t toFreeLength = 0;
		bool err = false;



		bool addNodeToFreeList(mtl::omNode<K, V>* node) {

			toFreeLength++;

			if (toFree == nullptr) {

				toFree = static_cast<mtl::omNode<K, V>**>(HeapAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					sizeof(mtl::omNode<K, V>*)
				));
			}

			else {

				toFree = static_cast<mtl::omNode<K, V>**>(HeapReAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					this->toFree,
					toFreeLength * sizeof(mtl::omNode<K, V>*)
				));

			}

			if (err = (toFree == nullptr))
				return false;

			toFree[toFreeLength - 1] = node;
			return true;
		}
		

		bool createNodePrivate(K key, _In_opt_ V value) {

			mtl::omNode<K, V>* itr = &root;
			mtl::omNode<K, V>* newNode = nullptr;



			auto chkState = [&](bool state) {
				
				if (state) {
					newNode->value = value;
					newNode->key = key;
					newNode->right = nullptr;
					newNode->left = nullptr;
					state = addNodeToFreeList(newNode);
					keyCount++;
				}
				
				else {
					HeapFree(GetProcessHeap(), 0, newNode);
				}

				return state;
			};



			newNode = static_cast<mtl::omNode<K, V>*>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				sizeof(mtl::omNode<K, V>)
			));

			if (!newNode)
				return false;

			

			while (itr != nullptr) {

				if (key == itr->key)
					return chkState(false);

				if (key > itr->key && itr->right == nullptr) {
					itr->right = newNode;
					return chkState(true);
				}

				if (key < itr->key && itr->left == nullptr) {
					itr->left = newNode;
					return chkState(true);
				}

				itr = (itr->key < key ? itr->right : itr->left);
			}


			return chkState(false);
		}

		
		V& findPrivate(K key, bool& exists){

			mtl::omNode<K, V>* itr = &root;
			while (itr != nullptr) {

				if (itr->key == key) {
					exists = true;
					return itr->value;
				}

				itr = (key > itr->key ? itr->right : itr->left);
			}

			exists = false;
			return root.value;
		}



	public:


		/* Find Operations */

		V& find(K key) {

			bool exists = false;
			return findPrivate(key, exists);
		}


		bool key_exists(K key) {

			bool exists = false;
			findPrivate(key, exists);
			return exists;
		}


		V& operator[](K key) {
			
			bool exists = false;
			V& value = findPrivate(key, exists);

			if (!exists) {
				insert(key);
				return findPrivate(key, exists);
			}

			return value;
		}


		bool update(K key, V newValue) {

			if (!keyCount)
				return false;

			mtl::omNode<K, V>* itr = &root;

			while (itr != nullptr) {

				if (itr->key == key) {
					itr->value = newValue;
					return true;
				}

				itr = (itr->key < key ? itr->right : itr->left);
			}

			return false;
		}


		/* Insertion */

		bool insert(K key, V value) {
			
			if(!keyCount){
				root.key = key;
				root.value = value;
				keyCount++;
				return true;
			}

			return createNodePrivate(key, value);
		}

		bool insert(K key) {
			
			if (!keyCount) {
				root.key = key;
				root.value = (V)0x00;
				keyCount++;
				return true;
			}

			return createNodePrivate(key, (V)0x00);
		}



		/* Misc */

		void map(mtl::mapFuncPtr<V> funcPtr) {

			if (!keyCount || !funcPtr)
				return;


			mtl::omNode<K, V>* itr = &root;
			mtl::omNode<K, V>** localStack = nullptr;

			localStack = static_cast<mtl::omNode<K, V>**>(HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				(keyCount + 5) * sizeof(void*)
			));

			if (localStack == nullptr)
				return;



			void* stackBase = localStack;
			while (itr != nullptr || localStack != stackBase) {

				while (itr != nullptr) {
					localStack++;
					*localStack = itr;
					itr = itr->left;
				}

				itr = *localStack;
				funcPtr(itr->key);

				itr = itr->right;
				localStack--;
			}

			HeapFree(GetProcessHeap(), 0, stackBase);
		}


		void clear() {

			root.key = (K)0x00;
			root.value = (V)0x00;
			root.right = nullptr;
			root.left = nullptr;

			if (!toFree || !toFreeLength)
				return;


			for (size_t i = 0; i < toFreeLength; i++)
				HeapFree(GetProcessHeap(), 0, toFree[i]);

			HeapFree(GetProcessHeap(), 0, toFree);

			toFree = nullptr;
			toFreeLength = 0;
			keyCount = 0;
		}



		/* Getters */

		size_t length() {
			return keyCount;
		}

		bool is_ok() {
			return !err;
		}

		mtl::omNode<K, V>* rootPtr() {
			return &root;
		}


		/* Constructors */

		ordered_map(K rootKey, V rootValue = (V)0x00) {
			root.key = rootKey;
			root.value = rootValue;
			keyCount++;
		}

		ordered_map() {
			root.key = (K)0x00;
			root.value = (V)0x00;
		}

		~ordered_map() {
			
			if (!toFree || !toFreeLength)
				return;

			for (size_t i = 0; i < toFreeLength; i++)
				HeapFree(GetProcessHeap(), 0, toFree[i]);
		
			HeapFree(GetProcessHeap(), 0, toFree);
		}
};


}