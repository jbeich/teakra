#pragma once
#include "common_types.h"

template<unsigned bits>
struct Oprand {
    static_assert(bits > 0 && bits <=16, "!");
    static constexpr unsigned Bits = bits;
    u16 storage;
};

template<typename OprandT, unsigned pos>
struct At {
    using OprandType = OprandT;
    static constexpr unsigned Bits = OprandType::Bits;
    static_assert((Bits < 16 && pos < 16 && Bits + pos <= 16)
     || (Bits == 16 && pos == 16), "!");
    static constexpr u16 Mask = (((1 << Bits) - 1) << pos) & 0xFFFF;
    static constexpr bool NeedExpansion = pos == 16;

    static OprandT Filter(u16 opcode, u16 expansion) {
        OprandT oprand;
        if (NeedExpansion)
            oprand.storage = expansion;
        else
            oprand.storage = (u16)((opcode & Mask) >> pos);
        return oprand;
    }
};

template<typename OprandT, u16 value>
struct Const {
    using OprandType = OprandT;
    static constexpr u16 Mask = 0;
    static constexpr bool NeedExpansion = false;
    static OprandT Filter(u16 opcode, u16 expansion) {
        OprandT oprand;
        oprand.storage = value;
        return oprand;
    }
};

template<typename OprandT, unsigned pos, u16 value>
struct AtConst {
    using Base = At<OprandT, pos>;
    static_assert(Base::NeedExpansion == false, "");
    static constexpr u16 Mask = Base::Mask;
    static constexpr u16 Pad = value << pos;
};

struct Dummy : Oprand<1> {};
using DummyMatch = Const<Dummy, 0>;

//////////////////////////////////////////////////////////////////////////////

constexpr unsigned log2(unsigned n) {
    if (n % 2 != 0)
        throw "wtf";
    return (n == 2) ? 1 : 1 + log2(n / 2);
}

template <typename EnumT, EnumT ... names>
struct EnumOprand : Oprand<log2(sizeof...(names))> {
    static constexpr EnumT values[] = {names...};
    EnumT GetName() const {
        return values[this->storage];
    }
};

template <typename EnumT>
struct EnumAllOprand : Oprand<log2((unsigned)EnumT::EnumEnd)> {
    EnumT GetName() const {
        return (EnumT)this->storage;
    }
};

enum class RegName {
    a0, a0l, a0h, a0e,
    a1, a1l, a1h, a1e,
    b0, b0l, b0h, b0e,
    b1, b1l, b1h, b1e,

    r0, r1, r2, r3, r4, r5, r6, r7,

    x0, y0, x1, y1, p0, p1, p,

    pc, sp, sv, lc,

    ar0, ar1,
    arp0, arp1, arp2, arp3,

    ext0, ext1, ext2, ext3,

    stt0, stt1, stt2,
    st0, st1, st2,
    cfgi, cfgj,
    mod0, mod1, mod2, mod3,

    undefine,
};

template <RegName ... reg_names>
using RegOprand = EnumOprand <RegName, reg_names...>;

struct Register : RegOprand<
    RegName::r0,
    RegName::r1,
    RegName::r2,
    RegName::r3,
    RegName::r4,
    RegName::r5,
    RegName::r7,
    RegName::y0,
    RegName::st0,
    RegName::st1,
    RegName::st2,
    RegName::p, // take special care of this as src oprand
    RegName::pc,
    RegName::sp,
    RegName::cfgi,
    RegName::cfgj,
    RegName::b0h,
    RegName::b1h,
    RegName::b0l,
    RegName::b1l,
    RegName::ext0,
    RegName::ext1,
    RegName::ext2,
    RegName::ext3,
    RegName::a0, // take special care of this as src oprand
    RegName::a1, // take special care of this as src oprand
    RegName::a0l,
    RegName::a1l,
    RegName::a0h,
    RegName::a1h,
    RegName::lc,
    RegName::sv
> {};
struct Ax : RegOprand<
    RegName::a0,
    RegName::a1
