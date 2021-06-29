#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#define ROR(x,y) ((unsigned)(x) >> (y) | (unsigned)(x) << (32 - (y)))
#define ROL(x,y) ((unsigned)(x) << (y) | (unsigned)(x) >> (32 - (y)))

unsigned long qword_30A0;
unsigned long qword_30A4;
unsigned long qword_30A8;
unsigned long qword_30AC;

unsigned int dword_32B0; // [15]
unsigned int dword_32AC; // [14]
unsigned int dword_30D7; // [13] <==> 0x2137
unsigned int dword_30BD; // [12] <==> 0x7a69
unsigned int dword_32A8; // [11]
unsigned int dword_30DC; // [10] <==> 0x1234567
unsigned int dword_32A4; // [9]
unsigned int dword_32A0; // [8]

unsigned int dword_3280; // [0]
unsigned int dword_3284; // [1]
unsigned int dword_3288; // [2]
unsigned int dword_328C; // [3]
unsigned int dword_3290; // [4]
unsigned int dword_3294; // [5]
unsigned int dword_3298; // [6]
unsigned int dword_329C; // [7]

typedef struct {
    char hardness_level[4];
    uint size;
    char name [256];
    char cyphered_key[32];
} file_info_t;
file_info_t file;


void sub_FC0(uint param_1)
{
    param_1 = param_1 ^ qword_30A0;
    qword_30A0 = qword_30A4;
    qword_30A4 = qword_30A8;
    qword_30A8 = qword_30AC;
    qword_30AC = ROR(param_1, 6);
}
uint inv_sub_1000(uint param_2) {
    uint uVar1 = ROL(qword_30AC, 6);
    qword_30AC = qword_30A8;
    qword_30A8 = qword_30A4;
    qword_30A4 = qword_30A0;
    uint param_1 = uVar1 - qword_30A8 * 0x2137;
    qword_30A0 = param_2 ^ param_1;
    return param_1;
}
uint sub_1000(uint param_1) {
    uint uVar1;
    param_1 = param_1 ^ qword_30A0;
    uVar1 = qword_30A8 * 0x2137 + param_1;
    qword_30A0 = qword_30A4;
    qword_30A4 = qword_30A8;
    qword_30A8 = qword_30AC;
    qword_30AC = ROR(uVar1, 6);
    return param_1;
}

uint sub_30B0(uint param_1) {
    uint uVar1;
    param_1 = param_1 ^ qword_30A0;
    uVar1 = qword_30A8 * dword_30D7 + param_1;
    qword_30A0 = qword_30A8 * dword_30BD + qword_30A4;
    qword_30A4 = qword_30A8;
    qword_30A8 =qword_30A8 * dword_30DC + qword_30AC;
    qword_30AC = ROR(uVar1, 6);
    return param_1;
}
int sub_3100(file_info_t* file_info) {
    int *f_name;
    int *c_key;
    FILE *stream;
    unsigned int param_scanfed;
    unsigned v6;
    uint pid;
    char s[256];
    char pid_str[1032];

    f_name = (int *)file_info->name;
    c_key = (int *)file_info->cyphered_key;
    for (int i = 0; &f_name[i] != c_key; i++) {
        sub_FC0(f_name[i]);
    }
    stream = fopen("/proc/self/status", "r");
    if ( !stream ) {
        perror("status");
        exit(1);
    }
    while ( 1 ){
        param_scanfed = fscanf(stream, "%[^:]: %s ", s, pid_str);
        if ( param_scanfed == -1 )
            break;
        if ( param_scanfed != 2 )
        {
            fprintf(stderr, "%d? umm what?\n", param_scanfed);
            exit(1);
        }
        v6 = strlen(s);
        if ( s[v6 - 3] == 80 && s[v6 - 2] == 105 && s[v6 - 1] == 100 ) // == "Pid"
        {
            sscanf(pid_str, "%u", &pid);
            sub_FC0(pid);
        }
    }
    return fclose(stream);
}

bool char4_eq_str(const char s1[4], const char* s2) {
    return s1[0] == s2[3] && s1[1] == s2[2] && s1[2] == s2[1] && s1[3] == s2[0];
}

