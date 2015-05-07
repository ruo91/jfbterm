/* This file is a part of Ami software, (C) Hwang chi-deok 2001 */

/*

일반 완성형 한글
0xb0a1    => 0x8861
...
0xc8fe    => 0xd3b7

자모
0xa4a1    => 0x
0xa4fe    => 0x

특수문자
0xa1a1    => 0xd931
...
0xacf1    => 0xdef1

*/


#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>

#if defined(__sun__)
#define CD_EUC_KR "ko_KR-euc"
#define CD_JOHAB  "ko_KR-johap92"
#define CD_UTF8   "ko_KR-UTF-8"
#else
#define CD_EUC_KR "EUC-KR"
#define CD_JOHAB  "JOHAB"
#define CD_UTF8   "UTF-8"
#endif

static iconv_t johab_to_wan_cd = (iconv_t) -1;
static iconv_t wan_to_johab_cd = (iconv_t) -1;
static iconv_t johab_to_utf8_cd = (iconv_t) -1;
      
int johab_to_wansung(char *s, int johab) {
    int in_len, out_len;
    char in[2];
    char *in_p, *out_p;
    if (johab < 128) {
        *s = johab;
        return 1;
    }
    if (johab_to_wan_cd == (iconv_t) -1) {
        johab_to_wan_cd = iconv_open(CD_EUC_KR, CD_JOHAB);
        if (johab_to_wan_cd == (iconv_t) -1) {
            perror("johab_to_wansung");
            exit(-1);
        }
    }
    in_p = in; out_p = s;
    in_len = 2; out_len = 2;
    in[0] = (johab >> 8) | 0x80;
    in[1] = johab & 0xff;
    iconv(johab_to_wan_cd, &in_p, &in_len, &out_p, &out_len);

    return 2 - out_len;
}

int johab_to_wansung_str(char *s, char *johab, int len) {
    char *out_p = s;
    char *in_p = johab;
    int in_len = len;
    int out_len = len;
    if (johab_to_wan_cd == (iconv_t) -1) {
        johab_to_wan_cd = iconv_open(CD_EUC_KR, CD_JOHAB);
        if (johab_to_wan_cd == (iconv_t) -1) {
            perror("johab_to_wansung_str");
            exit(-1);
        }
    }
    iconv(johab_to_wan_cd, &in_p, &in_len, &out_p, &out_len);
    return len - in_len;
}

int johab_from_wansung(char *s, int wansung) {
    int in_len, out_len;
    char in[2];
    char *in_p, *out_p;
    if (wansung < 128) {
        *s = wansung;
        return 1;
    }
    if (wan_to_johab_cd == (iconv_t) -1) {
        wan_to_johab_cd = iconv_open(CD_JOHAB, CD_EUC_KR);
        if (wan_to_johab_cd == (iconv_t) -1) {
            perror("johab_from_wansung");
            exit(-1);
        }
    }
    in_p = in; out_p = s;
    in_len = 2; out_len = 2;
    in[0] = (wansung >> 8) | 0x80;
    in[1] = wansung & 0xff;
    iconv(wan_to_johab_cd, &in_p, &in_len, &out_p, &out_len);
    *out_p = 0;
    return 2 - out_len;
}

int johab_from_wansung_str(char *s, char *wansung, int len) {
    char *out_p = s;
    char *in_p = wansung;
    int in_len = len;
    int out_len = len;
    if (wan_to_johab_cd == (iconv_t) -1) {
        wan_to_johab_cd = iconv_open(CD_JOHAB, CD_EUC_KR);
        if (wan_to_johab_cd == (iconv_t) -1) {
            perror("johab_from_wansung_str");
            exit(-1);
        }
    }
    iconv(wan_to_johab_cd, &in_p, &in_len, &out_p, &out_len);
    *out_p = 0;
    return len - in_len;
}

int johab_to_johab(char *s, int johab) {
    if (johab < 128) {
        *s = johab;
        return 1;
    }
    *s++ = 0x80 | (johab >> 8);
    *s = johab & 0xff;
    return 2;
}

void johab_to_3(int johab, int *cho, int *jung, int *jong) {
    *cho = (johab >> 10) & 0x1f;
    *jung = (johab >> 5) & 0x1f;
    *jong = (johab) & 0x1f;
}

int johab_from_3(int cho, int jung, int jong) {
    return 0x8000 | (cho << 10) | (jung << 5) | jong;
}

int johab_to_utf8(char *s, int johab) {
    int in_len, out_len;
    char in[2];
    char *in_p, *out_p;
    if (johab < 128) {
        *s = johab;
        return 1;
    }
    if (johab_to_utf8_cd == (iconv_t) -1) {
        johab_to_utf8_cd = iconv_open(CD_UTF8, CD_JOHAB);
        if (johab_to_utf8_cd == (iconv_t) -1) {
             perror("johab_to_utf8");
            exit(-1);
        }
    }
    in_p = in; out_p = s;
    in_len = 2; out_len = 3;
    in[0] = (johab >> 8) | 0x80;
    in[1] = johab & 0xff;
    iconv(johab_to_utf8_cd, &in_p, &in_len, &out_p, &out_len);

    return 3 - out_len;
}
