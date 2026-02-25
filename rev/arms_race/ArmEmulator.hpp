#pragma once

#include <cstdint>

struct CPUState
{
    uint32_t regs[16] = { 0 };
    bool carry = false;
};

// ARM Opcodes Mapping
enum class Opcode : uint32_t
{
    AND = 0x0,
    EOR = 0x1,
    SUB = 0x2,
    RSB = 0x3,
    ADD = 0x4,
    ADC = 0x5,
    SBC = 0x6,
    ORR = 0xC,
    MOV = 0xD
};

class ARMEmulator
{
public:
    static void Execute(uint32_t val, CPUState& state)
    {
        // --- Instruction Decoding ---
        uint32_t opNum  = (val >> 21) & 0xF;
        Opcode opcode   = static_cast<Opcode>(opNum);
        bool s_bit      = (val >> 20) & 1;
        bool i_bit      = (val >> 25) & 1;
        int rn_idx      = (val >> 16) & 0xF;
        int rd_idx      = (val >> 12) & 0xF;

        // --- Type 1: Move Wide (MOVW / MOVT) ---
        // Pattern: xxxx 0011 0000 .... .... .... .... .... (MOVW)
        // Pattern: xxxx 0011 0100 .... .... .... .... .... (MOVT)
        if ((val & 0x0FB00000) == 0x03000000)
        {
            uint32_t imm = ((val >> 4) & 0xF000) | (val & 0x0FFF);
            bool is_top  = (val >> 22) & 1;

            if (is_top)
            {
                state.regs[rd_idx] = (state.regs[rd_idx] & 0x0000FFFF) | (imm << 16);
            }
            else
            {
                state.regs[rd_idx] = imm;
            }
            return;
        }

        // --- Type 2: Multiplication (MUL) ---
        if ((val & 0x0FC000F0) == 0x00000090)
        {
            int rm_idx = val & 0xF;
            int rs_idx = (val >> 8) & 0xF;
            state.regs[rd_idx] = state.regs[rm_idx] * state.regs[rs_idx];
            return;
        }

        // --- Type 3: Data Processing ---

        // Resolve Operand2
        uint32_t operand2;
        if (i_bit)
        {
            uint32_t imm = val & 0xFF;
            uint32_t rotate = (val >> 8) & 0xF;
            operand2 = (imm >> (rotate * 2)) | (imm << (32 - (rotate * 2)));
        }
        else
        {
            operand2 = state.regs[val & 0xF];
        }

        uint32_t rnVal = state.regs[rn_idx];
        uint32_t rdResult = 0;

        switch (opcode)
        {
            case Opcode::AND:
                rdResult = rnVal & operand2;
                break;

            case Opcode::EOR:
                rdResult = rnVal ^ operand2;
                break;

            case Opcode::SUB:
                if (s_bit) state.carry = (rnVal >= operand2);
                rdResult = rnVal - operand2;
            break;

            case Opcode::RSB:
                if (s_bit) state.carry = (operand2 >= rnVal);
                rdResult = operand2 - rnVal;
            break;

            case Opcode::ADD:
            {
                uint64_t res = static_cast<uint64_t>(rnVal) + operand2;
                if (s_bit) state.carry = (res > 0xFFFFFFFFULL);
                rdResult = static_cast<uint32_t>(res);
                break;
            }

            case Opcode::ADC:
            {
                uint64_t res = static_cast<uint64_t>(rnVal) + operand2 + (state.carry ? 1 : 0);
                if (s_bit) state.carry = (res > 0xFFFFFFFFULL);
                rdResult = static_cast<uint32_t>(res);
                break;
            }

            case Opcode::SBC:
            {
                uint32_t borrow_val = state.carry ? 0 : 1;
                if (s_bit) state.carry = (rnVal >= (static_cast<uint64_t>(operand2) + borrow_val));
                rdResult = rnVal - operand2 - borrow_val;
                break;
            }

            case Opcode::ORR:
                rdResult = rnVal | operand2;
                break;

            case Opcode::MOV:
                rdResult = operand2;
                break;

            default:
                // Placeholder for unknown opcodes
                return;
        }

        state.regs[rd_idx] = rdResult;
    }
};
