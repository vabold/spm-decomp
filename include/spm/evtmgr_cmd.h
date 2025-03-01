/*
  Functions to execute interpreted evt script code and handle evt variables
*/

#pragma once

#include <common.h>
#include <spm/evtmgr.h>

// Data types
#define EVTDAT_ADDR_MAX -290000000
#define EVTDAT_FLOAT_MAX -220000000
#define EVTDAT_UF_MAX -200000000
#define EVTDAT_UW_MAX -180000000
#define EVTDAT_GSW_MAX -160000000
#define EVTDAT_LSW_MAX -140000000
#define EVTDAT_GSWF_MAX -120000000
#define EVTDAT_LSWF_MAX -100000000
#define EVTDAT_GF_MAX -80000000
#define EVTDAT_LF_MAX -60000000
#define EVTDAT_GW_MAX -40000000
#define EVTDAT_LW_MAX -20000000

#define EVTDAT_ADDR_BASE 270000000
#define EVTDAT_FLOAT_BASE 240000000
#define EVTDAT_UF_BASE 210000000
#define EVTDAT_UW_BASE 190000000
#define EVTDAT_GSW_BASE 170000000
#define EVTDAT_LSW_BASE 150000000
#define EVTDAT_GSWF_BASE 130000000
#define EVTDAT_LSWF_BASE 110000000
#define EVTDAT_GF_BASE 90000000
#define EVTDAT_LF_BASE 70000000
#define EVTDAT_GW_BASE 50000000
#define EVTDAT_LW_BASE 30000000

#define UF(id) ((id) - EVTDAT_UF_BASE)
#define UW(id) ((id) - EVTDAT_UW_BASE)
#define GSW(id) ((id) - EVTDAT_GSW_BASE)
#define LSW(id) ((id) - EVTDAT_LSW_BASE)
#define GSWF(id) ((id) - EVTDAT_GSWF_BASE)
#define LSWF(id) ((id) - EVTDAT_LSWF_BASE)
#define GF(id) ((id) - EVTDAT_GF_BASE)
#define LF(id) ((id) - EVTDAT_LF_BASE)
#define GW(id) ((id) - EVTDAT_GW_BASE)
#define LW(id) ((id) - EVTDAT_LW_BASE)
#define FLOAT(value) (((s32) ((value) * 1024.f)) - EVTDAT_FLOAT_BASE)

#define EVT_NULLPTR (-EVTDAT_ADDR_BASE)

/*
    Instruction / User Func Return values
*/

// Stop execution of all scripts this frame
#define EVT_RET_END_FRAME -1

// Keep re-executing current instruction, continue next frame if too much time passed
#define EVT_RET_BLOCK_WEAK 0

// Move to next instruction, continue next frame if too much time passed
#define EVT_RET_CONTINUE_WEAK 1

// Move to next instruction
#define EVT_RET_CONTINUE 2

// Keep re-executing current instruction
#define EVT_RET_BLOCK 3

// Stop running current script
#define EVT_RET_END 0xff

/*
    evtmgrCmd return values
*/

#define EVTCMD_RET_END -1
#define EVTCMD_RET_CONTINUE 0
#define EVTCMD_RET_END_ALL 1

