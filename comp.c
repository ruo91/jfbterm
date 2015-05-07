/* This file is a part of Ami software, (C) Hwang chi-deok 2001 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>

#include "comp.h"
#include "johabcode.h"

typedef unsigned int unit;
typedef unsigned int bool;

#define CHO_SHIFT 10
#define JUNG_SHIFT 5
#define JONG_SHIFT 0

#define CHO_MASK	(0x3f << CHO_SHIFT)
#define JUNG_MASK	(0x1f << JUNG_SHIFT)
#define JONG_MASK	(0x1f << JONG_SHIFT)

#define CHO_JUNG_MASK	(CHO_MASK | JUNG_MASK)
#define CHO_JONG_MASK	(CHO_MASK | JONG_MASK)
#define JUNG_JONG_MASK	(JUNG_MASK | JONG_MASK)

#define CHO_NULL	(0x01 << CHO_SHIFT)
#define JUNG_NULL	(0x02 << JUNG_SHIFT)
#define JONG_NULL	(0x01 << JONG_SHIFT)

#define COMPOSER_KEYMAP_MAX 256

typedef enum {
    NOT_HAN, CHO, JUNG, JONG,
    CHO_JONG
} JamoType;

typedef enum {
    ENC_UTF8, ENC_WANSUNG, ENC_JOHAB
} Encoding;

typedef struct {
    JamoType type;
    int jung;
    int cho;
    int jong;
    int not_han;
} JamoInfo;

struct _Keymap {
    int ref;
    JamoInfo **jamoinfo;
    int *translation;
};

struct _Composer {
    JamoInfo *jamo[6];
    uint code[6];
    int stack_index;
    Keymap *keymap;
    char return_buffer[8];
    Encoding enc;
    int error_level;
};

typedef struct {
    char *name;
    char *filename;
} KeyboardFile;

enum {
    Kiyeog = 2,
    SsangKiyeog = 3,
    Nieun = 4,
    Dikeud = 5,
    SsangDikeud = 6,
    Rieul = 7,
    Mieum = 8,
    Pieub = 9,
    SsangPieub = 10,
    Sios = 11,
    SsangSios = 12,
    Ieung = 13,
    Jieuj = 14,
    SsangJieuj = 15,
    Cieuc = 16,
    Khieuq = 17,
    Theut = 18,
    Phieuf = 19,
    Hieuh = 20
};
enum {
    A = 3,
    AE = 4,
    YA = 5,
    YAE = 6,
    EO = 7,
    E = 10,
    YEO = 11,
    YE = 12,
    O = 13,
    WA = 14,
    WAE = 15,
    OE = 18,
    YO = 19,
    U = 20,
    WEO = 21,
    WE = 22,
    WI = 23,
    YU = 26,
    EU = 27,
    YI = 28,
    I = 29
};
enum {
    J_Kiyeog = 2,
    J_SsangKiyeog = 3,
    J_KiyeogSios = 4,
    J_Nieun = 5,
    J_NieunJieuj = 6,
    J_NieunHieuh = 7,
    J_Dikeud = 8,
    J_Rieul = 9,
    J_RieulKiyeog = 10,
    J_RieulMieum = 11,
    J_RieulPieub = 12,
    J_RieulSios = 13,
    J_RieulTieut = 14,
    J_RieulPhieuf = 15,
    J_RieulHieuh = 16,
    J_Mieum = 17,
    J_Pieub = 19,
    J_PieubSios = 20, 
    J_Sios = 21,
    J_SsangSios = 22,
    J_Ieung = 23,
    J_Jieuj = 24,
    J_Cieuc = 25,
    J_Khieuq = 26,
    J_Tieut = 27,
    J_Phieuf = 28,
    J_Hieuh = 29
};

static Keymap * keymap_load(char *keyboard);
static void keymap_free(Keymap *keymap);
static Keymap *def_keymap = 0;
static KeyboardFile *kbd = 0;
static int num_kbd = 0;
static char *public_dir = 0, *private_dir = 0;
static int shuffle_active = 0;

static const char *
cho_code[] = {
    [Kiyeog] = "ㄱ",
    [SsangKiyeog] = "ㄲ",
    [Nieun] = "ㄴ",
    [Dikeud] = "ㄷ",
    [SsangDikeud] = "ㄸ",
    [Rieul] = "ㄹ",
    [Mieum] = "ㅁ",
    [Pieub] = "ㅂ",
    [SsangPieub] = "ㅃ",
    [Sios] = "ㅅ",
    [SsangSios] = "ㅆ",
    [Ieung] = "ㅇ",
    [Jieuj] = "ㅈ",
    [SsangJieuj] = "ㅉ",
    [Cieuc] = "ㅊ",
    [Khieuq] = "ㅋ",
    [Theut] = "ㅌ",
    [Phieuf] = "ㅍ",
    [Hieuh] = "ㅎ"
};

static const char *
jung_code[] = {
    [A] = "ㅏ",
    [AE] = "ㅐ",
    [YA] = "ㅑ",
    [YAE] = "ㅒ",
    [EO] = "ㅓ",
    [E] = "ㅔ",
    [YEO] = "ㅕ",
    [YE] = "ㅖ",
    [O] = "ㅗ",
    [WA] = "ㅘ",
    [WAE] = "ㅙ",
    [OE] = "ㅚ",
    [YO] = "ㅛ",
    [U] = "ㅜ",
    [WEO] = "ㅝ",
    [WE] = "ㅞ",
    [WI] = "ㅟ",
    [YU] = "ㅠ",
    [EU] = "ㅡ",
    [YI] = "ㅢ",
    [I] = "ㅣ"
};

static const char *
jong_code[] = {
    [J_Kiyeog] = "ㄱ",
    [J_SsangKiyeog] = "ㄲ",
    [J_KiyeogSios] = "ㄱㅅ",
    [J_Nieun] = "ㄴ",
    [J_NieunJieuj] = "ㄴㅈ",
    [J_NieunHieuh] = "ㄴㅎ",
    [J_Dikeud] = "ㄷ",
    [J_Rieul] = "ㄹ",
    [J_RieulKiyeog] = "ㄹㄱ",
    [J_RieulMieum] = "ㄹㅁ",
    [J_RieulPieub] = "ㄹㅂ",
    [J_RieulSios] = "ㄹㅅ",
    [J_RieulTieut] = "ㄹㅌ",
    [J_RieulPhieuf] = "ㄹㅍ",
    [J_RieulHieuh] = "ㄹㅎ",
    [J_Mieum] = "ㅁ",
    [J_Pieub] = "ㅂ",
    [J_PieubSios] = "ㅂㅅ", 
    [J_Sios] = "ㅅ",
    [J_SsangSios] = "ㅆ",
    [J_Ieung] = "ㅇ",
    [J_Jieuj] = "ㅈ",
    [J_Cieuc] = "ㅊ",
    [J_Khieuq] = "ㅋ",
    [J_Tieut] = "ㅌ",
    [J_Phieuf] = "ㅍ",
    [J_Hieuh] = "ㅎ"
};


static int
cho_combine(int c1, int c2)
{
    if (c1 != c2) return 0;
    switch (c1) {
       case Kiyeog: return SsangKiyeog;
       case Dikeud: return SsangDikeud;
       case Pieub: return SsangPieub;
       case Sios: return SsangSios;
       case Jieuj: return SsangJieuj;
    }
    return 0;
}

static int
jung_combine(int c1, int c2)
{
    if (c1 == O) {
        if (c2 == A) return WA;
        if (c2 == AE) return WAE;
        if (c2 == I) return OE;
	return 0;
    }
    if (c1 == U) {
        if (c2 == EO) return WEO;
        if (c2 == E) return WE;
        if (c2 == I) return WI;
    }
    if (c1 == EU) {
        if (c2 == I) return YI;
    }
    return 0;
}

static int
jong_combine(int c1, int c2)
{
    if (c1 == J_Kiyeog) {
        if (c2 == J_Kiyeog) return J_SsangKiyeog;
        if (c2 == J_Sios) return J_KiyeogSios;
        return 0;
    }
    if (c1 == J_Nieun) {
        if (c2 == J_Jieuj) return J_NieunJieuj;
        if (c2 == J_Hieuh) return J_NieunHieuh;
        return 0;
    }
    if (c1 == J_Rieul) {
        if (c2 == J_Kiyeog) return J_RieulKiyeog;
        if (c2 == J_Mieum) return J_RieulMieum;
        if (c2 == J_Pieub) return J_RieulPieub;
        if (c2 == J_Sios) return J_RieulSios;
        if (c2 == J_Tieut) return J_RieulTieut;
        if (c2 == J_Phieuf) return J_RieulPhieuf;
        if (c2 == J_Hieuh) return J_RieulHieuh;
        return 0;
    }
    if (c1 == J_Pieub) {
        if (c2 == J_Sios) return J_PieubSios;
        return 0;
    }
    return 0;
}

static uint
composer_get_last_code(Composer *comp) 
{
    if (comp->stack_index >= 0) 
        return comp->code[comp->stack_index];
    else
        return CHO_NULL | JUNG_NULL | JONG_NULL;
}

static bool
composer_has_chosung(Composer *comp)
{
    uint c = composer_get_last_code(comp);
    return ((CHO_MASK & c) != CHO_NULL);
}

static bool
composer_has_jungsung(Composer *comp)
{
    uint c = composer_get_last_code(comp);
    return ((JUNG_MASK & c) != JUNG_NULL);
}

static bool
composer_has_jongsung(Composer *comp)
{
    uint c = composer_get_last_code(comp);
    return ((JONG_MASK & c) != JONG_NULL);
}

static JamoInfo *
composer_get_last_char(Composer *composer)
{
    return composer->jamo[composer->stack_index];
}

void
composer_reset(Composer *comp)
{
    comp->stack_index = -1;
}

static void
composer_pop(Composer *comp)
{
    comp->stack_index--;
}

static void
composer_set_cho(Composer *comp, JamoInfo *jamo, uint cho)
{
    uint code;
    if (jamo->type != CHO && jamo->type != CHO_JONG) {
        fprintf(stderr, "internal error at %s\n", __FUNCTION__);
        return;
    }
    if (!cho) cho = jamo->cho;
        cho <<= CHO_SHIFT;
    if (comp->stack_index < 0) {
        code = JUNG_NULL | JONG_NULL | cho;
    } else {
        code = comp->code[comp->stack_index];
        code = (JUNG_JONG_MASK & code) | cho;
    }
    comp->stack_index++;
    comp->jamo[comp->stack_index] = jamo;
    comp->code[comp->stack_index] = code;
}

static void
composer_set_jung(Composer *comp, JamoInfo *jamo, uint jung)
{
    uint code;
    if (jamo->type != JUNG) {
        fprintf(stderr, "internal error at %s\n", __FUNCTION__);
        return;
    }
    if (!jung) jung = jamo->jung;
        jung <<= JUNG_SHIFT;
    if (comp->stack_index < 0) {
        code = CHO_NULL | JONG_NULL | jung;
    } else {
        code = comp->code[comp->stack_index];
        code = (CHO_JONG_MASK &code) | jung;
    }
    comp->stack_index++;
    comp->jamo[comp->stack_index] = jamo;
    comp->code[comp->stack_index] = code;
}

static void
composer_set_jong(Composer *comp, JamoInfo *jamo, uint jong)
{
    uint code;
    if (jamo->type != JONG && jamo->type != CHO_JONG) {
        fprintf(stderr, "internal error at %s\n", __FUNCTION__);
        return;
    }
    if (!jong) jong = jamo->jong;
    if (comp->stack_index < 0) {
        code = CHO_NULL | JUNG_NULL | jong;
    } else {
        code = comp->code[comp->stack_index];
//        printf("set jong code = %x jong = %x\n", code, jong);
        code = (CHO_JUNG_MASK & code) | jong;
    }
    comp->stack_index++;
    comp->jamo[comp->stack_index] = jamo;
    comp->code[comp->stack_index] = code;
//    printf("set jong code = %x jong = %x\n", code, jong);
}

static uint
composer_try_combine_cho(Composer *composer, JamoInfo *info)
{
    JamoInfo *last;
    if (composer->stack_index >= 1) return 0;
    last = composer->jamo[composer->stack_index];
    return cho_combine(last->cho, info->cho);
}

static uint
composer_try_combine_jung(Composer *composer, JamoInfo *info)
{
    JamoInfo *last;
    if (composer->stack_index >= 1) {
        last = composer->jamo[composer->stack_index-1];
        if (last->type == JUNG) return 0;
    }
    last = composer->jamo[composer->stack_index];
    return jung_combine(last->jung, info->jung);
}

static uint
composer_try_combine_jong(Composer *composer, JamoInfo *info)
{
    JamoInfo *last;
    if (composer->stack_index >= 2) {
        last = composer->jamo[composer->stack_index - 1];
        if (last->type != JUNG) return 0;
    }
    last = composer->jamo[composer->stack_index];
    return jong_combine(last->jong, info->jong);
}

void
composer_set_output_encoding(Composer *comp, char *enc)
{
    if (!enc) return;
    if (strcmp(enc, "UTF-8") == 0) comp->enc = ENC_UTF8;
    else if (strcmp(enc, "WANSUNG") == 0) comp->enc = ENC_WANSUNG;
    else if (strcmp(enc, "JOHAB") == 0) comp->enc = ENC_JOHAB; 
}

int
composer_get_output_encoding(Composer *comp) //
{
    if(comp->enc == ENC_UTF8) return 1;
    else if(comp->enc == ENC_WANSUNG) return 2;
    else if(comp->enc == ENC_JOHAB) return 3;
    else return 0;
}

static int
composer_set_return_buffer(Composer *comp, uint code1, uint code2)
{
    char *result = comp->return_buffer;
    int (*converter)(char *, int) = 0;
    switch (comp->enc) {
        case ENC_WANSUNG: converter = johab_to_wansung; break;
        case ENC_UTF8: converter = johab_to_utf8; break;
        case ENC_JOHAB: converter = johab_to_johab; break;
    }
    result += (converter)(result, code1);
    if (!result) return 0;
    if (code2) result += (converter)(result, code2);
    *result = '\0';
    return 1;
}

char *
composer_get_buffer(Composer *comp)
{
    return comp->return_buffer;
}


static JamoInfo *
composer_get_char_info(Composer *comp, int c)
{
    Keymap *keymap = (comp->keymap?:def_keymap);
    JamoInfo *jamo = keymap->jamoinfo[c];
    if (!jamo && (c >= 'A' && c <= 'Z')) {
        jamo = keymap->jamoinfo[c - ('A' - 'a')];
    }
    return jamo;
}

static int
composer_is_valid_code(Composer *composer)
{
    if (composer->enc == ENC_WANSUNG) {
        int code = composer->code[composer->stack_index];
        char buf[2];
        return 2 == johab_to_wansung(buf, code);
    }
    return 1;
}

static void
composer_set_return1(Composer *composer)
{
    composer_set_return_buffer(composer, composer->code[composer->stack_index], 0);
}
/*				?why?
static void
composer_set_return2(Composer *composer, uint code1)
{
    composer_set_return_buffer(composer, code1, composer->code[composer->stack_index]);
}
*/
#define CORRECT_RETURN composer_set_return1(composer); return COMPOSER_CORRECT
#define NEW_RETURN composer_set_return1(composer); return COMPOSER_NEW

