#!/usr/bin/env python

"""
	Simple implementation of gettext utilities
"""
import sys
import re
import os, os.path
import shutil

class MessagesDB(object):
    _comment = re.compile("^\\s*#.*$")
    _msg = re.compile('^\\[(?P<text>([^\\]]|\\\\\\])*)\\]\\s*$')
    _dmsg = re.compile('^\\s*(?P<name>[a-zA-Z]+)\\s*=\\s*"(?P<text>([^"]|\\\\")*)"\\s*$')
    def __init__(self, filename = "messages.db"):
        " Create new messages.db instance "
        self.filename = filename
        self.messages = { }
        self.load()
        try:
            self.load()
        except:
            pass

    def load(self):
        f = open(self.filename, "rt")
        msg = ""

        for s in f:
            m = MessagesDB._msg.match(s)
            if m:
                msg = m.group("text").replace("\\]", "]")
            else:
                m = MessagesDB._dmsg.match(s)
                if m:
                    nm = m.group('name')
                    txt = m.group('text').replace('\\"', '"')

                    if msg not in self.messages:
                        self.messages[msg] = {}
                    self.messages[msg][nm] = txt
                else:
                    continue # TODO: error message?
        f.close()

    def _escape(self, m, s):
        return m.replace(s, "\\%s" % s)

    def save(self):
        if os.path.exists(self.filename): # Try to save copy:
            try:
                shutil.copyfile(self.filename, "%s~" % self.filename)
            except:
                pass

        f = open(self.filename, "wt")
        for msg in self.messages:
            f.write("\n[%s]\n" % self._escape(msg, "]"))
            for d in self.messages[msg]:
                f.write('%s = "%s"\n' % (d, self._escape(self.messages[msg][d], '"')))

        f.close()


def usage():
    print """Usage: %s command command_args
    Supported commands:
        grep [-e regexp] files
            Grep text messages from source files and update messages.db
        compile [filename]
            Compile messages.db to file. Filename used as base name, so if
            you say compile file file.h and file.c will be created. Default
            filename is messages
""" % sys.argv[0]

def scan_file(fn, rx, lst):
    " scan file for regular expression: "
    f = open(fn, "rt")
    txt = f.read()
    f.close()

    for s in rx.finditer(txt):
        lst[s.group("text")] = 1

def msg_grep(args):
    expr = 'T\\s*\\(\\s*"(?P<text>([^"]|\\\\")*)"\\)'
    if len(args) == 0:
        print >> sys.stderr, "Error: files not present in command line for grep"
        usage()
        sys.exit(1)
    i = 0
    while 1:
        if args[i] == '-e':
            i += 1
            expr = args[i]
        else:
            break

        i += 1

    if expr.index("(?P<text>") < 0:
        print >> sys.stderr, "Error: group 'text' is not present in regular expression"

    lst = {}
    rx = re.compile(expr, re.MULTILINE)
    for fn in args[i:]:
        scan_file(fn, rx, lst)

    db = MessagesDB()
    for m in lst:
        if m not in db.messages:
            db.messages[m] = {}
    db.save()

def c_str(s):
    return '"%s"' % s.replace('"', '\\"')

def msg_compile(args):
    if len(args) > 0:
        fn = args[0]
    else:
        fn = 'messages'

    db = MessagesDB()

    # We must collect all messages from domains:
    domains = [ ]
    for m in db.messages:
        for d in db.messages[m]:
            if d not in domains:
                domains.append(d)

    if len(domains) == 0:
        print "Warning: empty domains. Invalid messages?"

    f = open(fn + ".c", "wt")
    f.write("""
/* This file is generated automatically from messages.db */
#include <string.h>
#include <stdlib.h>

/* For compilers that does not support 'const' */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

typedef struct translation_st {
    char *msg;
%s
} translation;

""" % "".join(['    char *l_%s;\n' % s for s in domains]))
    # Write translations:
    msgs = [ m for m in db.messages ]
    msgs.sort()

    f.write("static translation messages[] = {\n")
    for m in msgs:
        f.write("    { %s" % c_str(m))
        for d in domains:
            if d in db.messages[m]:
                f.write(", %s" % c_str(db.messages[m][d]))
            else:
                f.write(", NULL")
        f.write(" },\n")
    f.write("    { NULL }\n};\n")

    f.write("""

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
    %s{
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

	return slgettext(msg, g_lang);
}

""" % "".join(["if (!strcmp(domain, %s)) {\n        return t->l_%s;\n    } else " % (c_str(d), d) for d in domains]))
    f.close() # C-file ready

    f = open("%s.h" % fn, "wt");
    f.write("""
#ifndef SGETTEXT___%s___H
#define SGETTEXT___%s___H

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
    """ % (fn, fn))
    f.close()

def main():
    if len(sys.argv) < 2:
        usage()
        sys.exit(1)
    elif sys.argv[1] == 'grep':
        msg_grep(sys.argv[2:])
    elif sys.argv[1] == 'compile':
        msg_compile(sys.argv[2:])
    else:
        print >> sys.stderr, "Error: unknown command %s" % sys.argv[1]
        usage()
        sys.exit(1)

if __name__ == '__main__':
    main()