/*
    Opcodes
*/
enum EvtOpcode
{
    EVT_OPC_NEXT,
    EVT_OPC_END_SCRIPT,
    EVT_OPC_END_EVT,
    EVT_OPC_LBL,
    EVT_OPC_GOTO,
    EVT_OPC_DO,
    EVT_OPC_WHILE,
    EVT_OPC_DO_BREAK,
    EVT_OPC_DO_CONTINUE,
    EVT_OPC_WAIT_FRM,
    EVT_OPC_WAIT_MSEC,
    EVT_OPC_HALT,
    EVT_OPC_IF_STR_EQUAL,
    EVT_OPC_IF_STR_NOT_EQUAL,
    EVT_OPC_IF_STR_SMALL,
    EVT_OPC_IF_STR_LARGE,
    EVT_OPC_IF_STR_SMALL_EQUAL,
    EVT_OPC_IF_STR_LARGE_EQUAL,
    EVT_OPC_IFF_EQUAL,
    EVT_OPC_IFF_NOT_EQUAL,
    EVT_OPC_IFF_SMALL,
    EVT_OPC_IFF_LARGE,
    EVT_OPC_IFF_SMALL_EQUAL,
    EVT_OPC_IFF_LARGE_EQUAL,
    EVT_OPC_IF_EQUAL,
    EVT_OPC_IF_NOT_EQUAL,
    EVT_OPC_IF_SMALL,
    EVT_OPC_IF_LARGE,
    EVT_OPC_IF_SMALL_EQUAL,
    EVT_OPC_IF_LARGE_EQUAL,
    EVT_OPC_IF_FLAG,
    EVT_OPC_IF_NOT_FLAG,
    EVT_OPC_ELSE,
    EVT_OPC_END_IF,
    EVT_OPC_SWITCH,
    EVT_OPC_SWITCHI,
    EVT_OPC_CASE_EQUAL,
    EVT_OPC_CASE_NOT_EQUAL,
    EVT_OPC_CASE_SMALL,
    EVT_OPC_CASE_LARGE,
    EVT_OPC_CASE_SMALL_EQUAL,
    EVT_OPC_CASE_LARGE_EQUAL,
    EVT_OPC_CASE_ETC,
    EVT_OPC_CASE_OR,
    EVT_OPC_CASE_AND,
    EVT_OPC_CASE_FLAG,
    EVT_OPC_CASE_END,
    EVT_OPC_CASE_BETWEEN,
    EVT_OPC_SWITCH_BREAK,
    EVT_OPC_END_SWITCH,
    EVT_OPC_SET,
    EVT_OPC_SETI,
    EVT_OPC_SETF,
    EVT_OPC_ADD,
    EVT_OPC_SUB,
    EVT_OPC_MUL,
    EVT_OPC_DIV,
    EVT_OPC_MOD,
    EVT_OPC_ADDF,
    EVT_OPC_SUBF,
    EVT_OPC_MULF,
    EVT_OPC_DIVF,
    EVT_OPC_SET_READ,
    EVT_OPC_READ,
    EVT_OPC_READ2,
    EVT_OPC_READ3,
    EVT_OPC_READ4,
    EVT_OPC_READ_N,
    EVT_OPC_SET_READF,
    EVT_OPC_READF,
    EVT_OPC_READF2,
    EVT_OPC_READF3,
    EVT_OPC_READF4,
    EVT_OPC_READF_N,
    EVT_OPC_CLAMP_INT,
    EVT_OPC_SET_USER_WRK,
    EVT_OPC_SET_USER_FLG,
    EVT_OPC_ALLOC_USER_WRK,
    EVT_OPC_AND,
    EVT_OPC_ANDI,
    EVT_OPC_OR,
    EVT_OPC_ORI,
    EVT_OPC_SET_FRAME_FROM_MSEC,
    EVT_OPC_SET_MSEC_FROM_FRAME,
    EVT_OPC_SET_RAM,
    EVT_OPC_SET_RAMF,
    EVT_OPC_GET_RAM,
    EVT_OPC_GET_RAMF,
    EVT_OPC_SETR,
    EVT_OPC_SETRF,
    EVT_OPC_GETR,
    EVT_OPC_GETRF,
    EVT_OPC_USER_FUNC,
    EVT_OPC_RUN_EVT,
    EVT_OPC_RUN_EVT_ID,
    EVT_OPC_RUN_CHILD_EVT,
    EVT_OPC_DELETE_EVT,
    EVT_OPC_RESTART_EVT,
    EVT_OPC_SET_PRI,
    EVT_OPC_SET_SPD,
    EVT_OPC_SET_TYPE,
    EVT_OPC_STOP_ALL,
    EVT_OPC_START_ALL,
    EVT_OPC_STOP_OTHER,
    EVT_OPC_START_OTHER,
    EVT_OPC_STOP_ID,
    EVT_OPC_START_ID,
    EVT_OPC_CHK_EVT,
    EVT_OPC_INLINE_EVT,
    EVT_OPC_INLINE_EVT_ID,
    EVT_OPC_END_INLINE,
    EVT_OPC_BROTHER_EVT,
    EVT_OPC_BROTHER_EVT_ID,
    EVT_OPC_END_BROTHER,
    EVT_OPC_DEBUG_PUT_MSG,
    EVT_OPC_DEBUG_MSG_CLEAR,
    EVT_OPC_DEBUG_PUT_REG,
    EVT_OPC_DEBUG_NAME,
    EVT_OPC_DEBUG_REM,
    EVT_OPC_DEBUG_BP
};