static ComposerProcessState
composer_erase_backward(Composer *composer)
{
    if (composer->stack_index < 0) return COMPOSER_RAW;
    if (composer->stack_index == 0) {
        composer_reset(composer);
        return COMPOSER_NULL;
    }
    composer_pop(composer);
    CORRECT_RETURN;
}

ComposerProcessState
composer_process(Composer *composer, int c)
{
    JamoInfo * info;
    Keymap *keymap;

    if (c == '\b' || c == '') {
        return composer_erase_backward(composer);
    }
    keymap = composer->keymap?composer->keymap:def_keymap;
    if (keymap->translation) {
        if (keymap->translation[c]) c = keymap->translation[c];
    }
    info = composer_get_char_info(composer, c);
    if (!info) {
        composer_reset(composer);
        return COMPOSER_RAW;
    }
    switch (info->type) {
	case NOT_HAN:
	    composer_reset(composer);
	    composer_set_return_buffer(composer, info->not_han, 0);
	    return COMPOSER_NOT_HAN;
	case CHO:
        if (composer_has_jongsung(composer)) {
		/* ? + ? + jong */
            composer_reset(composer);
            composer_set_cho(composer, info, 0);
            NEW_RETURN;
	    } else if (!composer_has_chosung(composer)) {
		/*  0 + ? + 0 */
            composer_set_cho(composer, info, 0);
            if (composer_is_valid_code(composer)) {
                if (composer->stack_index > 0) {
                    if (shuffle_active) {
                        CORRECT_RETURN;
                    } else {
                        composer_reset(composer);
                        composer_set_cho(composer, info, 0);
                        NEW_RETURN;
                    }
                } else {
                    NEW_RETURN;
                }
            } else {
                composer_reset(composer);
                composer_set_cho(composer, info, 0);
                NEW_RETURN;
            }
	    } else if (composer_has_jungsung(composer)) {
            /*  cho + jung + 0 */
            composer_reset(composer);
            composer_set_cho(composer, info, 0);
            NEW_RETURN;
	    } else if (composer_has_chosung(composer)) {
            /* cho + 0 + 0 */
            uint cho = composer_try_combine_cho(composer, info);
            if (cho) {
                composer_set_cho(composer, info, cho);
                CORRECT_RETURN;
            } 
            if (composer->error_level > 0) {
                return COMPOSER_ERROR;
            } else {
                composer_reset(composer);
                composer_set_cho(composer, info, 0);
                NEW_RETURN;
            }
	    } 
	    break;
	case JUNG:
	    if (composer_has_jongsung(composer)) {
            /* ? + ? + jong */
            JamoInfo *last_char = composer_get_last_char(composer);
            if (last_char->type == CHO_JONG) {
                /* 두벌식 */
                int code1;
                composer_pop(composer);
                code1 = composer_get_last_code(composer);
                composer_reset(composer);
                composer_set_cho(composer, last_char, 0);
                composer_set_jung(composer, info, 0);
                composer_set_return_buffer(composer, code1, composer_get_last_code(composer));
                return COMPOSER_CORRECT_NEW;
            } else {
                /* 세벌식 */
                composer_reset(composer);
                composer_set_jung(composer, info, 0);
                NEW_RETURN;
            }
	    } else if (composer_has_jungsung(composer)) {
            /* ? + jung + 0 */
            uint jung = composer_try_combine_jung(composer, info);
            if (jung) {
                composer_set_jung(composer, info, jung);
                if (composer_is_valid_code(composer)) {
                    CORRECT_RETURN;
                } 
                composer_pop(composer);
            } 
            if (shuffle_active) {
                if (composer_is_valid_code(composer)) {
                    composer_set_return1(composer); 
                    composer_reset(composer);
                    composer_set_jung(composer, info, 0);
                    NEW_RETURN;
                }
            }
            if (composer->error_level > 0) {
                return COMPOSER_ERROR;
            } else {
                composer_reset(composer);
                composer_set_jung(composer, info, 0);
                NEW_RETURN;
            }
	    } else if (composer_has_chosung(composer)) {
            /* cho + 0 + 0 */
            composer_set_jung(composer, info, 0);
            if (composer_is_valid_code(composer)) {
                CORRECT_RETURN;
            } else {
                composer_pop(composer);
                return COMPOSER_ERROR;
            }
	    } else {
            /* 0 + 0 + 0 */
            composer_set_jung(composer, info, 0);
            NEW_RETURN;
	    }
	    break;
	case JONG:
	    if (composer_has_chosung(composer) && composer_has_jungsung(composer)) {
            /* cho + jung + ? */
            if (!composer_has_jongsung(composer)) {
                /* cho + jung + 0 */
                composer_set_jong(composer, info, 0);
                if (composer_is_valid_code(composer)) {
                    CORRECT_RETURN;
                } else {
                    composer_pop(composer);
                    return COMPOSER_ERROR;
                }
            } else {
                /* cho + jung + jong */
                uint jong = composer_try_combine_jong(composer, info);
                if (jong) {
                    composer_set_jong(composer, info, jong);
                    if (composer_is_valid_code(composer)) {
                        CORRECT_RETURN;
                    } else {
                        composer_pop(composer);
                        return COMPOSER_ERROR;
                    }
                } else {
                    return COMPOSER_ERROR;
                }
            }
	    } else {
            /* 0 + jung + ? */
            /* cho + 0 + ? */
            return COMPOSER_ERROR;
	    }
	    break;
	case CHO_JONG:
	    if (composer_has_jongsung(composer)) {
	        /* ? + ? + jong */
	        uint jong = composer_try_combine_jong(composer, info);
	        if (jong) {
	            composer_set_jong(composer, info, jong);
	            if (composer_is_valid_code(composer)) {
	                CORRECT_RETURN;
	            } 
	        } 
	        composer_reset(composer);
	        composer_set_cho(composer, info, 0);
	        NEW_RETURN;
	    } else if (composer_has_jungsung(composer)) {
	        /* ? + jung + 0 */
	        composer_set_jong(composer, info, 0);
	        if (!shuffle_active) {
	            if (composer_is_valid_code(composer)) {
	                CORRECT_RETURN;
	            }
	        }
	        else {
	            if(composer_is_valid_code(composer) && composer->stack_index > 1) {
	               CORRECT_RETURN;
	            }
	            composer_pop(composer);
	            if (composer_is_valid_code(composer)) {
	                if (composer->stack_index > 0) {
	                    composer_set_return1(composer); 
	                }
	                else {
	                    composer_set_cho(composer, info, 0);
	                    if (composer_is_valid_code(composer)) {
	                        CORRECT_RETURN;
	                    }
	                }
		        }
	        }
	        composer_reset(composer);
	        composer_set_cho(composer, info, 0);
	        NEW_RETURN;
	    } else if (composer_has_chosung(composer)) {
	        /* cho + 0 + 0 */
	        uint cho = composer_try_combine_cho(composer, info);
	        if (cho) {
	            composer_set_cho(composer, info, cho);
	            CORRECT_RETURN;
	        } else {
	            if (composer->error_level > 0) {
	                return COMPOSER_ERROR;
	            } else {
	                composer_reset(composer);
	                composer_set_cho(composer, info, 0);
	                NEW_RETURN;
	            }
	        }
	    } else {
	        /* 0 + 0 + 0 */
	        composer_set_cho(composer, info, 0);
	        NEW_RETURN;
	    }
	    break;
    }
    return COMPOSER_ERROR;
}

