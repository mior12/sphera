#include <stdexcept>
#include <string>
#include <map>
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
    Pointer() {}
    void* get() const {return m?*m:nullptr;}

private:
    Pointer(const std::shared_ptr<char*>& p) :
        m(p)
        {}

    std::shared_ptr<char*> m;
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
        
        struct comp {
            bool operator() (const std::shared_ptr<char*>& l,const std::shared_ptr<char*>& r) {
                return *l < *r;
            }
        };

        char* find(size_t N);
        char* begin;
        char* curr;
        std::map<std::shared_ptr<char*>, std::size_t, comp> pointers;
        size_t capacity;
        size_t size;
};