// Script instruction functions, 800da0b0 - 800ddd5b
s32 evt_end_evt(EvtEntry * entry);
s32 evt_lbl(EvtEntry * entry);
s32 evt_goto(EvtEntry * entry);
s32 evt_do(EvtEntry * entry);
s32 evt_while(EvtEntry * entry);
s32 evt_do_break(EvtEntry * entry);
s32 evt_do_continue(EvtEntry * entry);
s32 evt_wait_frm(EvtEntry * entry);
s32 evt_wait_msec(EvtEntry * entry);
s32 evt_halt(EvtEntry * entry);
s32 evt_if_str_equal(EvtEntry * entry);
s32 evt_if_str_not_equal(EvtEntry * entry);
s32 evt_if_str_small(EvtEntry * entry);
s32 evt_if_str_large(EvtEntry * entry); 
s32 evt_if_str_small_equal(EvtEntry * entry);
s32 evt_if_str_large_equal(EvtEntry * entry); 
s32 evt_iff_equal(EvtEntry * entry);
s32 evt_iff_not_equal(EvtEntry * entry);
s32 evt_iff_small(EvtEntry * entry);
s32 evt_iff_large(EvtEntry * entry);
s32 evt_iff_small_equal(EvtEntry * entry);
s32 evt_iff_large_equal(EvtEntry * entry);
s32 evt_if_equal(EvtEntry * entry);
s32 evt_if_not_equal(EvtEntry * entry);
s32 evt_if_small(EvtEntry * entry);
s32 evt_if_large(EvtEntry * entry);
s32 evt_if_small_equal(EvtEntry * entry);
s32 evt_if_large_equal(EvtEntry * entry);
s32 evt_if_flag(EvtEntry * entry);
s32 evt_if_not_flag(EvtEntry * entry);
s32 evt_else(EvtEntry * entry);
s32 evt_end_if(EvtEntry * entry);
s32 evt_switch(EvtEntry * entry);
s32 evt_switchi(EvtEntry * entry);
s32 evt_case_equal(EvtEntry * entry);
s32 evt_case_not_equal(EvtEntry * entry);
s32 evt_case_small(EvtEntry * entry);
s32 evt_case_small_equal(EvtEntry * entry);
s32 evt_case_large(EvtEntry * entry);
s32 evt_case_large_equal(EvtEntry * entry);
s32 evt_case_between(EvtEntry * entry);
s32 evt_case_etc(EvtEntry * entry);
s32 evt_case_flag(EvtEntry * entry);
s32 evt_case_or(EvtEntry * entry);
s32 evt_case_and(EvtEntry * entry);
s32 evt_case_end(EvtEntry * entry);
s32 evt_switch_break(EvtEntry * entry);
s32 evt_end_switch(EvtEntry * entry);
s32 evt_set(EvtEntry * entry);
s32 evt_seti(EvtEntry * entry);
s32 evt_setf(EvtEntry * entry);
s32 evt_add(EvtEntry * entry);
s32 evt_sub(EvtEntry * entry);
s32 evt_mul(EvtEntry * entry);
s32 evt_div(EvtEntry * entry);
s32 evt_mod(EvtEntry * entry);
s32 evt_addf(EvtEntry * entry);
s32 evt_subf(EvtEntry * entry);
s32 evt_mulf(EvtEntry * entry);
s32 evt_divf(EvtEntry * entry);
s32 evt_set_read(EvtEntry * entry);
s32 evt_set_readf(EvtEntry * entry);
s32 evt_read(EvtEntry * entry);
s32 evt_read2(EvtEntry * entry);
s32 evt_read3(EvtEntry * entry);
s32 evt_read4(EvtEntry * entry);
s32 evt_read_n(EvtEntry * entry);
s32 evt_readf(EvtEntry * entry);
s32 evt_readf2(EvtEntry * entry);
s32 evt_readf3(EvtEntry * entry);
s32 evt_readf4(EvtEntry * entry);
s32 evt_readf_n(EvtEntry * entry);
s32 evt_clamp_int(EvtEntry * entry);
s32 evt_set_user_wrk(EvtEntry * entry);
s32 evt_set_user_flg(EvtEntry * entry);
s32 evt_alloc_user_wrk(EvtEntry * entry);
s32 evt_and(EvtEntry * entry);
s32 evt_andi(EvtEntry * entry);
s32 evt_or(EvtEntry * entry);
s32 evt_ori(EvtEntry * entry);
s32 evt_set_frame_from_msec(EvtEntry * entry);
s32 evt_set_msec_from_frame(EvtEntry * entry);
s32 evt_set_ram(EvtEntry * entry);
s32 evt_set_ramf(EvtEntry * entry);
s32 evt_get_ram(EvtEntry * entry);
s32 evt_get_ramf(EvtEntry * entry);
s32 evt_setr(EvtEntry * entry);
s32 evt_setrf(EvtEntry * entry);
s32 evt_getr(EvtEntry * entry);
s32 evt_getrf(EvtEntry * entry);
s32 evt_user_func(EvtEntry * entry);
s32 evt_run_evt(EvtEntry * entry);
s32 evt_run_evt_id(EvtEntry * entry);
s32 evt_run_child_evt(EvtEntry * entry);
s32 evt_restart_evt(EvtEntry * entry);
s32 evt_delete_evt(EvtEntry * entry);
s32 evt_set_pri(EvtEntry * entry);
s32 evt_set_spd(EvtEntry * entry);
s32 evt_set_type(EvtEntry * entry);
s32 evt_stop_all(EvtEntry * entry);
s32 evt_start_all(EvtEntry * entry);
s32 evt_stop_other(EvtEntry * entry);
s32 evt_start_other(EvtEntry * entry);
s32 evt_stop_id(EvtEntry * entry);
s32 evt_start_id(EvtEntry * entry);
s32 evt_chk_evt(EvtEntry * entry);
s32 evt_inline_evt(EvtEntry * entry);
s32 evt_inline_evt_id(EvtEntry * entry);
s32 evt_end_inline(EvtEntry * entry);
s32 evt_brother_evt(EvtEntry * entry);
s32 evt_brother_evt_id(EvtEntry * entry);
s32 evt_end_brother(EvtEntry * entry);
s32 evt_debug_put_msg(EvtEntry * entry);
s32 evt_debug_msg_clear(EvtEntry * entry);
s32 evt_debug_put_reg(EvtEntry * entry);
s32 evt_debug_name(EvtEntry * entry);
s32 evt_debug_rem(EvtEntry * entry);
s32 evt_debug_bp(EvtEntry * entry);