int
composer_has_temp_hangul(Composer *composer)
{
    return composer->stack_index >= 0;
}

Composer *
composer_new(void)
{
    return composer_new_with_keymap(NULL);
}

Composer *
composer_new_with_keyboard(char *keyboard)
{
//    Composer *comp;			/* ?why? */
    Keymap *keymap;

    if (!keyboard) return NULL;
    keymap = keymap_load(keyboard);
    if (!keymap) return NULL;
    /* keymap_load()에서 1로 설정되는 데, composer_new_with_keymap()에서
     * 다시 ref counter를 증가시키므로 여기에서 reset */
    keymap->ref = 0;
    return composer_new_with_keymap(keymap);
}

int
composer_set_keymap(char *keyboard)
{
    Keymap * keymap;
    if (!keyboard) return 0;
        keymap = keymap_load(keyboard);
    if (keymap) {
        if (def_keymap) keymap_free(def_keymap);
        def_keymap = keymap;
        return 1;
    }
    return 0;
}

Composer *
composer_new_with_keymap(Keymap *keymap)
{
    Composer *comp;
    comp = (Composer *)malloc(sizeof(Composer));
    memset(comp, 0, sizeof(Composer));
    comp->keymap = keymap;
    if (keymap) keymap->ref++;
    composer_reset(comp);
    comp->enc = ENC_WANSUNG;
    comp->error_level = 1;
    return comp;
}


