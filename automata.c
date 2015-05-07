/* 
 * automata.c 
 * hanterm automata (for hanterm-xf-2.0.5)
 * slightly edited for UTF-8 compatibility 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "comp.h"
#include "automata.h"

#define has_temp_han() (han_compose_buf[0] != 0)
#define ENC_UTF8 1 //
#define ENC_WANSUNG 2 //
#define ENC_JOHAB 3 //

/* included 1 - first */
int han_kbd = 2;
int han_code = 0;
int han_esc_toggle;
char *han_dubeol_keyboard_file = "qwerty_2beol";
char *han_sebeol_keyboard_file = "qwerty_3beol_final";
/* included 1 - end */

char han_compose_buf[8] = {0};
int hangul_state = 0;		/* 0 = English | 1 = Korean */

int han_new_typed_row = -999;

static Composer *composer = NULL;

/* included 2 - first */
void get_ime_status(char *buf) {
    buf[0] = han_compose_buf[0];
    buf[1] = han_compose_buf[1];
    if(composer_get_output_encoding(composer) == ENC_UTF8) { //
	buf[2] = han_compose_buf[2]; //
    } //
}

void set_keyboard_file(int keyboard, char *filename) {
    if(keyboard == 2) han_dubeol_keyboard_file = filename;
    else if(keyboard == 3) han_sebeol_keyboard_file = filename;
    hangul_set_keyboard(keyboard);
}

void toggle_keyboard_layout(void) {
    hangul_set_keyboard(han_kbd == 2 ? 3 : 2);
}
/* included 2 - end */

static int fill_return_buffer_c(char *buf, char c) {
    if (has_temp_han()) {
        *buf++ = han_compose_buf[0];
        *buf++ = han_compose_buf[1];
	if(composer_get_output_encoding(composer) == ENC_UTF8) //
	    *buf++ = han_compose_buf[2]; //
        *buf = c;
        han_compose_buf[0] = 0;
        return 3;
    }
    *buf = c;
    return 1;
}

static int fill_return_buffer_s(char *buf, char *s) {
    int len;
    if (!s) {
	if (has_temp_han()) {
	    strcpy(buf, han_compose_buf);
	    return 2;
	}
	return 0;
    }
    len = strlen(s);
    if (has_temp_han()) {
        *buf++ = han_compose_buf[0];
        *buf++ = han_compose_buf[1];
	if(composer_get_output_encoding(composer) == ENC_UTF8) //
	    *buf++ = han_compose_buf[2]; //
        strcpy(buf, s);
        han_compose_buf[0] = 0;
        return len + 2;
    }
    strcpy(buf, s);
    return len;
}

void hangul_automata_init(void) {
    int i, n;
    char **kbd_names;
    char *dubeol = han_dubeol_keyboard_file;
    char *sebeol = han_sebeol_keyboard_file;
    kbd_names = composer_get_keyboard_list(KEYBOARD_DIR, &n);
    for(i = 0; i < n; i++) {
	if (strcmp(kbd_names[i], dubeol) == 0) break;
    }
    if (i == n) {
        fprintf(stderr, "두벌식 자판 파일(%s)을 찾을 수 없습니다\n", dubeol);
        exit(1);
    }
    for(i = 0; i < n; i++) {
	if (strcmp(kbd_names[i], sebeol) == 0) break;
    }
    if (i == n) {
        fprintf(stderr, "세벌식 자판 파일(%s)을 찾을 수 없습니다\n", sebeol);
        exit(1);
    }
    hangul_set_keyboard(han_kbd);
    composer = composer_new();
    hangul_set_code(han_code);
}

int hangul_automata(int c, char *buf) {
    int ret = 0;
    int process_ret;
    if (!composer) {
        hangul_automata_init();
    }
    if (c == -2) return hangul_automata_flush(buf);
    if (c == -1) return hangul_automata_toggle(buf);

    assert (c >= 0);

    if (!hangul_state) {
        buf[0] = c;
        return 1;
    }
    switch ((process_ret = composer_process(composer, c))) {
	case COMPOSER_NEW:
	    ret = fill_return_buffer_s(buf, NULL);
	    strcpy(han_compose_buf, composer_get_buffer(composer));
	    break;
	case COMPOSER_RAW:
	    ret = fill_return_buffer_c(buf, c);
	    han_compose_buf[0] = 0;
	    if (c == 27 && han_esc_toggle) {
		hangul_state = 0;
	    }
	    break;
	case COMPOSER_NULL:
	    han_compose_buf[0] = 0;
	    break;
	case COMPOSER_NOT_HAN:
	    ret = fill_return_buffer_s(buf, composer_get_buffer(composer));
	    han_compose_buf[0] = 0;
	    break;
	case COMPOSER_CORRECT:
	    strcpy(han_compose_buf, composer_get_buffer(composer));
	    ret = 0;
	    break;
	case COMPOSER_ERROR:
	    return 0;
	case COMPOSER_CORRECT_NEW:
	    strcpy(buf, composer_get_buffer(composer));
	    if(composer_get_output_encoding(composer) == ENC_UTF8) //
		strcpy(han_compose_buf, composer_get_buffer(composer) + 3); //
	    else
		strcpy(han_compose_buf, composer_get_buffer(composer) + 2);
	    ret = 2;
	    break;
    }
    if (ret || process_ret == COMPOSER_NULL) {
        int c = han_compose_buf[0];
        han_compose_buf[0] = 0;
        han_compose_buf[0] = c;
    }
    return ret;
}

int hangul_automata_flush(char *buf) {
    composer_reset(composer);
    if (has_temp_han()) {
        buf[0] = han_compose_buf[0];
        buf[1] = han_compose_buf[1];
	if(composer_get_output_encoding(composer) == ENC_UTF8)
	    buf[2] = han_compose_buf[2];
        han_compose_buf[0] = 0;
        return 2;
    }
    return 0;
}

int hangul_automata_toggle(char *buf) {
    hangul_state = !hangul_state;
    composer_reset(composer);
    return hangul_automata_flush(buf);
}

void hangul_set_keyboard(int keyboard) {
    if (keyboard == 2) {
        composer_set_keymap(han_dubeol_keyboard_file);
    } else {
        composer_set_keymap(han_sebeol_keyboard_file);
    }
}

void hangul_set_code(int code) {
    if (!composer) {
        hangul_automata_init();
    }
    if (code == C_WANSUNG) {
        composer_set_output_encoding(composer, "WANSUNG");
    } else if (code == C_JOHAB) {
        composer_set_output_encoding(composer, "JOHAB");
    } else if (code == C_UTF8) { //
	composer_set_output_encoding(composer, "UTF-8"); //
    }

}

void hangul_automata_clear(void) {
    composer_reset(composer);
    han_compose_buf[0] = 0;
}