/*
    Executes an entry's script until it pauses or finishes
*/
s32 evtmgrCmd(EvtEntry * entry);

/*
    Gets the value of an evt variable
*/
s32 evtGetValue(EvtEntry * entry, s32 variable);

// evtGetNumber (inlined / unused)

/*
    Sets the value of an evt variable
*/
s32 evtSetValue(EvtEntry * entry, s32 variable, s32 value);

/*
    Gets the value of a float evt variable
*/
f32 evtGetFloat(EvtEntry * entry, s32 variable);

/*
    Sets the value of a float evt variable
*/
f32 evtSetFloat(EvtEntry * entry, s32 variable, f32 value);

/*
    Returns a pointer to the instruction after the specified label
    in an entry's script
*/
EvtScriptCode * evtSearchLabel(EvtEntry * entry, s32 id); // inlined

/*
    Returns a pointer to the instruction after the next else on the
    current if else depth in an entry's script
*/
EvtScriptCode * evtSearchElse(EvtEntry * entry);

/*
    Returns a pointer to the instruction after the next end if on the
    current if else depth in an entry's script
*/
EvtScriptCode * evtSearchEndIf(EvtEntry * entry);

/*
    Returns a pointer to the the next end switch on the current switch
    depth in an entry's script
*/
EvtScriptCode * evtSearchEndSwitch(EvtEntry * entry);

/*
    Returns a pointer to the next case or end switch on the current
    switch depth in an entry's script
*/
EvtScriptCode * evtSearchCase(EvtEntry * entry);

/*
    Returns a pointer to the instruction after the next while on the
    current do while depth in an entry's script
*/
EvtScriptCode * evtSearchWhile(EvtEntry * entry);

/*
    Returns a pointer to the next while instruction on the current do
    while depth in an entry's script
*/
EvtScriptCode * evtSearchJustBeforeWhile(EvtEntry * entry);
