#include "allocator.h"
#include <cstring>
#include <vector>

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
        rm = f;
    }
    else 
        curr += N;
    std::shared_ptr<char*> sp = std::make_shared<char*>(rm); 
    pointers.insert(std::make_pair(sp, N));
    size += N;
    return sp; 
}

void Allocator::free(Pointer& p) 
{
    auto it = p.m ? pointers.find(p.m) : pointers.end(); 
    if(it == pointers.end())
         throw AllocError(AllocErrorType::InvalidFree, "Can't free memory");
    size -= it->second;
    pointers.erase(it);
    p = Pointer(); 
}

void Allocator::realloc(Pointer& p, size_t N) {
    auto it = p.m ? pointers.find(p.m) : pointers.end(); 
    if(it == pointers.end()) 
        p = alloc(N); 
    else if(it->second > N) {
        size -= it->second - N;
        it->second = N;
    }
    else if(it->second < N) {
        int diff = N - it->second;
        if(size + diff > capacity)
            throw AllocError(AllocErrorType::NoMemory, "Can't alloc memory!");
        char* ptr = *it->first;
        if(ptr + it->second == curr)  
            curr = ptr;
        free(p);
        p = alloc(N);
    }
}

void Allocator::defrag() {
    char* temp = begin;
    std::vector<std::pair<std::shared_ptr<char*>, size_t>> old(pointers.begin(), pointers.end());
    pointers.clear();
    for(auto it = old.begin(); it != old.end(); ++it) {
        if(temp < *it->first) {
            memcpy(temp, *it->first, it->second);
            *it->first = temp;
        }
        pointers.insert(*it);
        temp += it->second;   
    }
    curr = temp;
}

char* Allocator::find(size_t N) {
    char* temp = begin;
    for(auto it = pointers.begin(); it != pointers.end(); ++it) {
        if(*it->first - temp >= N)
            return temp;
        temp = *it->first + it->second;
    }
    return nullptr;
}