void
composer_free(Composer *composer)
{
//    int i;			/* ?why? */
    keymap_free(composer->keymap);
    free(composer);
}

static void
keymap_free(Keymap *keymap)
{
    int i;
    if (!keymap) return;
    keymap->ref--;
    if (keymap->ref) return;
    for(i = 0; i < COMPOSER_KEYMAP_MAX; i++) {
        if (keymap->jamoinfo[i]) free(keymap->jamoinfo[i]);
    }
    free(keymap);
}

static char *
skip_space(char *s)
{
    while (isspace((unsigned char)*s)) s++;
    return s;
}

static char *
skip_word(char *s)
{
    while (!isspace((unsigned char)*s)) s++;
    return s;
}

#define FIND_KEY(a, b) find_key(a, b, sizeof(a)/sizeof(a[0]))

static int
find_key(const char * code[], char *value, int max)
{
    int i;
    for(i = 1; i < max; i++) if (code[i] && (strcmp(code[i], value) == 0)) return i;
    return 0;
}

static void
read_translation_table(char *map_file, int *table)
{
    unsigned char *s, line[256];
    char filename[256];
    FILE *fp;

    snprintf(filename, sizeof(filename), "%s/%s", private_dir, map_file);
    fp = fopen(filename, "r");
    if (!fp) {
        snprintf(filename, sizeof(filename), "%s/%s", public_dir, map_file);
        fp = fopen(filename, "r");
    }
    if (!fp) {
        printf("cannot read map file: %s\n", map_file);
        return;
    }
    while (!feof(fp)) {
        fgets(line, 256, fp);

        /* skip empty-line/comment */
        s = skip_space(line);
        if (s[0] == 0 || (line[0] == '#' && line[1] == '#')) continue;
        if (strncmp(&s[1], " -> ", 4) || s[5] == 0 || isspace(s[5])
	    || s[5] == '\n' || s[5] == '\r') {
            printf("Invalid translation(%s): %s\n", map_file, line);
            continue;
	}

	table[s[0]] = s[5];
    }
    fclose(fp);
}


