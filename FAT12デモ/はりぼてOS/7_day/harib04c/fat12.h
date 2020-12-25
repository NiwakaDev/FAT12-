#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<sys/stat.h>
#pragma pack(1)
typedef struct _BPB{
    unsigned char BS_JmpBoot[3];
    unsigned char BS_OEMName[8];
    unsigned short BPB_BytePerSec;
    unsigned char BPB_SecPerClus;
    unsigned short BPB_RsvdSecCnt;
    unsigned char BPB_NumFATs;
    unsigned short BPB_RootEntCnt;
    unsigned short BPB_TotSec16;
    unsigned char BPB_Media;
    unsigned short BPB_FATSz16;
    unsigned short BPB_SecPerTrk;
    unsigned short BPB_NumHeads;
    unsigned int BPB_HiddSec;
    unsigned int BPB_TotSec32;
}BPB;

//ディレクトリ構造体
typedef struct _DirEntry{
    unsigned char DIR_Name[11];
    unsigned char DIR_Attr;
    unsigned char DIR_NTRes;
    unsigned char DIR_CrtTimeTenth;
    unsigned short DIR_CrtTime;
    unsigned short DIR_CrtDate;
    unsigned short DIR_LstAccDate;
    unsigned short DIR_FstClusHI;
    unsigned short DIR_WrtTime;
    unsigned short DIR_WrtDate;
    unsigned short DIR_FstClusLO;
    unsigned int DIR_FileSize;
}DirEntry;
#define FILE_SIZE 1474560
#define START_FILE_REGION 16896 //ファイル領域の開始番地(オフセット)
#define START_DIR_REGION  9728  //ディレクトリ領域の開始番地(オフセット)
#define START_FAT_REGION  5120  //FAT領域の開始番地(オフセット)
#define CLUSTER 512
#pragma pack()