/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    instructions.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *  Ivory virtual machine instruction codes and adress modes.
 *
 * Modification history:
 *
 *------------------------------------------------------------------------------
 *
 * License: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 *-----------------------------------------------------------------------------
 */

#ifndef IVORY_INSTRUCTIONS_H
#define IVORY_INSTRUCTIONS_H

// Instruction Codes

#define OPCODE_BITS        8
#define OPCODE_OFFSET      0
#define OPCODE_MASK        (((1<<OPCODE_BITS)-1)<<OPCODE_OFFSET)

#define ATD_INS         1     // allocate typr descriptor

#define MOV_E_INS       2     // move Expr

#define MOV_P_INS       3     // move pointer

#define ADD_P_INS       4     // add pointer
#define BEQ_P_INS       5     // branch if equal pointer
#define BNE_P_INS       6     // branch if not equal pointer

#define CEQ_P_INS       7     // compare if equal pointer
#define CNE_P_INS       8     // compare if not equal pointer

#define MOV_N_INS       9     // move name
#define TAG_N_INS       10    // tag name

#define CEQ_N_INS       11    // compare equal name   
#define CNE_N_INS       12    // comapre not equal name

#define BEQ_N_INS       13    // branch if equal name
#define BNE_N_INS       14    // branch if no equal name

#define MOV_T_INS       15    // move type
#define TAG_T_INS       16    // tag type

#define CEQ_T_INS       17    // compare if equal type
#define CNE_T_INS       18    // compare if not equal type

#define BEQ_T_INS       19    // branch if equal type
#define BNE_T_INS       20    // banch if not equal type

#define MOV_B8_INS      21    // move byte

#define MOV_B_INS       215   // move bits

#define MOV_I_INS       22    // move integer
#define TAG_I_INS       23    // tag integer to expr

#define ADD_I_INS       24    // add integer
#define SUB_I_INS       25    // subtract integer
#define MUL_I_INS       26    // multiply integer
#define DIV_I_INS       27    // divide integer
#define NEG_I_INS       28    // negate integer
#define MOD_I_INS       29    // modulus integer

#define AND_B_INS       30    // bitwise and
#define IOR_B_INS       31    // bitwise inclusive or
#define XOR_B_INS       32    // bitwise exclusive or 
#define LSH_B_INS       33    // bitwise left shift
#define RSH_B_INS       34    // bitwise right shift
#define NOT_B_INS       35    // bitwise complement

#define CLT_I_INS       36    // compare less than integer
#define CLE_I_INS       37    // compare less than or equal integer
#define CEQ_I_INS       38    // compare equal integer
#define CNE_I_INS       39    // compare not equal integer
#define CGE_I_INS       40    // compare greater than or equal integer
#define CGT_I_INS       41    // compare greater than equal integer

#define BLT_I_INS       42    // branch if less than integer
#define BLE_I_INS       43    // branch if less than or equal integer
#define BEQ_I_INS       44    // branch if equal integer
#define BNE_I_INS       45    // branch if not equal integer
#define BGE_I_INS       46    // branch if greater than or equal integer
#define BGT_I_INS       47    // branch if greater than integer

#define BLTZ_I_INS      48    // branch if less than zero integer
#define BLEZ_I_INS      49    // branch if less than or equal to zero integer
#define BEQZ_I_INS      50    // branch if equal to zero integer
#define BNEZ_I_INS      51    // branch if not equal to zero integer
#define BGEZ_I_INS      52    // branch if greater than or equal to zero integer
#define BGTZ_I_INS      53    // branch if greater than zero integer

#define STR_I_INS       54    // string to integer
#define STR_IC_INS      55    // string constant to integer

#define MOV_F_INS       56    // move float
#define TAG_F_INS       57    // tag float

#define NEG_F_INS       58    // negate float
#define ADD_F_INS       59    // add float
#define SUB_F_INS       60    // subtract float
#define MUL_F_INS       61    // multiply float
#define DIV_F_INS       62    // divide float

#define CLT_F_INS       63    // compare less than float
#define CLE_F_INS       64    // compare less than or equal float
#define CEQ_F_INS       65    // compare equal float
#define CNE_F_INS       66    // compare not equal float
#define CGE_F_INS       67    // compare greater than or equal float
#define CGT_F_INS       68    // compare greater than equal float

