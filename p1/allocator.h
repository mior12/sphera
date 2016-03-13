#include <stdexcept>
#include <string>
#include <set>
#include <memory>

enum class AllocErrorType {
    InvalidFree,
    NoMemory
};

class AllocError: std::runtime_error {
private:
    AllocErrorType type;

public:
    AllocError(AllocErrorType _type, std::string message):
            runtime_error(message),
            type(_type)
    {}

    AllocErrorType getType() const { return type; }
};

class Allocator;

class Pointer {
public:
    friend class Allocator;
    Pointer() : 
        m(new char*(nullptr)),
        N(0) 
        {}
    Pointer(char* _m, size_t _N) : 
        m(new char*(_m)),
        N(_N)
        {}

    size_t get_size() const {return N;}
    void* get() const {return *m;}
private:
    void reset_memory() { *m = nullptr; }
    void set_memory(char* p) const { *m = p; }
    void set_size(size_t n) { N = n; } 
    mutable std::shared_ptr<char*> m;
    size_t N; 
};

template<typename T>
struct comp {
    bool operator() (const T& l,const T& r) {
        return l.get() < r.get();
    }
};

class Allocator {
public:
    Allocator(void *base, size_t _capacity);
    Pointer alloc(size_t N);
    void realloc(Pointer &p, size_t N);
    void free(Pointer &p);
    void defrag();

    std::string dump() { return ""; }
    private:
    char* find(size_t N);
    char* begin;
    char* curr;
    std::set<Pointer, comp<Pointer>> pointers;
    size_t capacity;
    size_t size;
};

