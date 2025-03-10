#include "arena.h"

#include <cstdlib>
#include <cstring>

#include "profile.h"

static bool IsPowerOfTwo(const uintptr_t val)
{
    return (val & (val - 1)) == 0;
}

static uintptr_t AlignForward(const uintptr_t ptr, const size_t align)
{
    assert(IsPowerOfTwo(align));

    uintptr_t p = ptr;
    const uintptr_t modulo = p & (align - 1); // Same as (p % align) but faster as align is a power of two.

    if (modulo != 0)
    {
        // If 'p' address is not aligned, push the address to the next value which is aligned.
        p += (align - modulo);
    }

    return p;
}

void ArenaInit(Arena* const arena, void* backingBuffer, const size_t backingBufferLength)
{
    arena->m_Buf = rcast<u8*>(backingBuffer);
    arena->m_BufLen = backingBufferLength;
    arena->m_PrevOffset = 0;
    arena->m_CurrOffset = 0;
}

void ArenaCreateHeap(Arena* const arena, const size_t size)
{
    u8* buf = rcast<u8*>(std::malloc(size));
    assert(buf && "ERROR: Failed to allocate memory.");

    PROFILE_MEMORY_ALLOC(buf, size);

    ArenaInit(arena, buf, size);
}

void* ArenaAllocAligned(
    Arena* const arena,
    const size_t size,
    const size_t align/* = DEFAULT_ALIGNMENT*/
)
{
    const uintptr_t base = rcast<uintptr_t>(arena->m_Buf);
    const uintptr_t currPtr = base + arena->m_CurrOffset;
    const uintptr_t alignedPtr = AlignForward(currPtr, align);
    const size_t offset = scast<size_t>(alignedPtr - base); // Get the distance from base.

    // Check to see if the backing memory has space left.
    if (offset + size <= arena->m_BufLen)
    {
        void* const ptr = arena->m_Buf + offset;
        arena->m_PrevOffset = offset;
        arena->m_CurrOffset = offset + size;

        std::memset(ptr, 0, size); // Zero the memory by default.
        return ptr;
    }

    assert(false && "ERROR: Arena out of memory.");
    return nullptr;
}

#if 0
void* ArenaResizeAligned(
    Arena* arena,
    void* oldMemory,
    const size_t oldSize,
    const size_t newSize,
    const size_t align/* = DEFAULT_ALIGNMENT*/
)
{
    assert(IsPowerOfTwo(align));

    const u8* const oldMem = rcast<u8*>(oldMemory);

    if (oldMem == nullptr || oldSize == 0)
    {
        return ArenaAllocAligned(arena, newSize, align);
    }

    // Check if old_mem belongs to the arena's buffer.
    const bool isWithinArena = (arena->m_Buf <= oldMem && oldMem <= arena->m_Buf + arena->m_BufLen);

    if (isWithinArena)
    {
        // Check if old_mem is the most recent allocation.
        const bool isLastAllocation = (oldMem == arena->m_Buf + arena->m_PrevOffset);
        if (isLastAllocation)
        {
            // Cheap resize, adjust the arena's offset directly.
            arena->m_CurrOffset = arena->m_PrevOffset + newSize;

            // If expanding, zero out the new part.
            if (newSize > oldSize)
            {
                std::memset(arena->m_Buf + arena->m_CurrOffset, 0, newSize - oldSize);
            }

            return oldMemory; // Return same memory, no copying needed.
        }
        else
        {
            // Otherwise, allocate new memory and copy existing data.
            void* const new_memory = ArenaAllocAligned(arena, newSize, align);
            const size_t copy_size = (oldSize < newSize) ? oldSize : newSize;
            std::memmove(new_memory, oldMemory, copy_size); // Copy data to new allocation.
            return new_memory;
        }
    }

    // If old_memory is outside the arena buffer, something went wrong.
    assert(false && "ERROR: Memory is out of bounds of the buffer in this arena.");
    return nullptr;
}
#endif

void ArenaFree(Arena* const arena)
{
    arena->m_CurrOffset = 0;
    arena->m_PrevOffset = 0;
}

void ArenaDestroyHeap(Arena* const arena)
{
    std::free(arena->m_Buf);
    arena->m_Buf = nullptr;
    arena->m_BufLen = 0;
    arena->m_PrevOffset = 0;
    arena->m_CurrOffset = 0;
    PROFILE_MEMORY_FREE(arena->m_Buf);
}

TempArena TempArenaBegin(Arena* const originalArena)
{
    TempArena result = {};
    result.m_OriginalArena = originalArena;
    result.m_PrevOffset = originalArena->m_PrevOffset;
    result.m_CurrOffset = originalArena->m_CurrOffset;
    return result;
}

void TempArenaEnd(TempArena* const tempArena)
{
    tempArena->m_OriginalArena->m_PrevOffset = tempArena->m_PrevOffset;
    tempArena->m_OriginalArena->m_CurrOffset = tempArena->m_CurrOffset;
}