#define BLT_F_INS       69    // branch if less than float
#define BLE_F_INS       70    // branch if less than or equal float
#define BEQ_F_INS       71    // branch if equalfloat
#define BNE_F_INS       72    // branch if not equal float
#define BGE_F_INS       73    // branch if greater than or equal float
#define BGT_F_INS       74    // branch if greater than float

#define INT_B8_INS      216   // integer to byte
#define INT_B_INS       75    // integer to bits
#define INT_F_INS       76    // integer to float
#define DBL_F_INS       77    // double to float

#define MOV_D_INS       78    // move double
#define TAG_D_INS       79    // tag double

#define NEG_D_INS       80    // negate double
#define ADD_D_INS       81    // add double
#define SUB_D_INS       82    // subtract double
#define MUL_D_INS       83    // multiply double
#define DIV_D_INS       84    // divide double

#define CLT_D_INS       85    // compare less than double
#define CLE_D_INS       86    // compare less than or equal double
#define CEQ_D_INS       87    // compare equal double
#define CNE_D_INS       88    // compare not equal double
#define CGE_D_INS       89    // compare greater than or equal double
#define CGT_D_INS       90    // compare greater than equal double

#define BLT_D_INS       91    // branch if less than double
#define BLE_D_INS       92    // branch if less than or equal double
#define BEQ_D_INS       93    // branch if equal double
#define BNE_D_INS       94    // branch if not equal double
#define BGE_D_INS       95    // branch if greater than or equal double
#define BGT_D_INS       96    // branch if greater than double

#define INT_D_INS       97    // integer to double
#define FLT_D_INS       98    // Float to double
#define STR_D_INS       99    // string to double
#define STR_DC_INS      100   // string constant to double

#define MOV_O_INS       101   // move ordinal
#define TAG_O_INS       102   // tag ordinal
#define CLT_O_INS       103   // compare less than ordinal
#define CLE_O_INS       104   // compare less than or equal ordinal
#define CEQ_O_INS       105   // compare equal ordinal
#define CNE_O_INS       106   // compare not equal ordinal
#define CGE_O_INS       107   // compare greater than or equal ordinal
#define CGT_O_INS       108   // compare greater than equal ordinal

#define BLT_O_INS       109   // branch if less than ordinal
#define BLE_O_INS       110   // branch if less than or equal ordinal
#define BEQ_O_INS       111   // branch if equal ordinal
#define BNE_O_INS       112   // branch if not equal ordinal
#define BGE_O_INS       113   // branch if greater than or equal ordinal
#define BGT_O_INS       114   // branch if greater than ordinal

#define BF_INS          115   // branch if true
#define BT_INS          116   // branch if false

#define MOV_C_INS       117   // move character
#define TAG_C_INS       118   // tag character

#define CLT_C_INS       119   // compare less than character
#define CLE_C_INS       120   // compare less than or equal character
#define CEQ_C_INS       121   // compare equal character
#define CNE_C_INS       122   // compare not equal character
#define CGE_C_INS       123   // compare greater than or equal character
#define CGT_C_INS       124   // compare greater than equal character

#define BLT_C_INS       125   // branch if less than character
#define BLE_C_INS       126   // branch if less than or equal character
#define BEQ_C_INS       127   // branch if equal character
#define BNE_C_INS       128   // branch if not equal character
#define BGE_C_INS       129   // branch if greater than or equal character
#define BGT_C_INS       130   // branch if greater than character

#define TSC_INS         131   // tag string constant
#define TAG_S_INS       132   // tag string

#define LEN_S_INS       133   // lengthString
#define LEN_SC_INS      134   // lengthString (string constant)
#define GET_AT_S_INS    135   // getAtString
#define GET_AT_SC_INS   136   // getAtString (string constant)
#define PUT_AT_S_INS    137   // putAtString

#define MMV_B_INS       138   // move multiple bytes

#define TAG_B_INS       139   // tag bytestring

#define MOV_R_INS       140   // move reference
#define TAG_R_INS       141   // tag reference

#define MOV_L_INS       142   // move label
#define MOV_I_A_INS     143   // move i-machine address
#define MIL_INS         144   // move label pair