> {};
struct Axl : RegOprand<
    RegName::a0l,
    RegName::a1l
> {};
struct Axh : RegOprand<
    RegName::a0h,
    RegName::a1h
> {};
struct Bx : RegOprand<
    RegName::b0,
    RegName::b1
> {};
struct Bxl : RegOprand<
    RegName::b0l,
    RegName::b1l
> {};
struct Bxh : RegOprand<
    RegName::b0h,
    RegName::b1h
> {};
struct Px : RegOprand<
    RegName::p0,
    RegName::p1
> {};
struct Ab : RegOprand<
    RegName::b0,
    RegName::b1,
    RegName::a0,
    RegName::a1
> {};
struct Abl : RegOprand<
    RegName::b0l,
    RegName::b1l,
    RegName::a0l,
    RegName::a1l
> {};
struct Abh : RegOprand<
    RegName::b0h,
    RegName::b1h,
    RegName::a0h,
    RegName::a1h
> {};
struct Abe : RegOprand<
    RegName::b0e,
    RegName::b1e,
    RegName::a0e,
    RegName::a1e
> {};
struct Ablh : RegOprand<
    RegName::b0l,
    RegName::b0h,
    RegName::b1l,
    RegName::b1h,
    RegName::a0l,
    RegName::a0h,
    RegName::a1l,
    RegName::a1h
> {};
struct RnOld : RegOprand<
    RegName::r0,
    RegName::r1,
    RegName::r2,
    RegName::r3,
    RegName::r4,
    RegName::r5,
    RegName::r7,
    RegName::y0
> {};
struct Rn : RegOprand<
    RegName::r0,
    RegName::r1,
    RegName::r2,
    RegName::r3,
    RegName::r4,
    RegName::r5,
    RegName::r6,
    RegName::r7
> {};
struct R45 : RegOprand<
    RegName::r4,
    RegName::r5
> {};
struct R0123 : RegOprand<
    RegName::r0,
    RegName::r1,
    RegName::r2,
    RegName::r3
> {};
struct ArArpSttMod : RegOprand<
    RegName::ar0,
    RegName::ar1,
    RegName::arp0,
    RegName::arp1,
    RegName::arp2,
    RegName::arp3,
    RegName::undefine,
    RegName::undefine,
    RegName::stt0,
    RegName::stt1,
    RegName::stt2,
    RegName::undefine,
    RegName::mod0,
    RegName::mod1,
    RegName::mod2,
    RegName::mod3
> {};
struct ArArp : RegOprand<
    RegName::ar0,
    RegName::ar1,
    RegName::arp0,
    RegName::arp1,
    RegName::arp2,
    RegName::arp3,
    RegName::undefine,
    RegName::undefine
> {};
struct SttMod : RegOprand<
    RegName::stt0,
    RegName::stt1,
    RegName::stt2,
    RegName::undefine,
    RegName::mod0,
    RegName::mod1,
    RegName::mod2,
    RegName::mod3
> {};
struct Ar : RegOprand<
    RegName::ar0,
    RegName::ar1
> {};
struct Arp : RegOprand<
    RegName::arp0,
    RegName::arp1,
    RegName::arp2,
    RegName::arp3
> {};

struct SwapTypes : Oprand<4> {};

enum class StepValue {
    Zero,
    Increase,
    Decrease,
    PlusStep,
    Increase2,
    Decrease2,
};

using StepZIDS = EnumOprand<StepValue,
    StepValue::Zero,
    StepValue::Increase,
    StepValue::Decrease,
    StepValue::PlusStep
>;

struct ArRn2 : Oprand<1> {};
struct ArRn4 : Oprand<2> {};
struct ArStep2 : Oprand<1> {};
struct ArStep4 : Oprand<2> {};
struct ArpRn2 : Oprand<1> {};
struct ArpRn4 : Oprand<2> {};
struct ArpStep2 : Oprand<1> {};
struct ArpStep4 : Oprand<2> {};

