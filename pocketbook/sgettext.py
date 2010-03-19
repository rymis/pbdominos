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

def msg_compile(args):
    pass

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
