// Commands definition

DEF_CMD(ADD, "add", true, { PUSH_NUM(ADD); })
DEF_CMD(SUB, "sub", true, { PUSH_NUM(SUB); })
DEF_CMD(MUL, "mul", true, { PUSH_NUM(MUL); })
DEF_CMD(DIV, "div", true, { PUSH_NUM(DIV); })
DEF_CMD(MOD, "mod", true, { PUSH_NUM(MOD); })
DEF_CMD(CMP, "cmp", true, { PUSH_NUM(CMP); })
DEF_CMD(MOV, "mov", true, { PUSH_NUM(MOV); })
DEF_CMD(JMP, "jmp", true, { PUSH_NUM(JMP); })
DEF_CMD(JL, "jl", true, { PUSH_NUM(JL); })
DEF_CMD(JLE, "jle", true, { PUSH_NUM(JLE); })
DEF_CMD(JEQ, "jeq", true, { PUSH_NUM(JEQ); })
DEF_CMD(JNE, "jne", true, { PUSH_NUM(JNE); })
DEF_CMD(JGE, "jge", true, { PUSH_NUM(JGE); })
DEF_CMD(JG, "jg", true, { PUSH_NUM(JG); })
DEF_CMD(PUSH, "push", true, { PUSH_NUM(PUSH); })
DEF_CMD(POP, "pop", true, { PUSH_NUM(POP); })
DEF_CMD(IN, "in", true, { PUSH_NUM(IN); })
DEF_CMD(OUT, "out", true, { PUSH_NUM(OUT); })
DEF_CMD(OUTC, "outc", true, { PUSH_NUM(OUTC); })

#define DEF_RL_RR_CMD(cmd, text) \
DEF_CMD(cmd ## _RL, text, (ARGS_SIZE == 2 && IS_REG(0) && IS_LONG(1)), { \
	PUSH_NUM(cmd ## _RL); \
}) \
DEF_CMD(cmd ## _RSZ, text, (ARGS_SIZE == 2 && IS_REG(0) && IS_LONG(1)), { \
	PUSH_NUM(cmd ## _RL); \
}) \
DEF_CMD(cmd ## _RR, text, (ARGS_SIZE == 2 && IS_REG(0) && IS_REG(1)),  { \
	PUSH_NUM(cmd ## _RR); \
})\
DEF_CMD(cmd ## _RS, text, (ARGS_SIZE == 2 && IS_REG(0) && IS_REG_STAR(1)), { \
	PUSH_NUM(cmd ## _RS); \
})\
DEF_CMD(cmd ## _SR, text, (ARGS_SIZE == 2 && IS_REG_STAR(0) && IS_REG(1)), { \
	PUSH_NUM(cmd ## _SR); \
})\
DEF_CMD(cmd ## _SS, text, (ARGS_SIZE == 2 && IS_REG_STAR(0) && IS_REG_STAR(1)), { \
	PUSH_NUM(cmd ## _SS); \
})

DEF_RL_RR_CMD(ADDL, "addl")
DEF_RL_RR_CMD(SUBL, "subl")
DEF_RL_RR_CMD(MULL, "mull")
DEF_RL_RR_CMD(DIVL, "divl")
DEF_RL_RR_CMD(MODL, "modl")
DEF_RL_RR_CMD(MOVL, "movl")
DEF_RL_RR_CMD(ANDL, "andl")

DEF_CMD(MOVL_REG_STATIC_STAR, "movl", (ARGS_SIZE == 2 && IS_REG(0) && IS_STATIC_STAR(1)), { PUSH_NUM(MOVL_RSZ) })

DEF_CMD(PUSHR, "pushr", true, { PUSH_NUM(PUSHR); })
DEF_CMD(POPR, "popr", true, { PUSH_NUM(POPR); })

// AHTUNG
DEF_CMD(MEW, "mew", true, { PUSH_NUM(MEW); })
DEF_CMD(CALL, "call", true, { PUSH_NUM(CALL); })
DEF_CMD(RET, "ret", true, { PUSH_NUM(RET); })

#ifdef  ASSEMBLING
// Assembly derectives

DEF_CMD(STACK_SIZE, ".stack_size", true, { SET_STACK_SIZE(line, first_word_end); return; })
DEF_CMD(DEFINE_TEXT_DATA, ".text", true, { PUSH_TEXT_DATA(line, first_word_end); return; })

#endif //  ASSEMBLING

DEF_CMD(MAX_CMD_NUM, "MAX_CMD_NUM", true, {})