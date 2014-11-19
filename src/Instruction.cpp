#include "Instruction.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>
#include <sstream>

static const std::string TAG = "instruction";

static void decodeItype(UWord instruction, Instruction& out);
static void decodeRtype(UWord instruction, Instruction& out);
static void decodeJtype(UWord instruction, Instruction& out);
static void setRegisterTypes(Instruction& out);

Instruction::Instruction()
  : name(InstructionName::NOP), type(FunctionalUnitType::Integer),
    rd(RegisterID::NONE), rs1(RegisterID::NONE), rs2(RegisterID::NONE),
    immediate(0xffffffff)
{
}

StrongInstructionPtr Instruction::decode(UWord instruction)
{
  StrongInstructionPtr out(new Instruction);

  //logger->debug(TAG) << "Decoding " << util::hex<UWord> << instruction;
  switch (getEncodingType(instruction >> (31 - 5)))
  {
  case InstructionEncodingType::Itype:
    decodeItype(instruction, *out);
    break;

  case InstructionEncodingType::Rtype:
    decodeRtype(instruction, *out);
    break;

  case InstructionEncodingType::Jtype:
    decodeJtype(instruction, *out);
    break;
  }

  setRegisterTypes(*out);
  return out;
}

std::string Instruction::toString() const
{
  std::ostringstream os;
  os << name << " rd=" << rd << " rs1=" << rs1 << " rs2=" << rs2 
    << " imm=" << immediate;
  return os.str();
}

void decodeItype(UWord instruction, Instruction& out)
{
  //logger->debug(TAG, "Decoding as Itype");
  out.name = getName(instruction >> (31 - 5));
  out.type = getInstructionType(out.name);
  out.rd.index = (instruction >> (31 - 15)) & 0x1f;
  out.rs1.index = (instruction >> (31 - 10)) & 0x1f;
  out.immediate = instruction & 0xffff;
}

void decodeRtype(UWord instruction, Instruction& out)
{
  //logger->debug(TAG, "Decoding as Rtype");
  out.name = getName(instruction >> (31 - 5), instruction & 0x3f);
  out.type = getInstructionType(out.name);
  out.rd.index = (instruction >> (31 - 20)) & 0x1f;
  out.rs1.index = (instruction >> (31 - 10)) & 0x1f;
  out.rs2.index = (instruction >> (31 - 15)) & 0x1f;
  out.immediate = 0xffffffff;
}

void decodeJtype(UWord instruction, Instruction& out)
{
  //logger->debug(TAG, "Decoding as Jtype");
  out.name = getName(instruction >> (31 - 5));
  out.type = getInstructionType(out.name);
  out.immediate = instruction & 0x07ffffff;
}

void setRegisterTypes(Instruction& out)
{
  switch (out.name)
  {
    // No registers
  case InstructionName::J:
  case InstructionName::JAL:
  case InstructionName::NOP:
    out.rd = RegisterID::NONE;
    out.rs1 = RegisterID::NONE;
    out.rs2 = RegisterID::NONE;
    break;

    // GPR rs1 only
  case InstructionName::BEQZ:
  case InstructionName::TRAP:
  case InstructionName::JR:
  case InstructionName::JALR:
    out.rd = RegisterID::NONE;
    out.rs1.type = RegisterType::GPR;
    out.rs2 = RegisterID::NONE;
    break;

    // GPR rd and rs1
  case InstructionName::ADDI:
  case InstructionName::LW:
  case InstructionName::SW:
    out.rd.type = RegisterType::GPR;
    out.rs1.type = RegisterType::GPR;
    out.rs2 = RegisterID::NONE;
    break;

    // FPR rd, GPR rs1
  case InstructionName::LF:
  case InstructionName::SF:
  case InstructionName::MOVI2FP:
    out.rd.type = RegisterType::FPR;
    out.rs1.type = RegisterType::GPR;
    out.rs2 = RegisterID::NONE;
    break;

    // GPR rd, FPR rs1
  case InstructionName::MOVFP2I:
    out.rd.type = RegisterType::GPR;
    out.rs1.type = RegisterType::FPR;
    out.rs2 = RegisterID::NONE;
    break;

    // FPR rd and rs1
  case InstructionName::MOVF:
  case InstructionName::CVTF2I:
  case InstructionName::CVTI2F:
    out.rd.type = RegisterType::FPR;
    out.rs1.type = RegisterType::FPR;
    out.rs2 = RegisterID::NONE;
    break;

    // 3x GPR
  case InstructionName::ADD:
  case InstructionName::SUB:
  case InstructionName::AND:
  case InstructionName::OR:
  case InstructionName::XOR:
    out.rd.type = RegisterType::GPR;
    out.rs1.type = RegisterType::GPR;
    out.rs2.type = RegisterType::GPR;
    break;

    // 3x FPR
  case InstructionName::ADDF:
  case InstructionName::SUBF:
  case InstructionName::MULTF:
  case InstructionName::DIVF:
  case InstructionName::MULT:
  case InstructionName::DIV:
    out.rd.type = RegisterType::FPR;
    out.rs1.type = RegisterType::FPR;
    out.rs2.type = RegisterType::FPR;
    break;
  }  
}
