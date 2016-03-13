#include "allocator.h"
#include <string.h>

Allocator::Allocator(void* base, size_t _capacity):
    begin((char*)base),
    curr(begin),
    capacity(_capacity),
    size(0)
{}


Pointer Allocator::alloc(size_t N) {
    if(N == 0)
        return Pointer();
    if(size + N > capacity)
        throw AllocError(AllocErrorType::NoMemory, "Can't alloc memory!");
    char* rm = curr;
    if(begin + capacity < curr + N) {
        char* f = find(N);
        if(f == nullptr) 
            throw AllocError(AllocErrorType::NoMemory, "Can't alloc memory!");
        else 
            rm = f;
    }
    else 
        curr += N;

    Pointer p(rm, N);
    pointers.insert(p);
    size += N;
    return p; 
}

void Allocator::free(Pointer& p) 
{
    if(p.get() == nullptr)
        throw AllocError(AllocErrorType::InvalidFree, "Can't free memory");
    size_t n = p.get_size();
    char* ptr = (char*)p.get();
    if(ptr + n == curr) 
         curr = ptr;
    pointers.erase(p);
    size -= n;
    p.set_size(0);
    p.reset_memory(); 
}

void Allocator::realloc(Pointer& p, size_t N) {
    size_t n = p.get_size();
    if(p.get() == nullptr) 
        p = alloc(N); 
    else if(n > N) {
        char* ptr = (char*)p.get();
        if(ptr + n == curr)
            curr = ptr + N;
        p.set_size(N);
        size -= n - N;
    }
    else if(n < N) {
        char* ptr = (char*)p.get();
        int diff = N - n;
        if(size + diff > capacity)
            throw AllocError(AllocErrorType::NoMemory, "Can't alloc memory!");
        if(ptr + n == curr) {
            curr = ptr + N;
            p.set_size(N);
        }
        else {
            free(p);
            p = alloc(N);
        }
    }
}

void Allocator::defrag() {
    char* temp = begin;
    for(auto it = pointers.begin(); it != pointers.end(); ++it) {
        if(temp < it->get()) {
            memcpy(temp, it->get(), it->get_size());
            it->set_memory(temp);  
        }
        temp += it->get_size();   
    }
    curr = temp;
}

char* Allocator::find(size_t N) {
    char* temp = begin;
    for(auto it = pointers.begin(); it != pointers.end(); ++it) {
        if((char*)it->get() - temp >= N)
            return temp;
        temp = (char*)it->get() + it->get_size();
    }
    return nullptr;
}
