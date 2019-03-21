#pragma once

/*
 * Instruction mapping
 *
 * 	Arithmetics:
 *
 * 	0 - add
 *	1 - sub
 *	2 - mul
 *	3 - div
 *	4 - mod
 *
 *	5 - cmp
 *
 *	6 - mov
 *
 *	Jumps:
 *
 *	7 - jmp
 *	8 - jl
 *	9 - jle
 *	10 - jeq
 *	11 - jge
 *	12 - jg
 *	13 - PUSH
*	14 - POP
*	15 - IN
*	16 - OUT

*/

enum class CPUVASM_ARG_TYPES {
	REG,
	LONG,
	LABEL,
	REG_STAR,
	LONG_STAR,
	STATIC_DATA_STAR
};

enum CPUE_CMD_NUM {
#define DEF_CMD(CMD, WORD, PARSE_CODE, PROC_CODE) CMD ,
#include "cpuemu-cmd-defs.h"
#undef DEF_CMD
};


const std::string __zZz_CAT_zZz__ =

"                   ;,_            ,                                    \n"
"                  _uP~'b          d'u,                                 \n"
"                 dP'   ''b       ,d'  'o                               \n"
"                d'    , `b     d''    'b                               \n"
"               l] [    ' `l,  d'       lb                              \n"
"               Ol ?     '  'b`'=uoqo,_  'l                             \n"
"             ,dBb 'b        'b,    `'~~TObup,_                         \n"
"           ,d' (db.`'         ''     'tbc,_ `~'Yuu,_                   \n"
"         .d' l`T'  '=                      ~     `''Yu,                \n"
"       ,dO` gP,                           `u,   b,_  'b7               \n"
"      d?' ,d' l,                           `'b,_ `~b  '1               \n"
"    ,8i' dl   `l                 ,ggQOV',dbgq,._'  `l  lb              \n"
"   .df' (O,    '             ,ggQY'~  , @@@@@d'bd~  `b '1              \n"
"  .df'   `'           -=@QgpOY''     (b  @@@@P db    `Lp'b,            \n"
" .d(                  _               'ko '=d_,Q`  ,_  '  'b,          \n"
" Ql         .         `'qo,._          'tQo,_`''bo ;tb,    `'b,        \n"
" qQ         |L           ~'QQQgggc,_.,dObc,opooO  `'~~';.   __,7,      \n"
" qp         t\io,_           `~'TOOggQV''''        _,dg,_ =PIQHib.     \n"
" `qp        `Q['tQQQo,_                          ,pl{QOP''   7AFR`     \n"
"   `         `tb  '''tQQQg,_             p' 'b   `       .;-.`Vl'      \n"
"              'Yb      `'tQOOo,__    _,edb    ` .__   /`/'|  |b;=;.__  \n"
"                            `'tQQQOOOOP''`'\QV;qQObob'`-._`\_~~-._     \n"
"                                 ''''    ._        /   | |oP'\_   ~\   \n"
"                                         `~'\ic,qggddOOP'|  |  ~\   `  \n"
"                                           ,qP`'''|'   | `\ `;   `\    \n"
"                                _        _,p'     |    |   `\`;    |   \n"
"        Meooow!                 'boo,._dP'       `\     `\    `\       \n"
"                                  `'7tY~'                              \n"
"                                                                       \n";
