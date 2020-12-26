#include <common.h>
#include <filemgr.h>
#include <mem.h>
#include <memory.h>
#include <os.h>
#include <system.h>

#pragma inline_max_auto_size(17) // was needed for smartAlloc

#define IS_FIRST_SMART_ALLOC(allocation) (allocation->prev == NULL)
#define IS_LAST_SMART_ALLOC(allocation) (allocation->next == NULL)

static MemWork memWork;
static MemWork * wp = &memWork;
static SmartWork smartWork;
static SmartWork * swp = &smartWork;
static bool memInitFlag = false;
s32 g_bFirstSmartAlloc;

static HeapSize size_table[HEAP_COUNT] = {
    // MEM1
    { // 0
        .type = HEAPSIZE_ABSOLUTE_KB,
        .size = 0x2400
    },
    { // 1
        .type = HEAPSIZE_ABSOLUTE_KB,
        .size = 0x1800
    },
    { // 2
        .type = HEAPSIZE_PERCENT_REMAINING,
        .size = 100
    },

    // MEM2
    { // 3
        .type = HEAPSIZE_ABSOLUTE_KB,
        .size = 0x100
    },
    { // 4
        .type = HEAPSIZE_ABSOLUTE_KB,
        .size = 0x100
    },
    { // 5
        .type = HEAPSIZE_ABSOLUTE_KB,
        .size = 0x80
    },
    { // 6
        .type = HEAPSIZE_ABSOLUTE_KB,
        .size = 0x4400
    },
    { // 7
        .type = HEAPSIZE_PERCENT_REMAINING,
        .size = 100
    },
    { // 8
        .type = HEAPSIZE_ABSOLUTE_KB,
        .size = 1
    }
};

void memInit() {
    s32 i; // register usage closer to matching
    u32 max;
    u8 * min;

    // Instruction order doesn't match when geting size_table pointer at the start

    min = OSGetMem1ArenaLo();
    max = (u32) OSGetMem1ArenaHi();

    for (i = 0; i < MEM1_HEAP_COUNT; i++) {
        if (size_table[i].type == HEAPSIZE_ABSOLUTE_KB) {
            u32 size = (u32) size_table[i].size * 1024;
            wp->heapStart[i] = min;
            wp->heapEnd[i] = min + size;
            // "Error: Overheap of heap from arena [%d] \ n"
            assertf((u32)wp->heapEnd[i] <= max, "ERROR: アリーナからのヒープの取得オーバーです。[%d]\n", i);
            min += size;
        }
    }

    u32 space = (u32) max - (u32) min;
    for (i = 0; i < MEM1_HEAP_COUNT; i++) {
        if (size_table[i].type == HEAPSIZE_PERCENT_REMAINING) {
            u32 size = (u32) (((u64) space * size_table[i].size) / 100);
            // "ERROR: Excessive heap acquisition from arena\n"
            assert(size >= 32, "ERROR: アリーナからのヒープの取得オーバーです。\n");
            size -= size & 0x1f;
            wp->heapStart[i] = min;
            wp->heapEnd[i] = min + size;
            // "Error: Overheap of heap from arena [%d] \ n"
            assertf((u32)wp->heapEnd[i] <= max, "ERROR: アリーナからのヒープの取得オーバーです。[%d]\n", i);
            min += size;
        }
    }

    for (i = 0; i < MEM1_HEAP_COUNT; i++) {
        wp->heapHandle[i] = MEMCreateExpHeapEx(wp->heapStart[i], (u32)wp->heapEnd[i] - (u32)wp->heapStart[i], 0);
    }

    OSSetMem1ArenaLo((void *) max);
    // Register usage for min & max wrong way around from here
    min = OSGetMem2ArenaLo();
    max = (u32) OSGetMem2ArenaHi();

    for (i = MEM1_HEAP_COUNT; i < HEAP_COUNT; i++) {
        if (size_table[i].type == HEAPSIZE_ABSOLUTE_KB) {
            u32 size = (u32) size_table[i].size * 1024;
            wp->heapStart[i] = min;
            wp->heapEnd[i] = min + size;
            // "Error: Overheap of heap from arena [%d] \ n"
            assertf((u32)wp->heapEnd[i] <= max, "ERROR: アリーナからのヒープの取得オーバーです。[%d]\n", i);
            min += size;
        }
    }

    space = (u32) max - (u32) min;
    for (i = MEM1_HEAP_COUNT; i < HEAP_COUNT; i++) {
        if (size_table[i].type == HEAPSIZE_PERCENT_REMAINING) {
            u32 size = (u32) (((u64) space * size_table[i].size) / 100);
            // "ERROR: Excessive heap acquisition from arena\n"
            assert(size >= 32, "ERROR: アリーナからのヒープの取得オーバーです。\n");
            size -= size & 0x1f;
            wp->heapStart[i] = min;
            wp->heapEnd[i] = min + size;
            // "Error: Overheap of heap from arena [%d] \ n"
            assertf((u32)wp->heapEnd[i] <= max, "ERROR: アリーナからのヒープの取得オーバーです。[%d]\n", i);
            min += size;
        }
    }

    for (i = MEM1_HEAP_COUNT; i < HEAP_COUNT; i++) {
        wp->heapHandle[i] = MEMCreateExpHeapEx(wp->heapStart[i], (u32)wp->heapEnd[i] - (u32)wp->heapStart[i], 0);
    }

    OSSetMem2ArenaLo((void *) max);
    
    for (i = 0; i < HEAP_COUNT; i++) {
        // Maybe memClear inlined?
        if (i == 7) {
            MEMDestroyExpHeap(wp->heapHandle[i]);
            MEMCreateExpHeapEx(wp->heapStart[i], (u32)wp->heapEnd[i] - (u32)wp->heapStart[i], 4);
        }
        else {
            MEMDestroyExpHeap(wp->heapHandle[i]);
            MEMCreateExpHeapEx(wp->heapStart[i], (u32)wp->heapEnd[i] - (u32)wp->heapStart[i], 4 | 1);
        }
    }

    memInitFlag = true;
}