#define ADD_SP_INS      145   // add stack
#define SUB_SP_INS      146   // subtract stack

#define BRA_INS         147   // branch

#define JMP_INS         148   // jump

#define RET_INS         149   // jump to local continuation popped from stack
#define RTN_INS         150   // jump to continuation popped from stack
#define EXIT_INS        151   // End continuation sequence

#if (GARBAGE_COLLECTION==1)
#define AFD_INS         152   // activation frame descriptor
#endif

#define CIO_INS         153   // cell info of cell

#define ALC_INS         154   // allocate memory
#define ACI_INS         155   // allocate CellInfo
#define ACL_INS         156   // allocate closure in default environment
#define ACL_ENV_INS     157   // allocate closure in specified environment
#define APA_INS         158   // allocate pointer array

#define EFC_INS         159   // expression from cell pointer (Expr)

#define TAG_INS         160   // return register of type -> Exp (*)

#define ENT_E_INS       161   // enter special (Expr)

#define ENT_P_INS       162   // enter cell (pointer)
#define ENT_A_P_INS     163   // enter cell (pointer) at alternative entry
#define ASC_INS         164   // argument satisfaction check

#define UPD_INS         165   // update cell with indirection

#define OTY_INS         166   // object type instruction

#define MCP_INS         167   // move constant pointer

#define PCB_INS         168   // push continuatiuon and branch (BSR!)

#define ETC_INS         169   // expression to cell (Expr)

#define BSE_INS         170   // branch if special (Expr)

#define ATB_N_INS       171   // branch via Name association table
#define ATB_T_INS       172   // branch via Type association table
#define ATB_I_INS       173   // branch via Int  association table
#define ATB_O_INS       174   // branch via Tag  association table
#define ATB_C_INS       175   // branch via Char association table
#define ATB_S_INS       176   // branch via String association table
#define ATB_CS_INS      177   // branch via String constant association table

#define EXC_INS         178   // exception

#define SQZ_INS         179   // squeeze stack

#define RRL_INS         180   // reverse reference list

#define BNP_INS         181   // branch if null pointer
#define BNN_INS         182   // branch if not null pointer

#define ERR_INS         183   // error
#define TRC_INS         184   // trace

#define ECP_INS         185   // error (constant pointer)
#define TCP_INS         186   // trace (constant pointer)

#define SUB_U_INS       187   // subtract UTC

#define MAP_N_INS       188   // map name
#define MAP_T_INS       189   // map type
#define MAP_CL_INS      190   // map closure
#define MAP_E_INS       191   // map expression
#define MAP_S_INS       192   // map string
#define MAP_SC_INS      193   // map string constant

#define SET_ENV_INS     194   // set environment

#define RSS_INS         195   // return structure size

#define SIZE_OF_INS     196   // size (in bytes) of type

#define CPA_INS         197   // copy pointer array
#define GPAV_INS        198   // get pointer array val
#define PPAV_INS        199   // put pointer array val

#if (GARBAGE_COLLECTION==1)
#define MRK_P_INS       200   // mark pointer, return tag 1 if set, 0 otherwise
#define MRK_P_BS_INS    201   // mark pointer, branch if set
#define MRK_P_BNS_INS   202 
#define MRK_E_BS_INS    203   // mark expression (pointer), branch if set
#define MRK_E_BNS_INS   204 

#define MRK_CL_INS      205   // mark closure
#define MRK_S_INS       206   // mark string
#endif

#if (SERIALISATION==1)
#define EXT_CL_INS      207   // extract closure/cell
#define INS_CL_INS      208   // insert closure/cell
#define EXT_E_INS       209   // extract expression
#define INS_E_INS       210   // insert expression
#endif

#define CFN_INS         211   // construct atomic sum from constructor name
#define CFS_INS         212   // construct atomic sum from constructor string
#define AIN_INS         213   // allocate indirection cell
#define LVP_INS         214   // left value pointer

#define NXT_INS         217

// All instruction codes >= NEXT_INS and <= ILG_INS are illegal

#define ILG_INS         255   // illegal

// Address modes

#define ADDR_MODE_BITS        3
#define ADDR_MODE_OFFSET      5
#define ADDR_MODE_MASK        (((1<<ADDR_MODE_BITS)-1)<<ADDR_MODE_OFFSET)

