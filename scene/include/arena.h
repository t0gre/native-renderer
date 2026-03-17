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