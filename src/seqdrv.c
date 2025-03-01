#include <common.h>
#include <spm/seqdrv.h>
#include <spm/seqdef.h>
#include <spm/system.h>
#include <wii/string.h>

// .bss
static SeqWork seqWork;

// .sdata
static s32 now_seq = -1;
static s32 next_seq = -1;
static s32 prev_seq = -1;

// .sbss
static char * next_p1; // map name
static char * next_p0; // door name

void seqInit_SPMARIO()
{
    memset(&seqWork, 0, sizeof(seqWork));
    now_seq = -1;
    next_seq = -1;
    prev_seq = -1;
}

void seqMain()
{
    if (next_seq != now_seq)
    {
        if (now_seq != -1)
        {
            sysWaitDrawSync();

            (*seq_data[now_seq].exit)(&seqWork);
        }

        prev_seq = now_seq;

        memset(&seqWork, 0, sizeof(seqWork));

        now_seq = next_seq;

        seqWork.seq = next_seq;    
        seqWork.stage = 0;
        seqWork.p0 = next_p0;
        seqWork.p1 = next_p1;
        seqWork.afterFunc = NULL;

        (*seq_data[now_seq].init)(&seqWork);
    }
    
    (*seq_data[now_seq].main)(&seqWork);

    if (seqWork.afterFunc != NULL)
        (*seqWork.afterFunc)(&seqWork);
}

void seqSetSeq(s32 seq, char * p0, char * p1)
{
    next_seq = seq;
    next_p0 = p0;
    next_p1 = p1;
}

s32 seqGetSeq()
{
    return now_seq == -1 ? 0 : now_seq;
}
