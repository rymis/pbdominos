
/* This file is generated automatically from messages.db */
#include <string.h>
#include <stdlib.h>

/* For compilers that does not support 'const' */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

typedef struct translation_st {
    char *msg;
    char *l_ru;

} translation;

static translation messages[] = {
    { "Congradulations!", "Поздравляю!" },
    { "KOZEL", "КОЗЕЛ!" },
    { "You are the looser!", "Вы проиграли!" },
    { "You won!", "Вы выиграли!" },
    { NULL }
};


/* Functions: */

static const int messages_cnt = sizeof(messages) / sizeof(translation) - 1;
static char lang_buf[3];
static const char *g_lang = NULL;
static int initialized = 0;

void sgettext_lang_init(const char *lang)
{
    /* TODO: get LC_ALL, LC_MESSAGES... */
    if (lang) {
        g_lang = lang;
        initialized = 1;
        return;
    } else {
        const char *lc = getenv("LC_MESSAGES");
        if (lc) {
            lang_buf[0] = lc[0];
            lang_buf[1] = lc[1];
            lang_buf[2] = 0;
            g_lang = lang_buf;
            initialized = 1;
            return;
        }
        lc = getenv("LC_ALL");
        if (lc) {
            lang_buf[0] = lc[0];
            lang_buf[1] = lc[1];
            lang_buf[2] = 0;
            g_lang = lang_buf;
            initialized = 1;
            return;
        }
    }
    initialized = 1;
    g_lang = NULL;
}

const char *x_translation(translation *t, const char *domain)
{
    if (!strcmp(domain, "ru")) {
        return t->l_ru;
    } else {
        return t->msg;
    }
}

const char *slgettext(const char *msg, const char *lang)
{
	int l = 0;
	int r = messages_cnt - 1;
	int c, cmp;
	/* Find message for this language: */

	if (!messages_cnt)
		return msg;

    if (!lang)
        return msg;

	while (r - l > 1) {
		c = (r + l) / 2;
		cmp = strcmp(messages[c].msg, msg);

		if (cmp == 0) {
			return x_translation(messages + c, lang);
		} else if (cmp < 0) {
			r = c;
		} else /* if (cmp > 0) */ {
			l = c;
		}
	}

	if (!strcmp(messages[r].msg, msg)) {
		return x_translation(messages + r, lang);
	} else if (!strcmp(messages[l].msg, msg)) {
		return x_translation(messages + l, lang);
	}

	return msg;
}

const char *sgettext(const char *msg)
{
	if (!initialized)
		sgettext_lang_init(NULL);

	return sdgettext(msg, g_lang);
}

