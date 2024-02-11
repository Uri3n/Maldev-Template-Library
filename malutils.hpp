#pragma once
#include <Windows.h>


// NOTE: will not compile without setting memset as intrinsic. Do not change this
#pragma intrinsic(memset)
#pragma function(memset)

extern void* __cdecl memset(void*, int, size_t);

void* __cdecl memset(void* Destination, int Value, size_t Size) {

    unsigned char* p = (unsigned char*)Destination;
    while (Size > 0) {
        *p = (unsigned char)Value;
        p++;
        Size--;
    }
    return Destination;
}



namespace mtl{

    void* memcpy(void* Destination, void* Source, size_t Size)
    {
        for (volatile int i = 0; i < Size; i++) {
            ((byte*)Destination)[i] = ((byte*)Source)[i];
        }
        return Destination;
    }


    void* __cdecl memset(void* Destination, int Value, size_t Size) {

        unsigned char* p = (unsigned char*)Destination;
        while (Size > 0) {
            *p = (unsigned char)Value;
            p++;
            Size--;
        }
        return Destination;
    }


    size_t strlen(const char* string) {

        size_t counter = 0;
        while (*string != '\0') {

            ++counter;
            ++string;
        }

        return counter;
    }


    size_t wcstrlen(const wchar_t* string) {

        size_t counter = 0;
        while (*string != L'\0') {

            ++counter;
            ++string;
        }

        return counter;
    }


    char* strcpy(char* destination, const char* source)
    {
        char* p = destination;

        while ((*p++ = *source++) != 0);

        return destination;
    }

    wchar_t* wcstrcpy(wchar_t* destination, const wchar_t* source)
    {
        wchar_t* p = destination;

        while ((*p++ = *source++) != 0);

        return destination;
    }

    char* strcpy_s(char* destination, const char* source, size_t size)
    {
        char* pChar = destination;

        while (size-- && (*destination++ = *source++) != '\0');

        return pChar;
    }

    wchar_t* wcstrcpy_s(wchar_t* destination, const wchar_t* source, size_t size)
    {
        wchar_t* pChar = destination;

        while (size-- && (*destination++ = *source++) != '\0');

        return pChar;
    }




    template<typename T>
    struct slNode {
        T value;
        slNode* next = nullptr;
    };


    template<typename K, typename V>
    struct omNode {
        
        K key;
        V value;

        omNode<K, V>* left = nullptr;
        omNode<K, V>* right = nullptr;
    };



    template<typename V>
    using mapFuncPtr = void (*)(V&);

    template<typename T>
    using arrayFuncPtr = void(*)(T&);


    template<typename T>
    void sort(T* array, size_t arrayLen) {

        if (array == nullptr || arrayLen < 2) {
            return;
        }

        size_t right = arrayLen - 1;
        T temp;

        for (size_t i = 1; i <= right; i++) {

            for (size_t j = i; j >= 1; j--) {

                if (array[j] < array[j - 1]) {

                    temp = array[j - 1];
                    array[j - 1] = array[j];
                    array[j] = temp;

                }
            }
        }

    }





}