struct Address18_2 : Oprand<2> {};
struct Address18_16 : Oprand<16> {};

struct Address16 : Oprand<16> {};
struct RelAddr7 : Oprand<7> {};

template <unsigned bits>
struct Imm : Oprand<bits> {
    u16 Value() const {
        return this->storage;
    }
};

template <unsigned bits>
struct Imms : Oprand<bits> {
    static constexpr unsigned sign_bit = bits - 1;
    static constexpr u16 sign_ext_mask = 0xFFFF - ((1 << sign_bit) - 1);
    s16 Value() const {
        return (s16)(this->storage | ((this->storage & (1 << sign_bit)) ? sign_ext_mask : 0));
    }
};

struct Imm2 : Imm<2> {};
struct BitNo : Imm<4> {};
struct Imm4 : Imm<4> {};
struct Imm5 : Imm<5> {};
struct Imm5s : Imms<5> {};
struct Imm6s : Imms<6> {};
struct Imm7 : Imm<7> {};
struct Imm7s : Imms<7> {};
struct Imm8 : Imm<8> {};
struct Imm8s : Imms<8> {};
struct Imm9 : Imm<9> {};
struct Imm16 : Imm<16> {};

//struct MemRn : Rn {};
//struct ProgMemRn : Rn {};
//struct ProgMemAxl : Axl {};
//struct ProgMemAx : Ax {};
// struct ProgMemAx_
struct MemImm8 : Imm8 {};
struct MemImm16 : Imm16 {};
struct MemR7Imm7s : Imms<7> {};
struct MemR7Imm16 : Imm16 {};


enum class AlmOp {
    Or,
    And,
    Xor,
    Add,
    Tst0,
    Tst1,
    Cmp,
    Sub,
    Msu,
    Addh,
    Addl,
    Subh,
    Subl,
    Sqr,
    Sqra,
    Cmpu,

    EnumEnd
};

using Alm = EnumAllOprand<AlmOp>;

enum class AluOp {
    Or,
    And,
    Xor,
    Add,
    ReservedA,
    ReservedB,
    Cmp,
    Sub,

    EnumEnd
};

using Alu = EnumAllOprand<AluOp>;

enum class AlbOp {
    Set,
    Rst,
    Chng,
    Addv,
    Tst0,
    Tst1,
    Cmpv,
    Subv,

    EnumEnd
};

using Alb = EnumAllOprand<AlbOp>;

enum class ModaOp {
    Shr,
    Shr4,
    Shl,
    Shl4,
    Ror,
    Rol,
    Clr,
    Reserved,
    Not,
    Neg,
    Rnd,
    Pacr,
    Clrr,
    Inc,
    Dec,
    Copy,

};

using Moda4 = EnumOprand<ModaOp,
    ModaOp::Shr,
    ModaOp::Shr4,
    ModaOp::Shl,
    ModaOp::Shl4,
    ModaOp::Ror,
    ModaOp::Rol,
    ModaOp::Clr,
    ModaOp::Reserved,
    ModaOp::Not,
    ModaOp::Neg,
    ModaOp::Rnd,
    ModaOp::Pacr,
    ModaOp::Clrr,
    ModaOp::Inc,
    ModaOp::Dec,
    ModaOp::Copy
>;

using Moda3 = EnumOprand<ModaOp,
    ModaOp::Shr,
    ModaOp::Shr4,
    ModaOp::Shl,
    ModaOp::Shl4,
    ModaOp::Ror,
    ModaOp::Rol,
    ModaOp::Clr,
    ModaOp::Clrr
>;

enum class CondValue {
    True,
    Eq,
    Neq,
    Gt,
    Ge,
    Lt,
    Le,
    Nn,
    C,
    V,
    E,
    L,
    Nr,
    Niu0,
    Iu0,
    Iu1,

    EnumEnd
};

using Cond = EnumAllOprand<CondValue>;

struct BankFlags : Oprand<6>{};