static Keymap *
keymap_load(char *keyboard)
{
    Keymap *keymap;
    unsigned char line[256];
    FILE *fp;
    unsigned char *s;
    unsigned char key;
    unsigned char *value;
    uint cho = 0, jong = 0, jung = 0, not_han = 0;
    int type = NOT_HAN;
    int line_num = 0;
    int i;

    for(i = 0; i < num_kbd; i++) {
        if (strcmp(keyboard, kbd[i].name) == 0) break;
    }
    if (i == num_kbd) {
        fprintf(stderr, "cannot load '%s'\n", keyboard);
        fprintf(stderr, "No such keyboard file\n");
        return NULL;
    }
    fp = fopen(kbd[i].filename, "r");
    if (!fp) {
        fprintf(stderr, "cannot open for reading '%s'\n", kbd[i].filename);
        return NULL;
    }

    keymap = (Keymap *)malloc(sizeof(Keymap));
    keymap->jamoinfo = (JamoInfo **)malloc(sizeof(JamoInfo *) * COMPOSER_KEYMAP_MAX);
    memset(keymap->jamoinfo, 0, sizeof(JamoInfo *) * COMPOSER_KEYMAP_MAX);
    keymap->translation = 0;
    while (!feof(fp)) {
        fgets(line, sizeof(line), fp);
        line_num++;

        /* skip empty-line/comment */
        s = skip_space(line);
        if (s[0] == 0 || (s[0] == '#' && s[1] == '#')) continue;

        if (strncmp(s, "import", 6) == 0) {
            Keymap *ji;
            char *kb;
            int i;
            kb = skip_space(s + 6);
            s = kb + strlen(kb) - 1;
            while (isspace((unsigned char)*s)) s--;
            s++;
            *s = '\0';
            ji = keymap_load(kb);
            if (!ji) continue;
            for(i = 0; i < COMPOSER_KEYMAP_MAX; i++) {
                if (ji->jamoinfo[i]) {
                    if (!keymap->jamoinfo[i]) keymap->jamoinfo[i] = (JamoInfo *)malloc(sizeof(JamoInfo));
                    keymap->jamoinfo[i]->type = ji->jamoinfo[i]->type;
                    keymap->jamoinfo[i]->cho = ji->jamoinfo[i]->cho;
                    keymap->jamoinfo[i]->jung = ji->jamoinfo[i]->jung;
                    keymap->jamoinfo[i]->jong = ji->jamoinfo[i]->jong;
                    keymap->jamoinfo[i]->not_han = ji->jamoinfo[i]->not_han;
                }
            }
            keymap_free(ji);
            continue;
//          } else if (strncmp(s, "usemap", 6) == 0) {
        } else if (strcmp(s, "usemap") == 0) {		/* use 'strcmp' function */
            char *map;
            if (!keymap->translation) {
                keymap->translation = (int *)malloc(sizeof(int)*COMPOSER_KEYMAP_MAX);
                memset(keymap->translation, 0, sizeof(int)*COMPOSER_KEYMAP_MAX);
            }
            map = s = skip_space(s + 6);
            s = map + strlen(map) - 1;
            while (isspace((unsigned char)*s)) s--;
            s++;
            *s = '\0';
            read_translation_table(map, keymap->translation);
            continue;
        }
        key = *s;
        s = skip_space(s + 1);
        value = s;
        s = skip_word(s); *s = '\0';
        s = skip_space(s + 1);
        if (s[0] && strncmp(s, "cho", 3) == 0) {
            type = CHO;
            cho = FIND_KEY(cho_code, value);
            if (!cho) {
                fprintf(stderr, "%s: invalid line: # = %d\n", kbd[i].filename,
                line_num);
                continue;
            }
        } else if (s[0] && strncmp(s, "jon", 3) == 0) {
            type = JONG;
            jong = FIND_KEY(jong_code, value);
        } else {
            cho = FIND_KEY(cho_code, value);
            jong = FIND_KEY(jong_code, value);
            jung = FIND_KEY(jung_code, value);
//            printf("cho = %d jong = %d jung = %d value = %s\n", cho, jong, jung, value);
            if (jung) type = JUNG;
            else if (cho && jong) type = CHO_JONG;
            else if (cho) type = CHO;
            else if (jong) type = JONG;
            else if (!cho && !jung && !jong) {
                type = NOT_HAN;
                not_han = value[0];
                if (not_han >= 128) {
                    not_han  = (not_han << 8) + value[1];
                    johab_from_wansung(value, not_han);
                    not_han = (value[0] << 8) + value[1];
                }
            } else {
                fprintf(stderr, "%s: invalid line: # = %d\n", kbd[i].filename, line_num);
            }
        }
	    if (!keymap->jamoinfo[key]) keymap->jamoinfo[key] = (JamoInfo *)malloc(sizeof(JamoInfo));
	    keymap->jamoinfo[key]->type = type;
	    keymap->jamoinfo[key]->cho = cho;
	    keymap->jamoinfo[key]->jung = jung;
	    keymap->jamoinfo[key]->jong = jong;
	    keymap->jamoinfo[key]->not_han = not_han;
    }
    fclose(fp);
    keymap->ref = 1;
    return keymap;
}

