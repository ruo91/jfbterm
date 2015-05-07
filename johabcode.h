
int johab_to_wansung(char *s, int johab);
int johab_from_wansung(char *s, int wansung);

int johab_to_johab(char *s, int johab);

int johab_to_utf8(char *s, int johab);

int johab_to_wansung_str(char *s, char *johab, int len);
int johab_from_wansung_str(char *s, char *wansung, int len);

void johab_to_3(int johab, int *cho, int *jung, int *jong);
int johab_from_3(int cho, int jung, int jong);
