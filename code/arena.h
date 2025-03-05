#pragma once
#include <cassert>

#include "common.h"

inline constexpr u32 ARENA_DEFAULT_ALIGNMENT = 2 * sizeof(void*);

struct Arena
{
    u8* m_Buf;
    size_t m_BufLen;
    size_t m_PrevOffset;
    size_t m_CurrOffset;
};

void ArenaInit(Arena* const arena, void* backingBuffer, const size_t backingBufferLength);
void ArenaCreateHeap(Arena* const arena, const size_t size);
void* ArenaAllocAligned(
    Arena* const arena,
    const size_t size,
    const size_t align = ARENA_DEFAULT_ALIGNMENT
);
#if 0
void* ArenaResizeAligned(
    Arena* arena,
    void* oldMemory,
    const size_t oldSize,
    const size_t newSize,
    const size_t align = ARENA_DEFAULT_ALIGNMENT
);
#endif
void ArenaFree(Arena* const arena);
void ArenaDestroyHeap(Arena* const arena);

// Extra features
struct TempArena
{
    Arena* m_OriginalArena;
    size_t m_PrevOffset;
    size_t m_CurrOffset;
};

TempArena TempArenaBegin(Arena* const originalArena);
void TempArenaEnd(TempArena* const tempArena);

#define PushArray(arena, type, count) (rcast<type*>(ArenaAllocAligned(arena, sizeof(type) * count)))
#define PushStruct(value, arrPointer, counter) (arrPointer[counter++] = value)