static KeyboardFile *
get_kbd_names(char *directory, int *n)
{
    DIR *dir;
    int num_kbd;
    KeyboardFile *kbd;
    struct dirent *ent;
    dir = opendir(directory);
    if (!dir) {
        *n = 0;
        return NULL;
    }
    num_kbd = 0;
    while ((ent = readdir(dir)) != NULL) {
        char *name = ent->d_name;
        int len = strlen(name);
        if (len <= 4) continue;
        if (strcmp(".kbd", name + len - 4) != 0) continue;
        num_kbd++;
    }
    kbd = (KeyboardFile *)malloc(sizeof(KeyboardFile) * num_kbd);
    rewinddir(dir);
    num_kbd = 0;
    while ((ent = readdir(dir)) != NULL) {
        char *name = ent->d_name;
        int len = strlen(name);
        if (len <= 4) continue;
        if (strcmp(".kbd", name + len - 4) != 0) continue;

        kbd[num_kbd].filename = malloc(len + strlen(directory) + 2);
        strcpy(kbd[num_kbd].filename, directory);
        strcat(kbd[num_kbd].filename, "/");
        strcat(kbd[num_kbd].filename, name);

        kbd[num_kbd].name = (char *)malloc(len - 3);
        memcpy(kbd[num_kbd].name, name, len - 4);
        kbd[num_kbd].name[len-4] = '\0';
        num_kbd++;
    }
    *n = num_kbd;
    closedir(dir);
    return kbd;
}

