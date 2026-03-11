#include "arena.h"
#include <new>

Arena::Chunk* Arena::_make_chunk(size_t size) {
    Chunk* chunk = static_cast<Chunk*>(malloc(sizeof(Chunk)));
    if (!chunk) throw std::bad_alloc();
    chunk->buffer = static_cast<char*>(malloc(size));
    if (!chunk->buffer) { free(chunk); throw std::bad_alloc(); }
    chunk->capacity = size;
    chunk->offset = 0;
    chunk->next = nullptr;
    return chunk;
}

Arena::Arena(std::size_t chunk_size)
    : _chunk_size(chunk_size) {
    _first = _make_chunk(chunk_size);
    _current = _first;
}

char* Arena::allocate(std::size_t size) {
    if (_current->capacity - _current->offset < size) {
        size_t new_size = size > _chunk_size ? size : _chunk_size;
        Chunk* next = _make_chunk(new_size);
        _current->next = next;
        _current = next;
    }
    char* ptr = _current->buffer + _current->offset;
    _current->offset += size;
    return ptr;
}

void Arena::reset() {
    Chunk* chunk = _first;
    while (chunk) {
        chunk->offset = 0;
        chunk = chunk->next;
    }
    _current = _first;
}

Arena::~Arena() {
    Chunk* chunk = _first;
    while (chunk) {
        Chunk* next = chunk->next;
        free(chunk->buffer);
        free(chunk);
        chunk = next;
    }
}
