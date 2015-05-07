
#ifndef __AUTOMATA_H__
#define __AUTOMATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#define C_WANSUNG	2	/* Hangul Code  */
#define C_JOHAB		1
#define C_UTF8	0
#ifdef HAVE_CONFIG_H
#include "config.h"
#define KEYBOARD_DIR SYSCONFDIR"/jfbterm-han/keyboard"
#else
#define KEYBOARD_DIR "/usr/etc/jfbterm-han/keyboard"
#endif

void hangul_automata_init(void);
int hangul_automata(int c, char *buf);
int hangul_automata_flush(char *buf);
int hangul_automata_toggle(char *buf);
void hangul_set_keyboard(int keyboard);
void hangul_set_code(int code); //
void hangul_automata_clear(void);
void get_ime_status(char*);
void set_keyboard_file(int keyboard, char* filename);
void toggle_keyboard_layout(void);

#ifdef __cplusplus
}
#endif

#endif /* __AUTOMATA_H__ */