#define REG_ID_BITS           (8-ADDR_MODE_BITS)
#define REG_ID_OFFSET         0
#define REG_ID_MASK           (((1<<REG_ID_BITS)-1)<<REG_ID_OFFSET)

#define IMM_VAL_BITS          (8-ADDR_MODE_BITS)
#define IMM_VAL_OFFSET        0
#define IMM_VAL_MASK          (((1<<IMM_VAL_BITS)-1)<<IMM_VAL_OFFSET)
#define IMM_VAL_SIGN_MASK     (1<<(IMM_VAL_BITS-1))
#define IMM_VAL_SIGN_EXT_MASK (0xff^IMM_VAL_MASK)

#define EXT_ADDR_MODE_BITS    (8-ADDR_MODE_BITS)
#define EXT_ADDR_MODE_OFFSET  0
#define EXT_ADDR_MODE_MASK    (((1<<EXT_ADDR_MODE_BITS)-1)<<EXT_ADDR_MODE_OFFSET)

#define IMM_ADDR_MODE         0  // Immediate data
#define MEM_ADDR_MODE         1  // Memory relative
#define STACK_ADDR_MODE       2	// Stack relative
#define CELL_ADDR_MODE        3	// Cell relative
#define RACK_ADDR_MODE        4	// Rack relative
#define REG_ADDR_MODE         5	// Register
#define IND_ADDR_MODE         6	// Indirect
#define EXT_ADDR_MODE         7	// Extended address mode

#define IMM_1_EXT_ADDR_MODE      0  // Immediate data    (8  bits)
#define IMM_2_EXT_ADDR_MODE      1  // Immediate data    (16 bits)
#define IMM_4_EXT_ADDR_MODE      2  // Immediate data    (32 bits)
#define IMM_8_EXT_ADDR_MODE      3  // Immediate data    (64 bits)

#define MEM_1_EXT_ADDR_MODE      4  // Memory relative   (8  bits)
#define MEM_2_EXT_ADDR_MODE      5  // Memory relative   (16 bits)
#define MEM_4_EXT_ADDR_MODE      6  // Memory relative   (32 bits)
#define MEM_8_EXT_ADDR_MODE      7  // Memory relative   (64 bits)

#define STACK_1_EXT_ADDR_MODE    8  // Stack relative (8  bit signed offset)
#define STACK_2_EXT_ADDR_MODE    9  // Stack relative (16 bit signed offset)
#define STACK_4_EXT_ADDR_MODE    10 // Stack relative (32 bit signed offset)
#define STACK_8_EXT_ADDR_MODE    11 // Stack relative (64 bit signed offset)

#define CELL_1_EXT_ADDR_MODE     12 // Cell relative (8  bit unsigned offset)
#define CELL_2_EXT_ADDR_MODE     13 // Cell relative (16 bit unsigned offset)
#define CELL_4_EXT_ADDR_MODE     14 // Cell relative (32 bit unsigned offset)
#define CELL_8_EXT_ADDR_MODE     15 // Cell relative (64 bit unsigned offset)

#define FRAME_1_EXT_ADDR_MODE    16 // Frame relative (8  bit unsigned offset)
#define FRAME_2_EXT_ADDR_MODE    17 // Frame relative (16 bit unsigned offset)
#define FRAME_4_EXT_ADDR_MODE    18 // Frame relative (32 bit unsigned offset)
#define FRAME_8_EXT_ADDR_MODE    19 // Frame relative (64 bit unsigned offset)

#define RACK_1_EXT_ADDR_MODE     20 // Rack relative (8  bit unsigned offset)
#define RACK_2_EXT_ADDR_MODE     21 // Rack relative (16 bit unsigned offset)
#define RACK_4_EXT_ADDR_MODE     22 // Rack relative (32 bit unsigned offset)
#define RACK_8_EXT_ADDR_MODE     23 // Rack relative (64 bit unsigned offset)

#define IND_1_EXT_ADDR_MODE      24 // Indirect (8  bit unsigned offset)
#define IND_2_EXT_ADDR_MODE      25 // Indirect (16 bit unsigned offset)
#define IND_4_EXT_ADDR_MODE      26 // Indirect (32 bit unsigned offset)
#define IND_8_EXT_ADDR_MODE      27 // Indirect (64 bit unsigned offset)

