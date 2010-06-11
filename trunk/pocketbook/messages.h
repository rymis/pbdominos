
#ifndef SGETTEXT___messages___H
#define SGETTEXT___messages___H

#ifdef __cplusplus
extern "C" {
#endif

const char *sgettext(const char *msg);
const char *slgettext(const char *msg, const char *lang);
void sgettext_lang_init(const char *lang);
#define _T(m) sgettext((m))

#ifdef __cplusplus
}
#endif

#endif
    