char **
composer_get_keyboard_list(char *pdir, int *n)
{
//    DIR *dir;					/* ?why? */
//    struct dirent *ent;		/* ?why? */
    KeyboardFile *kbd1, *kbd2;
    static char **keyboard_list = NULL;
    char my_dir[256];
    int n1, n2;
    int i;

    if (keyboard_list) {
        *n = num_kbd;
        return keyboard_list;
    }

    public_dir = pdir;

    if (!public_dir) {
        public_dir = getenv("HANGUL_KEYBOARD_DIRECTORY");
    }

    kbd1 = get_kbd_names(public_dir, &n1);
    snprintf(my_dir, sizeof(my_dir), "%s/.hangul_keyboard", getenv("HOME"));

    private_dir = strdup(my_dir);

    kbd2 = get_kbd_names(my_dir, &n2);

    if ((n1 + n2) == 0) {
        fprintf(stderr, "cannot find any keyboard files\n");
        exit(-1);
    }
    if (n2) {
        if (n1) {
            int k = 0;
            kbd = realloc(kbd2, sizeof(KeyboardFile)*(n1 + n2));
            for (i = 0; i < n1; i++) {
                int j;
                char *name = kbd1[i].name;
                for(j = 0; j < n2; j++) {
                    if (strcmp(kbd[j].name, name) == 0) break;
                }
                if (j>=n2) {
                    kbd[n2 + k++] = kbd1[i];
                }
            }
            n1 = k;
            free(kbd1);
        } else {
            kbd = kbd2;
        }
    } else {
        kbd = kbd1;
    }

    num_kbd = n1 + n2;
    keyboard_list = (char **)malloc(sizeof(char *) * (num_kbd));
    for(i = 0; i < num_kbd; i++) {
        keyboard_list[i] = kbd[i].name;
    }
    *n = num_kbd;
    return keyboard_list;
}

void
composer_set_shuffle_active(int active)
{
    shuffle_active = (int) (active != 0);
}

