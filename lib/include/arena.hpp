#pragma once
#include <cstdlib>
#include <cstddef>

class Arena {

    struct Chunk {
        char* buffer;
        size_t capacity;
        size_t offset;
        Chunk* next;
    };

    Chunk* _make_chunk(size_t size);

    Chunk* _first;
    Chunk* _current;
    size_t _chunk_size;

public:
    explicit Arena(std::size_t chunk_size);

    // Allocate `size` raw bytes, returns a stable pointer
    char* allocate(std::size_t size);

    // Reset all chunks to reuse memory from the start
    void reset();

    // Non-copyable, non-movable
    Arena(const Arena&) = delete;
    Arena(Arena&&) = delete;
    ~Arena();
};

template<class T>
class ArenaAllocator {
public:
    using value_type = T;

    ArenaAllocator(Arena* arena) noexcept : _arena(arena) {}

    // Rebind constructor - lets std::vector create allocators for other types internally
    template<class U>
    ArenaAllocator(const ArenaAllocator<U>& other) noexcept : _arena(other._arena) {}

    T* allocate(std::size_t n) {
        return reinterpret_cast<T*>(_arena->allocate(sizeof(T) * n));
    }

    void deallocate(T*, std::size_t) noexcept {}

    template<class U>
    bool operator==(const ArenaAllocator<U>& other) const noexcept { return _arena == other._arena; }

    template<class U>
    bool operator!=(const ArenaAllocator<U>& other) const noexcept { return _arena != other._arena; }

private:
    Arena* _arena;

    template<class U> friend class ArenaAllocator;
};
