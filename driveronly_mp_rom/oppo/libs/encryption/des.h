/************************************************************************************
** File: - des.h
** VENDOR_EDIT
** Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
** 
** Description: 
**      Add this file for customizing OPPO_CUSTOM partition!
** 
** Version: 1.0
** Date created: 12/12/2012
** Author: ZhiYong.Lin@Plf.Framework
** 
** --------------------------- Revision History: --------------------------------
** 	<author>	                    <data>					<desc>
** 	YinXiang.Zhang@Plf.Framework     				      Init
** 	ZhiYong.Lin@Plf.Framework     12/12/2012      Modify
************************************************************************************/



#ifndef _DES_H
#define _DES_H

#include "stdio.h"   
#include "memory.h"   
#include "time.h"   
#include "stdlib.h"   
      
#define PLAIN_FILE_OPEN_ERROR -1   
#define KEY_FILE_OPEN_ERROR -2   
#define CIPHER_FILE_OPEN_ERROR -3   
#define OK 1  

typedef char ElemType;

int ByteToBit(ElemType ch,ElemType bit[8]);
int BitToByte(ElemType bit[8],ElemType *ch);
int Char8ToBit64(ElemType ch[8],ElemType bit[64]);
int Bit64ToChar8(ElemType bit[64],ElemType ch[8]);
int DES_MakeSubKeys(ElemType key[64],ElemType subKeys[16][48]);
int DES_PC1_Transform(ElemType key[64], ElemType tempbts[56]);
int DES_PC2_Transform(ElemType key[56], ElemType tempbts[48]);
int DES_ROL(ElemType data[56], int time);
int DES_IP_Transform(ElemType data[64]);
int DES_IP_1_Transform(ElemType data[64]);
int DES_E_Transform(ElemType data[48]);
int DES_P_Transform(ElemType data[32]);
int DES_SBOX(ElemType data[48]);
int DES_XOR(ElemType R[48], ElemType L[48],int count);
int DES_Swap(ElemType left[32],ElemType right[32]);
int DES_EncryptBlock(ElemType plainBlock[8], ElemType subKeys[16][48], ElemType cipherBlock[8]);
int DES_DecryptBlock(ElemType cipherBlock[8], ElemType subKeys[16][48], ElemType plainBlock[8]);
int DES_Encrypt_In_File(char *plainFile, char *keyStr,char *cipherFile);
int DES_Decrypt_In_File(char *cipherFile, char *keyStr,char *plainFile);
void* DES_Encrypt_In_Memeory(void *plainFile, long len, char *keyStr, long *enLen);
void* DES_Decrypt_In_Memeory(void *cipherFile, long len, char *keyStr, long *deLen);

#endif