#include "fat12.h"

void ClearBuff(char *buff){
    int i;
    for(i=0; i<FILE_SIZE; i++){
        *(buff+i) = 0;
    }
}

void SetBootSector(char *buff, char *filename){
    BPB bpb;
    FILE *fp;
    char *boot_buff=malloc(512);
    int i;

    fp = fopen(filename, "rb");
    fread(boot_buff, 1, 512, fp);
    for(i=0; i<512; i++){
        *(buff+i) = *(boot_buff+i);
    }
    return;
}

long GetFileSize(char *file)
{
    struct stat statBuf;

    if (stat(file, &statBuf) == 0)
        return statBuf.st_size;

    return -1;
}

unsigned int CalClusterNumber(int file_size){
    unsigned int quotient;  //商
    unsigned int remainder;  //余り

    quotient  = file_size/512;
    remainder = file_size%512;
    if(remainder>=1){
        quotient += 1;
    }

    return quotient;
}

void SetDirEntry(char *buff, char *filename){
    char* dir_entry = malloc(sizeof(DirEntry));
    DirEntry *dir_entry_pointer = (DirEntry*)dir_entry;

    strcpy(dir_entry, filename);
    dir_entry_pointer->DIR_FstClusLO = 0x0002;//開始クラスタ番号

    int i;

    for(i=0; i<sizeof(DirEntry); i++){
        *(buff+START_DIR_REGION+i) = *(dir_entry+i);
    }
    return;
}

//FAT領域をセットする
void SetFat(char *buff, char *filename){
    long file_size = GetFileSize(filename);
    unsigned int number_of_cluster = CalClusterNumber(file_size);
    unsigned short fat12_region[3072];
    unsigned short cluster_number;
    int i, j;
    FILE *fp;
    short a1;
    short a2;

    char low;
    char middle;
    char high;

    char *os_buff=malloc(file_size);

    ClearBuff(os_buff);
    fp = fopen(filename, "rb");
    if(fp==NULL){
        printf("%sは存在しません。\n", filename);
        exit(0);
    }
    fread(os_buff, 1, file_size, fp);
    for(i=0; i<3072; i++){
        fat12_region[i] = 0;
    }
    fat12_region[0] = 0xff0;//fat12仕様
    fat12_region[1] = 0xfff;//fat12仕様

    for(i=0; i<number_of_cluster-1; i++){
        fat12_region[2+i] = 0x0002+(i+1);
    }
    fat12_region[2+number_of_cluster-1] = 0x0fff;//ファイルの終わり
    j=0;
    //FAT用にfat配列を圧縮する。
    for(i=0; i < 3071; i+=2){
        a1 = fat12_region[i];
        a2 = fat12_region[i+1];

        low    = (char)((a1) & 0x00FF);
        middle = (char)((((a1>>8)&0x000F) | ((a2<<4)&0x00F0)) & 0x00FF);
        high   = (char)((a2>>4) & 0x00FF);
        *(buff+START_FAT_REGION+j+0) = low;
        *(buff+START_FAT_REGION+j+1) = middle;
        *(buff+START_FAT_REGION+j+2) = high;
        j += 3;
    }
    cluster_number = 0x002;

    //ディスクイメージにファイルを書き込む
    for(i=0; i<number_of_cluster; i++){
        for(j=0; j<512; j++){
            *(buff+START_FILE_REGION+512*i+j) = *(os_buff+512*i+j);
        }
    }
}

int main(int argc, char **arg){
    char *filename = arg[1];
    char *os_name   = arg[2];
    char *ipl_name = arg[3];

    FILE *fp;
    char *buff;
    int i;

    if(argc < 4){
        printf("実行時に、ファイル名を入力してください。\n");
        exit(0);
    }
    if(strlen(filename)>12){
        printf("ファイル名は拡張子含めて12文字までにしてください。\n");
        exit(0);
    }
    //FILE_SIZEbyte分の容量を用意する。
    buff = malloc(FILE_SIZE);
    ClearBuff(buff);

    fp = fopen(filename, "wb+");

    SetBootSector(buff, ipl_name);
    SetDirEntry(buff, os_name);
    SetFat(buff, os_name);
    fwrite(buff, 1, FILE_SIZE, fp);
    fclose(fp);
    
    printf("フォーマットしました。\n");

    return 0;
}