/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    opCodeToString.cpp
 *
 * Module:  Ivory common (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2003
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Reurn a string for a given byte operation code.
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

#include <ivory/exec.h>

const char* opCodeToString(UInt opCode) {
   switch (opCode) {

      case ATD_INS:        return "atd";

      case MOV_L_INS:      return "mov_l";
      case MOV_I_A_INS:    return "mov_i_a";
      case MOV_B8_INS:     return "mov_b8";
      case MOV_B_INS:      return "mov_b";
      case MOV_I_INS:      return "mov_i";
      case MOV_F_INS:      return "mov_f";
      case MOV_D_INS:      return "mov_d";
      case MOV_C_INS:      return "mov_c";
      case MOV_N_INS:      return "mov_n";
      case MOV_P_INS:      return "mov_p";
      case MOV_E_INS:      return "mov_e";
      case MOV_T_INS:      return "mov_t";
      case MOV_R_INS:      return "mov_r";
      case MOV_O_INS:      return "mov_o";
      case MMV_B_INS:      return "mmv_b";
      case LVP_INS:        return "lvp";

      case MCP_INS:        return "mcp";

      case BEQ_P_INS:      return "beq_p";
      case BNE_P_INS:      return "bne_p";

      case CEQ_P_INS:      return "ceq_p";
      case CNE_P_INS:      return "cne_p";

      case BEQ_N_INS:      return "beq_n";
      case BNE_N_INS:      return "bne_n";

      case CEQ_N_INS:      return "ceq_n";
      case CNE_N_INS:      return "cne_n";

      case BEQ_T_INS:      return "beq_t";
      case BNE_T_INS:      return "bne_t";

      case CEQ_T_INS:      return "ceq_t";
      case CNE_T_INS:      return "cne_t";

      case NEG_I_INS:      return "neg_i";
      case ADD_I_INS:      return "add_i";
      case SUB_I_INS:      return "sub_i";
      case MUL_I_INS:      return "mul_i";
      case DIV_I_INS:      return "div_i";
      case MOD_I_INS:      return "mod_i";

      case AND_B_INS:      return "and_b";
      case IOR_B_INS:      return "or_b";
      case XOR_B_INS:      return "xor_b";
      case LSH_B_INS:      return "lsh_b";
      case RSH_B_INS:      return "rsh_b";
      case NOT_B_INS:      return "not_b";

      case CLT_I_INS:      return "clt_i";
      case CLE_I_INS:      return "cle_i";
      case CEQ_I_INS:      return "ceq_i";
      case CNE_I_INS:      return "cne_i";
      case CGE_I_INS:      return "cge_i";
      case CGT_I_INS:      return "cgt_i";

      case BLT_I_INS:      return "blt_i";
      case BLE_I_INS:      return "ble_i";
      case BEQ_I_INS:      return "beq_i";
      case BNE_I_INS:      return "bne_i";
      case BGE_I_INS:      return "bge_i";
      case BGT_I_INS:      return "bgt_i";

      case STR_I_INS:      return "str_i";
      case STR_IC_INS:     return "str_ic";

      case NEG_F_INS:      return "neg_f";
      case ADD_F_INS:      return "add_f";
      case SUB_F_INS:      return "sub_f";
      case MUL_F_INS:      return "mul_f";
      case DIV_F_INS:      return "div_f";

      case CLT_F_INS:      return "clt_f";
      case CLE_F_INS:      return "cle_f";
      case CEQ_F_INS:      return "ceq_f";
      case CNE_F_INS:      return "cne_f";
      case CGE_F_INS:      return "cge_f";
      case CGT_F_INS:      return "cgt_f";

      case BLT_F_INS:      return "blt_f";
      case BLE_F_INS:      return "ble_f";
      case BEQ_F_INS:      return "beq_f";
      case BNE_F_INS:      return "bne_f";
      case BGE_F_INS:      return "bge_f";
      case BGT_F_INS:      return "bgt_f";

      case INT_B8_INS:     return "int_b8";
      case INT_B_INS:      return "int_b";
      case INT_F_INS:      return "int_f";
      case DBL_F_INS:      return "dbl_f";

      case NEG_D_INS:      return "neg_d";
      case ADD_D_INS:      return "add_d";
      case SUB_D_INS:      return "sub_d";
      case MUL_D_INS:      return "mul_d";
      case DIV_D_INS:      return "div_d";

      case CLT_D_INS:      return "clt_d";
      case CLE_D_INS:      return "cle_d";
      case CEQ_D_INS:      return "ceq_d";
      case CNE_D_INS:      return "cne_d";
      case CGE_D_INS:      return "cge_d";
      case CGT_D_INS:      return "cgt_d";

      case BLT_D_INS:      return "blt_d";
      case BLE_D_INS:      return "ble_d";
      case BEQ_D_INS:      return "beq_d";
      case BNE_D_INS:      return "bne_d";
      case BGE_D_INS:      return "bge_d";
      case BGT_D_INS:      return "bgt_d";

      case INT_D_INS:      return "int_d";
      case FLT_D_INS:      return "flt_d";
      case STR_D_INS:      return "str_d";
      case STR_DC_INS:     return "str_dc";

      case SUB_U_INS:      return "sub_u";

      case CLT_O_INS:      return "clt_o";
      case CLE_O_INS:      return "cle_o";
      case CEQ_O_INS:      return "ceq_o";
      case CNE_O_INS:      return "cne_o";
      case CGE_O_INS:      return "cge_o";
      case CGT_O_INS:      return "cgt_o";

      case BF_INS:         return "bf";
      case BT_INS:         return "bt";

      case BLT_O_INS:      return "blt_o";
      case BLE_O_INS:      return "ble_o";
      case BEQ_O_INS:      return "beq_o";
      case BNE_O_INS:      return "bne_o";
      case BGE_O_INS:      return "bge_o";
      case BGT_O_INS:      return "bgt_o";

      case CLT_C_INS:      return "clt_c";
      case CLE_C_INS:      return "cle_c";
      case CEQ_C_INS:      return "ceq_c";
      case CNE_C_INS:      return "cne_c";
      case CGE_C_INS:      return "cge_c";
      case CGT_C_INS:      return "cgt_c";

      case BLT_C_INS:      return "blt_c";
      case BLE_C_INS:      return "ble_c";
      case BEQ_C_INS:      return "beq_c";
      case BNE_C_INS:      return "bne_c";
      case BGE_C_INS:      return "bge_c";
      case BGT_C_INS:      return "bgt_c";

      case RET_INS:        return "ret";
      case RTN_INS:        return "rtn";
      case EXIT_INS:       return "exit";

      case ADD_P_INS:      return "add_p";

      case ADD_SP_INS:     return "add_sp";
      case SUB_SP_INS:     return "sub_sp";

      case TAG_INS:        return "tag";
      case TAG_N_INS:      return "tag_n";
      case TAG_T_INS:      return "tag_t";
      case TAG_I_INS:      return "tag_i";
      case TAG_F_INS:      return "tag_f";
      case TAG_D_INS:      return "tag_d";
      case TAG_S_INS:      return "tag_s";
      case TAG_O_INS:      return "tag_o";
      case TAG_C_INS:      return "tag_c";
      case TAG_B_INS:      return "tag_b";
      case TAG_R_INS:      return "tag_r";

      case CFN_INS:        return "cfn";
      case CFS_INS:        return "cfs";

      case TSC_INS:        return "tsc";
      case EFC_INS:        return "efc";

      case JMP_INS:        return "jmp";

      case MIL_INS:        return "mil";

      case BRA_INS:        return "bra";

#if (GARBAGE_COLLECTION==1)
      case AFD_INS:        return "afd";
#endif

      case CIO_INS:        return "cio";

      case ALC_INS:        return "alc";
      case ACI_INS:        return "aci";
      case ACL_INS:        return "acl";
      case ACL_ENV_INS:    return "acl_env";
      case APA_INS:        return "apa";
      case AIN_INS:        return "ain";

      case ENT_E_INS:      return "ent_e";
      case ENT_P_INS:      return "ent_p";
      case ENT_A_P_INS:    return "ent_a_p";

      case ASC_INS:        return "argCheck";

      case UPD_INS:        return "update";

      case OTY_INS:        return "oty";

      case ETC_INS:        return "etc";

      case BSE_INS:        return "bse";

      case ATB_N_INS:      return "atb_n";
      case ATB_T_INS:      return "atb_t";
      case ATB_I_INS:      return "atb_i";
      case ATB_O_INS:      return "atb_o";
      case ATB_C_INS:      return "atb_c";
      case ATB_S_INS:      return "atb_s";
      case ATB_CS_INS:     return "atb_cs";

      case BNP_INS:        return "bnp";
      case BNN_INS:        return "bnn";

      case EXC_INS:        return "exc";


      case SQZ_INS:        return "sqz";

      case RRL_INS:        return "rrl";

      case ERR_INS:        return "err";
      case TRC_INS:        return "trc";

      case ECP_INS:        return "ecp";
      case TCP_INS:        return "tcp";

      case MAP_N_INS:      return "map_n";
      case MAP_T_INS:      return "map_t";
      case MAP_CL_INS:     return "map_cl";
      case MAP_E_INS:      return "map_e";
      case MAP_S_INS:      return "map_s";
      case MAP_SC_INS:     return "map_sc";

      case LEN_S_INS:      return "len_s";
      case LEN_SC_INS:     return "len_sc";
      case GET_AT_S_INS:   return "get_at_s";
      case GET_AT_SC_INS:  return "get_at_sc";   
      case PUT_AT_S_INS:   return "put_at_s";

      case PCB_INS:        return "pcb";

      case SET_ENV_INS:    return "set_env";

      case RSS_INS:        return "rss";
      case SIZE_OF_INS:    return "size_of";

      case CPA_INS:        return "cpa";
      case GPAV_INS:       return "gaa";
      case PPAV_INS:       return "papa";

#if (GARBAGE_COLLECTION==1)
      case MRK_P_INS:      return "mrk_p";
      case MRK_P_BS_INS:   return "mrk_p_bs";
      case MRK_P_BNS_INS:  return "mrk_p_bns";
      case MRK_E_BS_INS:   return "mrk_e_bs";
      case MRK_E_BNS_INS:  return "mrk_e_bns";
      case MRK_CL_INS:     return "mrk_cl";
      case MRK_S_INS:      return "mrk_s";
#endif

#if (SERIALISATION==1)
      case EXT_CL_INS:     return "ext_cl";
      case INS_CL_INS:     return "ins_cl";
      case EXT_E_INS:      return "ext_e";
      case INS_E_INS:      return "ins_e";
#endif

      default:             return "???";
   }
}
