/*
 * Copyright (c) 2005 William Pitcock
 * Rights to this code are as documented in doc/LICENSE.
 *
 * Marking for channels.
 *
 * $Id: mark.c 2129 2005-09-05 00:59:19Z nenolod $
 */

#include "atheme.h"

DECLARE_MODULE_V1
(
	"chanserv/mark", FALSE, _modinit, _moddeinit,
	"$Id: mark.c 2129 2005-09-05 00:59:19Z nenolod $",
	"Atheme Development Group <http://www.atheme.org>"
);

static void cs_cmd_mark(char *origin);

command_t cs_mark = { "MARK", "Adds a note to a channel.",
			AC_IRCOP, cs_cmd_mark };

list_t *cs_cmdtree;

void _modinit(module_t *m)
{
	cs_cmdtree = module_locate_symbol("chanserv/main", "cs_cmdtree");
	command_add(&cs_mark, cs_cmdtree);
}

void _moddeinit()
{
	command_delete(&cs_mark, cs_cmdtree);
}

static void cs_cmd_mark(char *origin)
{
	char *target = strtok(NULL, " ");
	char *action = strtok(NULL, " ");
	char *info = strtok(NULL, "");
	mychan_t *mc;

	if (!target || !action)
	{
		notice(chansvs.nick, origin, "Insufficient parameters for \2MARK\2.");
		notice(chansvs.nick, origin, "Usage: MARK <#channel> <ON|OFF> [note]");
		return;
	}

	if (target[0] != '#')
	{
		notice(chansvs.nick, origin, "Invalid parameters specified for \2MARK\2.");
		return;
	}

	if (!(mc = mychan_find(target)))
	{
		notice(chansvs.nick, origin, "\2%s\2 is not registered.", target);
		return;
	}

	if (!strcasecmp(action, "ON"))
	{
		if (!info)
		{
			notice(chansvs.nick, origin, "Insufficient parameters for \2MARK\2.");
			notice(chansvs.nick, origin, "Usage: MARK <#channel> ON <note>");
			return;
		}

		if (metadata_find(mc, METADATA_CHANNEL, "private:mark:setter"))
		{
			notice(chansvs.nick, origin, "\2%s\2 is already marked.", target);
			return;
		}

		metadata_add(mc, METADATA_CHANNEL, "private:mark:setter", origin);
		metadata_add(mc, METADATA_CHANNEL, "private:mark:reason", info);
		metadata_add(mc, METADATA_CHANNEL, "private:mark:timestamp", itoa(CURRTIME));

		wallops("%s marked the channel \2%s\2.", origin, target);
		notice(chansvs.nick, origin, "\2%s\2 is now marked.", target);
	}
	else if (!strcasecmp(action, "OFF"))
	{
		if (!metadata_find(mc, METADATA_CHANNEL, "private:mark:setter"))
		{
			notice(chansvs.nick, origin, "\2%s\2 is not marked.", target);
			return;
		}

		metadata_delete(mc, METADATA_CHANNEL, "private:mark:setter");
		metadata_delete(mc, METADATA_CHANNEL, "private:mark:reason");
		metadata_delete(mc, METADATA_CHANNEL, "private:mark:timestamp");

		wallops("%s unmarked the channel \2%s\2.", origin, target);
		notice(chansvs.nick, origin, "\2%s\2 is now unmarked.", target);
	}
	else
	{
		notice(chansvs.nick, origin, "Invalid parameters for \2MARK\2.");
		notice(chansvs.nick, origin, "Usage: MARK <#channel> <ON|OFF> [note]");
	}
}