void memClear(s32 heapId) {
    if (heapId == 7) {
        MEMDestroyExpHeap(wp->heapHandle[heapId]);
        MEMCreateExpHeapEx(wp->heapStart[heapId], (u32)wp->heapEnd[heapId] - (u32)wp->heapStart[heapId], 4);
    }
    else {
        MEMDestroyExpHeap(wp->heapHandle[heapId]);
        MEMCreateExpHeapEx(wp->heapStart[heapId], (u32)wp->heapEnd[heapId] - (u32)wp->heapStart[heapId], 4 | 1);
    }
}

void * __memAlloc(s32 heapId, size_t size) {
    void * p = MEMAllocFromExpHeapEx(wp->heapHandle[heapId], size, 0x20);
    assertf(p, "メモリ確保エラー [id = %d][size = %d]", heapId, size);
    return p;
}

void __memFree(s32 heapId, void * ptr) {
    MEMFreeToExpHeap(wp->heapHandle[heapId], ptr);
}

void smartInit() {
    u32 size = MEMGetAllocatableSizeForExpHeapEx(wp->heapHandle[SMART_HEAP_ID], 4);
    void * p = MEMAllocFromExpHeapEx(wp->heapHandle[SMART_HEAP_ID], size, 0x20);
    assertf(p, "メモリ確保エラー [id = %d][size = %d]", SMART_HEAP_ID, size);
    swp->heapStart = p;

    swp->allocatedStart = 0;
    swp->allocatedEnd = 0;
    swp->heapStartSpace = MEMGetSizeForMBlockExpHeap(swp->heapStart);
    SmartAllocation * curAllocation = swp->allocations;
    for (int i = 0; i < SMART_ALLOCATION_MAX; i++) {
        curAllocation->next = curAllocation + 1;
        curAllocation->prev = curAllocation - 1;
        curAllocation++;
    }
    swp->freeStart = &swp->allocations[0];
    swp->freeStart->prev = NULL;
    swp->freeEnd = &swp->allocations[SMART_ALLOCATION_MAX - 1];
    swp->freeEnd->next = NULL;
    
    swp->freedThisFrame = 0;
    g_bFirstSmartAlloc = false;
}

void smartAutoFree(s32 type) {
    SmartAllocation * curAllocation;
    SmartAllocation * next; // only way register usage matched for next in 2nd loop

    curAllocation = swp->allocatedStart;
    while (curAllocation != NULL) {
        next = curAllocation->next;
        if (curAllocation->type == (type & 0xffff)) {
            smartFree(curAllocation);
        }
        curAllocation = next;
    }
    if (type == 3) {
        curAllocation = swp->allocatedStart;
        while (curAllocation != NULL) {
            next = curAllocation->next;
            if ((s8) curAllocation->type == 4) {
                smartFree(curAllocation);
            }
            curAllocation = next;
        }
    }
}

void smartFree(SmartAllocation * lp) {
    assertf(lp, "無効なポインタです。p=0x%x\n", lp);
    assertf(lp->flag != 0, "すでに開放されています。p=0x%x\n", lp);
    if (lp->type == 4) {
        lp->type = 3;
    }
    else {
        // Update previous item in allocated list
        if (IS_FIRST_SMART_ALLOC(lp)) {
            swp->allocatedStart = lp->next;
            if (swp->allocatedStart) {
                swp->allocatedStart->prev = NULL;
            }
        }
        else {
            lp->prev->next = lp->next;
        }

        // Update next item in allocated list
        if (IS_LAST_SMART_ALLOC(lp)) {
            swp->allocatedEnd = lp->prev;
            if (swp->allocatedEnd) {
                swp->allocatedEnd->next = NULL;
            }
        }
        else {
            lp->next->prev = lp->prev;
        }
        
        // Update spaceAfter of previous item in allocated list
        size_t spaceFreed = lp->size + lp->spaceAfter;
        if (!IS_FIRST_SMART_ALLOC(lp)) {
            lp->prev->spaceAfter += spaceFreed;
        }
        else {
            swp->heapStartSpace += spaceFreed;
        }

        // Append to free list
        if (swp->freeStart == NULL) {
            swp->freeStart = lp;
            lp->prev = NULL;
        }
        else {
            // "The list structure is broken"
            // was getting a compiler error with plain sjis
            assert(swp->freeEnd, "リスト" "\x8d\x5c\x91\xA2" "が壊れています");
            swp->freeEnd->next = lp;
            lp->prev = swp->freeEnd;
        }
        lp->flag = 0;
        lp->next = NULL;
        lp->size = 0;
        lp->spaceAfter = 0;
        lp->unknown_0x8 = 0;
        swp->freeEnd = lp;
        swp->freedThisFrame++;
    }
}

