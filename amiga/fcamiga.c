/* openamigafontconfig: the real name of an AmigaDOS name, for fcstr.c's
 * FcStrCanonFilename (patches/fontconfig-2.18.3-amiga-paths.patch).
 *
 * "PROGDIR:Fonts" is each program's own drawer, so kept as typed it gave
 * every program's fonts the same cache key. Lock() and NameFromLock() give
 * the real name ("Work:Programs/Viewer/Fonts"), and assigns resolve the
 * same way. NULL when the name can't be locked (not there yet, a volume
 * that isn't mounted): fcstr.c then keeps the name as typed. The result
 * is malloc()ed, as fontconfig frees its strings with free().
 *
 * MIT, Copyright (c) 2026 AmigaChrome Team. */
#include <dos/dosextens.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <stdlib.h>
#include <string.h>

unsigned char *FcAmigaRealName (const unsigned char *name);

unsigned char *
FcAmigaRealName (const unsigned char *name)
{
    struct Process *me = (struct Process *) FindTask (NULL);
    unsigned char *buf, *real;
    APTR win;
    BPTR lock;

    if (me->pr_Task.tc_Node.ln_Type != NT_PROCESS)
	return NULL;
    win = me->pr_WindowPtr;
    me->pr_WindowPtr = (APTR) -1;	/* no "insert volume" requester for a name that isn't there */
    lock = Lock ((STRPTR) name, ACCESS_READ);
    me->pr_WindowPtr = win;
    if (!lock)
	return NULL;
    buf = malloc (1024);
    if (buf && !NameFromLock (lock, (STRPTR) buf, 1024)) {
	free (buf);
	buf = NULL;
    }
    UnLock (lock);
    if (buf && (real = realloc (buf, strlen ((const char *) buf) + 1)))
	buf = real;
    return buf;
}