#define INDEXED_1_EXT_ADDR_MODE  28 // Indexed data      (8  bit unsigned offset)
#define INDEXED_2_EXT_ADDR_MODE  29 // Indexed data      (16 bit unsigned offset)
#define INDEXED_4_EXT_ADDR_MODE  30 // Indexed data      (32 bit unsigned offset)
#define INDEXED_8_EXT_ADDR_MODE  31 // Indexed data      (64 bit unsigned offset)


#define IMM_ADDR_MODE_VAL           ((Byte)(IMM_ADDR_MODE<<ADDR_MODE_OFFSET))
#define MEM_ADDR_MODE_VAL           ((Byte)(MEM_ADDR_MODE<<ADDR_MODE_OFFSET))
#define STACK_ADDR_MODE_VAL         ((Byte)(STACK_ADDR_MODE<<ADDR_MODE_OFFSET))
#define CELL_ADDR_MODE_VAL          ((Byte)(CELL_ADDR_MODE<<ADDR_MODE_OFFSET))
#define RACK_ADDR_MODE_VAL          ((Byte)(RACK_ADDR_MODE<<ADDR_MODE_OFFSET))
#define REG_ADDR_MODE_VAL           ((Byte)(REG_ADDR_MODE<<ADDR_MODE_OFFSET))
#define IND_ADDR_MODE_VAL           ((Byte)(IND_ADDR_MODE<<ADDR_MODE_OFFSET))
#define EXT_ADDR_MODE_VAL           ((Byte)(EXT_ADDR_MODE<<ADDR_MODE_OFFSET))

#define IMM_1_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|IMM_1_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define IMM_2_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|IMM_2_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define IMM_4_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|IMM_4_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define IMM_8_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|IMM_8_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))

#define MEM_1_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|MEM_1_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define MEM_2_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|MEM_2_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define MEM_4_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|MEM_4_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define MEM_8_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|MEM_8_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))



#define STACK_1_EXT_ADDR_MODE_VAL   ((Byte)(EXT_ADDR_MODE_VAL|STACK_1_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define STACK_2_EXT_ADDR_MODE_VAL   ((Byte)(EXT_ADDR_MODE_VAL|STACK_2_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define STACK_4_EXT_ADDR_MODE_VAL   ((Byte)(EXT_ADDR_MODE_VAL|STACK_4_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define STACK_8_EXT_ADDR_MODE_VAL   ((Byte)(EXT_ADDR_MODE_VAL|STACK_8_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))

#define CELL_1_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|CELL_1_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define CELL_2_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|CELL_2_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define CELL_4_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|CELL_4_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define CELL_8_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|CELL_8_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))

#define FRAME_1_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|FRAME_1_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define FRAME_2_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|FRAME_2_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define FRAME_4_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|FRAME_4_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define FRAME_8_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|FRAME_8_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))

#define RACK_1_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|RACK_1_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define RACK_2_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|RACK_2_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define RACK_4_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|RACK_4_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define RACK_8_EXT_ADDR_MODE_VAL    ((Byte)(EXT_ADDR_MODE_VAL|RACK_8_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))

#define IND_1_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|IND_1_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define IND_2_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|IND_2_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define IND_4_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|IND_4_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define IND_8_EXT_ADDR_MODE_VAL     ((Byte)(EXT_ADDR_MODE_VAL|IND_8_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))

#define INDEXED_1_EXT_ADDR_MODE_VAL ((Byte)(EXT_ADDR_MODE_VAL|INDEXED_1_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define INDEXED_2_EXT_ADDR_MODE_VAL ((Byte)(EXT_ADDR_MODE_VAL|INDEXED_2_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define INDEXED_4_EXT_ADDR_MODE_VAL ((Byte)(EXT_ADDR_MODE_VAL|INDEXED_4_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))
#define INDEXED_8_EXT_ADDR_MODE_VAL ((Byte)(EXT_ADDR_MODE_VAL|INDEXED_8_EXT_ADDR_MODE<<EXT_ADDR_MODE_OFFSET))

#endif /* IVORY_INSTRUCTIONS_H_DEFINED */