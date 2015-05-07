/* This file is a part of Ami software, (C) Hwang chi-deok 2001 */

#ifndef __COMP_H__
#define __COMP_H__

typedef struct _Composer Composer;
typedef struct _Keymap Keymap;
typedef enum {
    COMPOSER_NEW,
    COMPOSER_RAW,
    COMPOSER_NULL,
    COMPOSER_NOT_HAN,
    COMPOSER_CORRECT,
    COMPOSER_ERROR,
    COMPOSER_CORRECT_NEW
} ComposerProcessState;

Composer *composer_new(void);
Composer * composer_new_with_keymap(Keymap *keymap);
ComposerProcessState composer_process(Composer *comp, int c);
char * composer_get_buffer(Composer *comp);
void composer_set_output_encoding(Composer *comp, char *enc);
int composer_get_output_encoding(Composer *comp);
void composer_reset(Composer *comp);
char **composer_get_keyboard_list(char *directory_name, int *num_kbd);
int composer_set_keymap(char *keyboard);
int composer_has_temp_hangul(Composer *composer);
Keymap * composer_get_keymap(Composer *composer);
void composer_free(Composer *composer);
void composer_set_shuffle_active(int active);

#endif