SmartAllocation * smartAlloc(size_t size, u8 type) {
    // Special behaviour if this is the first time running
    if (!g_bFirstSmartAlloc) {
        g_bFirstSmartAlloc = true;
        smartAutoFree(3); // inline
    }

    // Wait if any allocations were freed this frame
    if (swp->freedThisFrame != 0) {
        sysWaitDrawSync();
        swp->freedThisFrame = 0;
    }

    // Pick a free SmartAllocation to use
    SmartAllocation * new_lp = swp->freeStart;
    assert(new_lp, "ヒープリスト足りない\n");
    
    // Update previous item in the free list
    if (IS_FIRST_SMART_ALLOC(new_lp)) {
        swp->freeStart = new_lp->next;
        if (swp->freeStart) {
            swp->freeStart->prev = NULL;
        }
    }
    else {
        new_lp->prev->next = new_lp->next;
    }

    // Update next item in the free list
    if (IS_LAST_SMART_ALLOC(new_lp)) {
        swp->freeEnd = new_lp->prev;
        if (swp->freeEnd) {
            swp->freeEnd->next = NULL;
        }
    }
    else {
        new_lp->next->prev = new_lp->prev;
    }

    // Round up 0x20
    if (size & 0x1f) {
        size += 0x20 - (size & 0x1f);
    }

    // Fill in some of the new allocation
    new_lp->flag = 1;
    new_lp->type = type;
    new_lp->size = size;
    new_lp->unknown_0x8 = 0;

    if (swp->heapStartSpace >= size) {
        // If it'll fit at the start of the heap, insert at the start of the list
        new_lp->data = swp->heapStart;
        new_lp->spaceAfter = swp->heapStartSpace - size;
        new_lp->next = swp->allocatedStart;
        new_lp->prev = NULL;
        swp->heapStartSpace = 0;
        if (swp->allocatedStart) {
            swp->allocatedStart->prev = new_lp;
        }
        swp->allocatedStart = new_lp;
        if(IS_LAST_SMART_ALLOC(new_lp)) {
            // This line isn't matching
            new_lp->spaceAfter = (u32)swp->heapStart + MEMGetSizeForMBlockExpHeap(swp->heapStart)
                                    - (u32) new_lp->data - new_lp->size;
            swp->allocatedEnd = new_lp;
        }
        return new_lp;
    }
    else {
        // If it'll fit after any existing allocations, insert there
        SmartAllocation * lp = swp->allocatedStart;
        while (lp != NULL) {
            if (lp->spaceAfter >= size) {
                new_lp->data = (void *) ((u32)lp->data + lp->size);
                new_lp->spaceAfter = lp->spaceAfter - size;
                new_lp->next = lp->next;
                new_lp->prev = lp;
                lp->spaceAfter = 0;
                if (!IS_LAST_SMART_ALLOC(lp)) {
                    lp->next->prev = new_lp;
                }
                else {
                    swp->allocatedEnd = new_lp;
                }
                lp->next = new_lp;
                return new_lp;
            }
            lp = lp->next;
        }

        // Try freeing space as a last resort
        for (int i = 0; i < 3; i++) {
            switch (i) {
                case 0:
                    // required to match
                    break;
                case 1:
                    _fileGarbage(1);
                    break;
                case 2:
                    _fileGarbage(0);
                    break;
            }
            smartGarbage();
            lp = swp->allocatedEnd;
            if (lp->spaceAfter >= size) {
                new_lp->data = (void *) ((u32)lp->data + lp->size);
                new_lp->spaceAfter = lp->spaceAfter - size;
                new_lp->next = lp->next;
                new_lp->prev = lp;
                lp->spaceAfter = 0;
                if (!IS_LAST_SMART_ALLOC(lp)) {
                    lp->next->prev = new_lp;
                }
                else {
                    swp->allocatedEnd = new_lp;
                }
                lp->next = new_lp;
                return new_lp;
            }
        }

        assert(0, "smartAlloc: ガーベージコレクトしたけどヒープ足りない\n");
        return NULL;
    }
}

// a lot

#undef IS_FIRST_SMART_ALLOC
#undef IS_LAST_SMART_ALLOC
#pragma inline_max_auto_size(5) // reset