int main(int argc, char *argv[]) {
    FILE *stream;
    unsigned int ret_val;
    FILE *decrypted_stream;
    unsigned int bytes_read;
    int v10;
    const char *hardness_level_str;
    uint v13;
    char key_part[8];

    char* key;
    char* easy = "\vT  ";
    char* hard = "  \vT";

    if ( argc <= 1 ) {
        ret_val = 0;
        printf("usage: %s <input filename> [<key>]\n", *argv);
    } else {
        stream = fopen(argv[1], "rb");
        if (stream) {
            memset(&file, 0, 0x128uLL);
            if (fread(&file, 0x128uLL, 1uLL, stream) == 1) { // 296 bytes
                if (char4_eq_str(file.hardness_level, easy) == 1 || char4_eq_str(file.hardness_level, hard) == 1) {
                    if ( argc == 2) {
                        hardness_level_str = "easy";
                        if (char4_eq_str(file.hardness_level, easy) != 1)
                            hardness_level_str = "hard";
                        ret_val = 0;
                        printf("%s mode file, original name %.256s\n", hardness_level_str, file.name);

                        qword_30A0 = getpid();
                        qword_30A4 = getppid();
                        qword_30A8 = 2589550373;
                        qword_30AC = 1496068438;

                        sub_3100(&file);

                        // WITHOUT KEY
                        dword_32B0 =  *((uint*)file.cyphered_key) ^ qword_30A0;
                        dword_3280 = sub_1000(dword_32B0);

                        dword_32AC = *((uint*)file.cyphered_key+1) ^ qword_30A0;
                        dword_3284 = sub_1000(dword_32AC);

                        dword_30D7 =  *((uint*)file.cyphered_key+2) ^ qword_30A0;
                        dword_3288 = sub_1000(dword_30D7);

                        dword_30BD =  *((uint*)file.cyphered_key+3) ^ qword_30A0;
                        dword_328C = sub_1000(dword_30BD);

                        dword_32A8 = *((uint*)file.cyphered_key+4) ^ qword_30A0;
                        dword_3290 = sub_1000(dword_32A8);

                        dword_30DC = *((uint*)file.cyphered_key+5) ^ qword_30A0;
                        dword_3294 = sub_1000(dword_30DC);

                        dword_32A4 = *((uint*)file.cyphered_key+6) ^ qword_30A0;
                        dword_3298 = sub_1000(dword_32A4);

                        dword_32A0 = *((uint*)file.cyphered_key+7) ^ qword_30A0;
                        dword_329C = sub_1000(dword_32A0);

                        goto LABEL_DECRYPTING;
                    } else {
                        key = argv[2];
                        if (strlen(key) == 64) {
                            // dont loop
                            /* do
                                {
                                    v6 = sub_1000(*v5);
                                    *(v5 - 1) = v6;
                                }
                                while ( v5 != &dword_3254 );*/
                            // but update variables that would be changed inside loop

                            //qword_30A0; // will be changed by getpid anyway
                            //qword_30A4; // will be changed by getppid anyway
                            qword_30A8 = 2589550373; // x/u 0x5555555570a8
                            qword_30AC = 1496068438; // x/u 0x5555555570ac

                            qword_30A0 = getpid();
                            qword_30A4 = getppid();
                            sub_3100(&file);

                            memcpy(key_part, key, 8);
                            sscanf(key_part, "%x", &dword_32B0);
                            memcpy(key_part, &key[1*8], 8);
                            sscanf(key_part, "%x", &dword_32AC);
                            memcpy(key_part, &key[2*8], 8);
                            sscanf(key_part, "%x", &dword_30D7);
                            memcpy(key_part, &key[3*8], 8);
                            sscanf(key_part, "%x", &dword_30BD);
                            memcpy(key_part, &key[4*8], 8);
                            sscanf(key_part, "%x", &dword_32A8);
                            memcpy(key_part, &key[5*8], 8);
                            sscanf(key_part, "%x", &dword_30DC);
                            memcpy(key_part, &key[6*8], 8);
                            sscanf(key_part, "%x", &dword_32A4);
                            memcpy(key_part, &key[7*8], 8);
                            sscanf(key_part, "%x", &dword_32A0);
                            dword_3280 = sub_1000(dword_32B0);
                            dword_3284 = sub_1000(dword_32AC);
                            dword_3288 = sub_1000(dword_30D7);
                            dword_328C = sub_1000(dword_30BD);
                            dword_3290 = sub_1000(dword_32A8);
                            dword_3294 = sub_1000(dword_30DC);
                            dword_3298 = sub_1000(dword_32A4);
                            dword_329C = sub_1000(dword_32A0);

                            ret_val = !(*((uint*)file.cyphered_key) == dword_3280 &&
                            *((uint*)file.cyphered_key+1) == dword_3284 &&
                            *((uint*)file.cyphered_key+2) == dword_3288 &&
                            *((uint*)file.cyphered_key+3) == dword_328C &&
                            *((uint*)file.cyphered_key+4) == dword_3290 &&
                            *((uint*)file.cyphered_key+5) == dword_3294 &&
                            *((uint*)file.cyphered_key+6) == dword_3298 &&
                            *((uint*)file.cyphered_key+7) == dword_329C);

                            if ( ret_val ) {
                                ret_val = 1;
                                fwrite("wrong key\n", 1uLL, 0xAuLL, stderr);
                            } else {
                                LABEL_DECRYPTING:
                                fwrite("OK, decrypting...\n", 1uLL, 0x12uLL, stderr);
                                decrypted_stream = fopen(file.name, "wb");
                                if (decrypted_stream) {
                                    bytes_read = 0;
                                    if (file.size) {
                                        while (1) {
                                            if ( fread(&v13, 4uLL, 1uLL, stream) != 1 ) {
                                                perror("fread");
                                                goto LABEL_35;
                                            }
                                            if (char4_eq_str(file.hardness_level, easy) == 1)
                                                v13 = sub_1000(v13);
                                            else
                                                v13 = sub_30B0(v13);

                                            v10 = file.size - bytes_read; // bytes yet to read
                                            if (v10 > 4) v10 = 4; // min(4, bytes_yet_to_read)
                                            
                                            if ( fwrite(&v13, v10, 1uLL, decrypted_stream) != 1 )
                                                break;
                                            bytes_read += 4;
                                            if (file.size <= bytes_read)
                                                goto LABEL_33;
                                        }
                                        perror("fwrite");
                                        LABEL_35:
                                        ret_val = 1;
                                    }
                                    else
                                    {
                                        LABEL_33:
                                        fclose(stream);
                                        fclose(decrypted_stream);
                                    }
                                }
                                else
                                {
                                    ret_val = 1;
                                    perror("open");
                                }
                            }
                        }
                        else
                        {
                            ret_val = 1;
                            fwrite("wrong key length\n", 1uLL, 0x11uLL, stderr);
                        }
                    }
                }
                else
                {
                    ret_val = 1;
                    fwrite("unrecognized file\n", 1uLL, 0x12uLL, stderr);
                }
            }
            else
            {
                ret_val = 1;
                perror("fread");
            }
        }
        else
        {
            ret_val = 1;
            perror("fopen");
        }
    }
    return ret_val;
}