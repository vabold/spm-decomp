#include <common.h>
#include <spm/filemgr.h>
#include <spm/memory.h>
#include <spm/relmgr.h>
#include <spm/system.h>
#include <wii/dvd.h>
#include <wii/lzss10.h>
#include <wii/os.h>
#include <wii/stdio.h>
#include <wii/string.h>

#pragma pool_strings off

// .bss
static RelWork work;

// .sdata
static RelWork * wp = &work;
static const char * relDecompName = "relF.rel";
static const char * relCompName = "relF.bin";

void relInit()
{
    wp->loaded = false;
}

void relMain()
{
    char relPath[0x48];
    bool isCompressed;
    FileEntry * file;
    
    if (wp->loaded)
        return;

    isCompressed = 1;
    sprintf(relPath, "%s/rel/%s", getSpmarioDVDRoot(), relCompName);
    if (DVDConvertPathToEntrynum(relPath) == -1)
    {
        sprintf(relPath, "%s/rel/%s", getSpmarioDVDRoot(), relDecompName);
        isCompressed = 0;
    }

    if (fileAsyncf(0, 0, relPath) == 0)
        return;

    file = fileAllocf(0, relPath);
    if ((u32) file == 0xffffffff)
    {
        wp->loaded = true;
        return;
    }

    if (!isCompressed)
    {
        wp->relFile = __memAlloc(0, file->sp->size);
        memcpy(wp->relFile, file->sp->data, file->sp->size);
    }
    else
    {
        wp->relFile = __memAlloc(0, lzss10ParseHeader(file->sp->data).decompSize);
        lzss10Decompress(file->sp->data, wp->relFile);
    }

    fileFree(file);

    wp->bss = __memAlloc(0, wp->relFile->bssSize);
    memset(wp->bss, 0, wp->relFile->bssSize);
    OSLink(wp->relFile, wp->bss);
    wp->relFile->prolog();
    wp->loaded = true;
}

bool isRelLoaded()
{
    return wp->loaded;
}
