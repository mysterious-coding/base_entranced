// Copyright (C) 2000-2002 Raven Software, Inc.
//
/*****************************************************************************
 * name:		g_saga.c
 *
 * desc:		Game-side module for Siege gametype.
 *
 * $Author: Rich Whitehouse $ 
 * $Revision: 1.6 $
 *
 *****************************************************************************/
#include "g_local.h"
#include "bg_saga.h"

#define SIEGEITEM_STARTOFFRADAR 8


static char		team1[512];
static char		team2[512];

siegePers_t	g_siegePersistant = {qfalse, 0, 0};

int			imperial_goals_required = 0;
int			imperial_goals_completed = 0;
int			rebel_goals_required = 0;
int			rebel_goals_completed = 0;

int			imperial_time_limit = 0;
int			rebel_time_limit = 0;

int			gImperialCountdown = 0;
int			gRebelCountdown = 0;

int			rebel_attackers = 0;
int			imperial_attackers = 0;

int			winningteam = 0;

qboolean	gSiegeRoundBegun = qfalse;
qboolean	gSiegeRoundEnded = qfalse;
qboolean	gSiegeRoundWinningTeam = 0;
int			gSiegeBeginTime = Q3_INFINITE;

int			g_preroundState = 0; //default to starting as spec (1 is starting ingame)

void LogExit( const char *string );
void SetTeamQuick(gentity_t *ent, int team, qboolean doBegin);

static char gParseObjectives[MAX_SIEGE_INFO_SIZE];
static char gObjectiveCfgStr[1024];

#ifdef NEWMOD_SUPPORT
#define SIEGETIMER_FAKEOWNER 1023
void UpdateNewmodSiegeTimers(void)
{
	int n;
	for (n = 0; n < MAX_CLIENTS; n++)
	{
		if (&g_entities[n] && g_entities[n].inuse && g_entities[n].client)
		{
			gentity_t *te = G_TempEntity(g_entities[n].client->ps.origin, EV_SIEGESPEC);
			te->s.time = level.siegeRespawnCheck;
			te->s.owner = SIEGETIMER_FAKEOWNER;
			te->s.saberInFlight = qtrue;
		}
	}
}
#endif

static int CurrentSiegeRound(void) {
	if (!g_siegeTeamSwitch.integer)
		return 1;
	if (g_siegePersistant.beatingTime)
		return 2;
	return 1;
}

// returns the first incomplete objective for the specified round
int G_FirstIncompleteObjective(int round) {
	int i;
	for (i = 1; i <= MAX_STATS - 1; i++) {
		if (!trap_Cvar_VariableIntegerValue(va("siege_r%i_obj%i", round, i)))
			return i;
	}
	return 0;
}

// returns the first complete objective for the specified round
int G_FirstCompleteObjective(int round) {
	int i;
	for (i = 1; i <= MAX_STATS - 1; i++) {
		if (trap_Cvar_VariableIntegerValue(va("siege_r%i_obj%i", round, i)))
			return i;
	}
	return 0;
}

// convert milliseconds to time string
void G_ParseMilliseconds(int ms, char *outBuf, size_t outSize) {
	if (!outBuf)
		return;

	int elapsedSeconds, minutes, seconds;

	elapsedSeconds = ((ms + 500) / 1000); //convert milliseconds to seconds
	minutes = (elapsedSeconds / 60) % 60; //find minutes
	seconds = elapsedSeconds % 60; //find seconds

	Q_strncpyz(outBuf, va("%d:%02d", minutes, seconds), outSize);
}

// gets the objective that was completed just before the specified objective or time
int G_PreviousObjective(int objective, int round, int timeOverride) {
	int i, mostRecentObj = 0, mostRecentTime = 0;
	int inputObjTime = timeOverride ? timeOverride : trap_Cvar_VariableIntegerValue(va("siege_r%i_obj%i", round, objective));
	for (i = 1; i <= MAX_STATS - 1; i++) {
		if (i == objective)
			continue;
		int time = trap_Cvar_VariableIntegerValue(va("siege_r%i_obj%i", round, i));
		if (time && time < inputObjTime && time > mostRecentTime) {
			mostRecentObj = i;
			mostRecentTime = time;
		}
	}
	return mostRecentObj;
}

// used for "objective completed in XX:XX"
int G_ObjectiveTimeDifference(int objective, int round) {
	int thisTime = trap_Cvar_VariableIntegerValue(va("siege_r%i_obj%i", round, objective));
	int prevTime = trap_Cvar_VariableIntegerValue(va("siege_r%i_obj%i", round, G_PreviousObjective(objective, round, 0)));

	return abs(thisTime - prevTime);
}

// used for "held at objective for XX:XX"
static int HeldForMaxTime(void) {
	int round = CurrentSiegeRound();
	int heldAtObj = trap_Cvar_VariableIntegerValue(va("siege_r%i_heldformaxat", round));

	if (heldAtObj == 1)
		return abs(level.time - level.siegeRoundStartTime);
	else {
		int firstComplete = G_FirstCompleteObjective(round);
		if (!firstComplete)
			return abs(level.time - level.siegeRoundStartTime);
		else
			return abs(abs(level.time - level.siegeRoundStartTime) - abs(trap_Cvar_VariableIntegerValue(va("siege_r%i_obj%i", round, G_PreviousObjective(heldAtObj, round, abs(level.time - level.siegeRoundStartTime))))));
	}
}

// print time of the objective that was either just completed or you were held for a max at
static void PrintObjStat(int objective, int heldForMax) {
	if (!g_autoStats.integer || !g_siegeTeamSwitch.integer)
		return;

	int round = CurrentSiegeRound();
	int ms = heldForMax ? HeldForMaxTime() : G_ObjectiveTimeDifference(objective, round);

	if (ms == -1)
		return;

	if (heldForMax)
		trap_Cvar_Set(va("siege_r%i_heldformaxtime", round), va("%i", ms));

	char formattedTime[8] = { 0 };
	G_ParseMilliseconds(ms, formattedTime, sizeof(formattedTime));

	G_TeamCommand(TEAM_RED, va("print \"%s "S_COLOR_CYAN"%s"S_COLOR_WHITE".\n\"", heldForMax ? "Held at objective for" : "Objective completed in", formattedTime));
	G_TeamCommand(TEAM_BLUE, va("print \"Objective held for "S_COLOR_CYAN"%s"S_COLOR_WHITE".\n\"", formattedTime));
	G_TeamCommand(TEAM_SPECTATOR, va("print \"Objective %s "S_COLOR_CYAN"%s"S_COLOR_WHITE".\n\"", heldForMax ? "held for" : "completed in", formattedTime));
}

//go through all classes on a team and register their
//weapons and items for precaching.
void G_SiegeRegisterWeaponsAndHoldables(int team)
{
	siegeTeam_t *stm = BG_SiegeFindThemeForTeam(team);

	if (stm)
	{
		int i = 0;
		siegeClass_t *scl;
		while (i < stm->numClasses)
		{
			scl = stm->classes[i];

			if (scl)
			{
				int j = 0;
				while (j < WP_NUM_WEAPONS)
				{
					if (scl->weapons & (1 << j))
					{ //we use this weapon so register it.
						RegisterItem(BG_FindItemForWeapon(j));
					}
					j++;
				}
				j = 0;
				while (j < HI_NUM_HOLDABLE)
				{
					if (scl->invenItems & (1 << j))
					{ //we use this item so register it.
						RegisterItem(BG_FindItemForHoldable(j));
					}
					j++;
				}
			}
			i++;
		}
	}
}

//tell clients that this team won and print it on their scoreboard for intermission
//or whatever.
void SiegeSetCompleteData(int team)
{
	trap_SetConfigstring(CS_SIEGE_WINTEAM, va("%i", level.siegeMatchWinner ? OtherTeam(level.siegeMatchWinner) : team)); //duo: override the config string so that you don't erroneously see "team 2 won the match!" if both teams were held for a max
}

void InitSiegeMode(void)
{
	vmCvar_t		mapname;
	char			levelname[512];
	char			autolevelname[512];
	char			teamIcon[128];
	char			goalreq[64];
	char			teams[2048];
	char			objective[MAX_SIEGE_INFO_SIZE];
	char			objecStr[8192];
	int				len = 0;
	int				autolen = 0;
	int				i = 0;
	int				objectiveNumTeam1 = 0;
	int				objectiveNumTeam2 = 0;
	fileHandle_t	f;
	fileHandle_t	autofile;
	int				n;

	if (g_gametype.integer != GT_SIEGE)
	{
		goto failure;
	}

	if (!Q_stricmp(g_redTeam.string, "0"))
	{
		trap_Cvar_Set("g_redTeam", "none");
	}
	if (!Q_stricmp(g_blueTeam.string, "0"))
	{
		trap_Cvar_Set("g_blueTeam", "none");
	}

	level.hangarCompletedTime = 0;
	level.hangarLiftUsedByDefense = qfalse;
	level.ccCompleted = qfalse;
	level.objectiveJustCompleted = 0;
	level.totalObjectivesCompleted = 0;
	level.wallCompleted = qfalse;
	level.siegeRoundComplete = qfalse;
	memset(&level.narStationBreached, qfalse, sizeof(level.narStationBreached));
	level.killerOfLastDesertComputer = NULL;

	//reset
	SiegeSetCompleteData(0);

	//get pers data in case it existed from last level
	if (g_siegeTeamSwitch.integer)
	{
		trap_SiegePersGet(&g_siegePersistant);
		if (g_siegePersistant.beatingTime)
		{
			trap_SetConfigstring(CS_SIEGE_TIMEOVERRIDE, va("%i", g_siegePersistant.lastTime));
		}
		else
		{
			trap_SetConfigstring(CS_SIEGE_TIMEOVERRIDE, "0");
		}
	}
	else
	{ //hmm, ok, nothing.
		trap_SetConfigstring(CS_SIEGE_TIMEOVERRIDE, "0");
	}

	imperial_goals_completed = 0;
	rebel_goals_completed = 0;

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	for (n = 0; n < MAX_CLIENTS; n++)
	{
		if (&g_entities[n])
		{
			g_entities[n].forcedClass = 0;
			g_entities[n].forcedClassTime = 0;
			g_entities[n].funnyClassNumber = 0;
		}
	}

	if (g_autoKorribanFloatingItems.integer)
	{
		if (!Q_stricmp(mapname.string, "mp/siege_korriban"))
		{
			trap_Cvar_Set("g_floatingItems", "1");
		}
		else
		{
			trap_Cvar_Set("g_floatingItems", "0");
		}
	}

	if (g_autoKorribanSpam.integer)
	{
		if (!Q_stricmp(mapname.string, "mp/siege_korriban"))
		{
			trap_Cvar_Set("iLikeToDoorSpam", "1");
			trap_Cvar_Set("iLikeToMineSpam", "1");
		}
		else
		{
			trap_Cvar_Set("iLikeToDoorSpam", "0");
			trap_Cvar_Set("iLikeToMineSpam", "0");
		}
	}

	if (autocfg_map.integer)
	{
		Com_sprintf(autolevelname, sizeof(autolevelname), "mapcfgs/%s.cfg\0", mapname.string);

		if (!autolevelname[0])
		{
			G_LogPrintf("g_saga: unknown autolevelname error!\n");
			goto afterauto;
		}

		autolen = trap_FS_FOpenFile(autolevelname, &autofile, FS_READ);

		if (autofile)
		{
			trap_SendConsoleCommand(EXEC_APPEND, va("exec %s\n", autolevelname));
		}
		else
		{
			if (autocfg_unknown.integer)
			{
				Com_sprintf(autolevelname, sizeof(autolevelname), "mapcfgs/unknown.cfg\0", mapname.string);
				autolen = trap_FS_FOpenFile(autolevelname, &autofile, FS_READ);
				if (autofile)
				{
					trap_SendConsoleCommand(EXEC_APPEND, va("exec %s\n", autolevelname));
				}
				else
				{
					G_LogPrintf("Unable to find %s.\n", autolevelname);
				}
			}
			else
			{
				G_LogPrintf("Unable to find %s.\n", autolevelname);
			}
		}
	}

	afterauto:

	Com_sprintf(levelname, sizeof(levelname), "maps/%s.siege\0", mapname.string);

	if (/*!levelname ||*/ !levelname[0])
	{
		goto failure;
	}

	len = trap_FS_FOpenFile(levelname, &f, FS_READ);

	if (!f || len >= MAX_SIEGE_INFO_SIZE)
	{
		goto failure;
	}

	trap_FS_Read(siege_info, len, f);

	trap_FS_FCloseFile(f);

	siege_valid = 1;

	//See if players should be specs or ingame preround
	if (BG_SiegeGetPairedValue(siege_info, "preround_state", teams))
	{
		if (teams[0])
		{
			g_preroundState = atoi(teams);
		}
	}

	if (BG_SiegeGetValueGroup(siege_info, "Teams", teams))
	{
		BG_SiegeGetPairedValue(teams, "team1", team1);
		BG_SiegeGetPairedValue(teams, "team2", team2);
	}
	else
	{
		G_Error("Siege teams not defined");
	}

	if (BG_SiegeGetValueGroup(siege_info, team2, gParseObjectives))
	{
		if (BG_SiegeGetPairedValue(gParseObjectives, "TeamIcon", teamIcon))
		{
			trap_Cvar_Set( "team2_icon", teamIcon);
		}

		if (BG_SiegeGetPairedValue(gParseObjectives, "RequiredObjectives", goalreq))
		{
			rebel_goals_required = atoi(goalreq);
		}
		if (BG_SiegeGetPairedValue(gParseObjectives, "Timed", goalreq))
		{
			rebel_time_limit = atoi(goalreq)*1000;
			if (g_siegeTeamSwitch.integer &&
				g_siegePersistant.beatingTime)
			{
				gRebelCountdown = level.time + g_siegePersistant.lastTime;
			}
			else
			{
				gRebelCountdown = level.time + rebel_time_limit;
			}
		}
		if (BG_SiegeGetPairedValue(gParseObjectives, "attackers", goalreq))
		{
			rebel_attackers = atoi(goalreq);
		}
	}

	if (BG_SiegeGetValueGroup(siege_info, team1, gParseObjectives))
	{

		if (BG_SiegeGetPairedValue(gParseObjectives, "TeamIcon", teamIcon))
		{
			trap_Cvar_Set( "team1_icon", teamIcon);
		}

		if (BG_SiegeGetPairedValue(gParseObjectives, "RequiredObjectives", goalreq))
		{
			imperial_goals_required = atoi(goalreq);
		}
		if (BG_SiegeGetPairedValue(gParseObjectives, "Timed", goalreq))
		{
			if (rebel_time_limit)
			{
				Com_Printf("Tried to set imperial time limit, but there's already a rebel time limit!\nOnly one team can have a time limit.\n");
			}
			else
			{
				imperial_time_limit = atoi(goalreq)*1000;
				if (g_siegeTeamSwitch.integer &&
					g_siegePersistant.beatingTime)
				{
					gImperialCountdown = level.time + g_siegePersistant.lastTime;
				}
				else
				{
					gImperialCountdown = level.time + imperial_time_limit;
				}
			}
		}
		if (BG_SiegeGetPairedValue(gParseObjectives, "attackers", goalreq))
		{
			imperial_attackers = atoi(goalreq);
		}
	}

	//Load the player class types
	BG_SiegeLoadClasses(NULL);

	if (!bgNumSiegeClasses)
	{ //We didn't find any?!
		G_Error("Couldn't find any player classes for Siege");
	}

	//Ok, I'm adding inventory item precaching now, so I'm finally going to optimize this
	//to only do weapons/items for the current teams used on the level.

	//Now load the teams since we have class data.
	BG_SiegeLoadTeams();

	if (!bgNumSiegeTeams)
	{ //React same as with classes.
		G_Error("Couldn't find any player teams for Siege");
	}

	//Get and set the team themes for each team. This will control which classes can be
	//used on each team.
	if (BG_SiegeGetValueGroup(siege_info, team1, gParseObjectives))
	{
		if (BG_SiegeGetPairedValue(gParseObjectives, "UseTeam", goalreq))
		{
			if (g_redTeam.string[0] && Q_stricmp(g_redTeam.string, "none"))
			{
				BG_SiegeSetTeamTheme(SIEGETEAM_TEAM1, g_redTeam.string, goalreq);
			}
			else
			{
				BG_SiegeSetTeamTheme(SIEGETEAM_TEAM1, goalreq, goalreq);
			}
		}

		//Now count up the objectives for this team.
		i = 1;
		strcpy(objecStr, va("Objective%i", i));
		while (BG_SiegeGetValueGroup(gParseObjectives, objecStr, objective))
		{
			objectiveNumTeam1++;
			i++;
			strcpy(objecStr, va("Objective%i", i));
		}
	}
	if (BG_SiegeGetValueGroup(siege_info, team2, gParseObjectives))
	{
		if (BG_SiegeGetPairedValue(gParseObjectives, "UseTeam", goalreq))
		{
			if (g_blueTeam.string[0] && Q_stricmp(g_blueTeam.string, "none"))
			{
				BG_SiegeSetTeamTheme(SIEGETEAM_TEAM2, g_blueTeam.string, goalreq);
			}
			else
			{
				BG_SiegeSetTeamTheme(SIEGETEAM_TEAM2, goalreq, goalreq);
			}
		}

		//Now count up the objectives for this team.
		i = 1;
		strcpy(objecStr, va("Objective%i", i));
		while (BG_SiegeGetValueGroup(gParseObjectives, objecStr, objective))
		{
			objectiveNumTeam2++;
			i++;
			strcpy(objecStr, va("Objective%i", i));
		}
	}

	//Set the configstring to show status of all current objectives
	strcpy(gObjectiveCfgStr, "t1");
	while (objectiveNumTeam1 > 0)
	{ //mark them all as not completed since we just initialized
		Q_strcat(gObjectiveCfgStr, 1024, "-0");
		objectiveNumTeam1--;
	}
	//Finished doing team 1's objectives, now do team 2's
	Q_strcat(gObjectiveCfgStr, 1024, "|t2");
	while (objectiveNumTeam2 > 0)
	{
		Q_strcat(gObjectiveCfgStr, 1024, "-0");
		objectiveNumTeam2--;
	}

	//And finally set the actual config string
	trap_SetConfigstring(CS_SIEGE_OBJECTIVES, gObjectiveCfgStr);

	//precache saber data for classes that use sabers on both teams
	BG_PrecacheSabersForSiegeTeam(SIEGETEAM_TEAM1);
	BG_PrecacheSabersForSiegeTeam(SIEGETEAM_TEAM2);

	G_SiegeRegisterWeaponsAndHoldables(SIEGETEAM_TEAM1);
	G_SiegeRegisterWeaponsAndHoldables(SIEGETEAM_TEAM2);

	return;

failure:
	siege_valid = 0;
}

void G_SiegeSetObjectiveComplete(int team, int objective, qboolean failIt)
{
	char *p = NULL;
	int onObjective = 0;

	if (team == SIEGETEAM_TEAM1)
	{
		p = strstr(gObjectiveCfgStr, "t1");
	}
	else if (team == SIEGETEAM_TEAM2)
	{
		p = strstr(gObjectiveCfgStr, "t2");
	}

	if (!p)
	{
		assert(0);
		return;
	}

	//Parse from the beginning of this team's objectives until we get to the desired objective
	//number.
	while (p && *p && *p != '|')
	{
		if (*p == '-')
		{
			onObjective++;
		}

		if (onObjective == objective)
		{ //this is the one we want
			//Move to the next char, the status of this objective
			p++;

			//Now change it from '0' to '1' if we are completeing the objective
			//or vice versa if the objective has been taken away
			if (failIt)
			{
				*p = '0';
			}
			else
			{
				*p = '1';
			}
			break;
		}

		p++;
	}

	//Now re-update the configstring.
	trap_SetConfigstring(CS_SIEGE_OBJECTIVES, gObjectiveCfgStr);
}

//Returns qtrue if objective complete currently, otherwise qfalse
qboolean G_SiegeGetCompletionStatus(int team, int objective)
{
	char *p = NULL;
	int onObjective = 0;

	if (team == SIEGETEAM_TEAM1)
	{
		p = strstr(gObjectiveCfgStr, "t1");
	}
	else if (team == SIEGETEAM_TEAM2)
	{
		p = strstr(gObjectiveCfgStr, "t2");
	}

	if (!p)
	{
		assert(0);
		return qfalse;
	}

	//Parse from the beginning of this team's objectives until we get to the desired objective
	//number.
	while (p && *p && *p != '|')
	{
		if (*p == '-')
		{
			onObjective++;
		}

		if (onObjective == objective)
		{ //this is the one we want
			//Move to the next char, the status of this objective
			p++;

			//return qtrue if it's '1', qfalse if it's anything else
			if (*p == '1')
			{
				return qtrue;
			}
			else
			{
				return qfalse;
			}
			break;
		}

		p++;
	}

	return qfalse;
}

void UseSiegeTarget(gentity_t *other, gentity_t *en, char *target)
{ //actually use the player which triggered the object which triggered the siege objective to trigger the target
	gentity_t		*t;
	gentity_t		*ent;

	if ( !en || !en->client )
	{ //looks like we don't have access to a player, so just use the activating entity
		ent = other;
	}
	else
	{
		ent = en;
	}

	if (!en)
	{
		return;
	}

	if ( !target )
	{
		return;
	}

	t = NULL;
	while ( (t = G_Find (t, FOFS(targetname), target)) != NULL )
	{
		if ( t == ent )
		{
			G_Printf ("WARNING: Entity used itself.\n");
		}
		else
		{
			if ( t->use )
			{
				GlobalUse(t, ent, ent);
			}
		}
		if ( !ent->inuse )
		{
			G_Printf("entity was removed while using targets\n");
			return;
		}
	}
}

void SiegeBroadcast_OBJECTIVECOMPLETE(int team, int client, int objective)
{
	gentity_t *te;
	vec3_t nomatter;

	VectorClear(nomatter);

	te = G_TempEntity( nomatter, EV_SIEGE_OBJECTIVECOMPLETE );
	te->r.svFlags |= SVF_BROADCAST;
	te->s.eventParm = team;
	te->s.weapon = client;
	te->s.trickedentindex = objective;
}

void SiegeBroadcast_ROUNDOVER(int winningteam, int winningclient)
{
	gentity_t *te;
	vec3_t nomatter;

	VectorClear(nomatter);

	te = G_TempEntity( nomatter, EV_SIEGE_ROUNDOVER );
	te->r.svFlags |= SVF_BROADCAST;
	te->s.eventParm = winningteam;
	te->s.weapon = winningclient;
}

void BroadcastObjectiveCompletion(int team, int objective, int final, int client)
{
	if (client != ENTITYNUM_NONE && g_entities[client].client && g_entities[client].client->sess.sessionTeam == team)
	{ //guy who completed this objective gets points, providing he's on the opposing team
		AddScore(&g_entities[client], g_entities[client].client->ps.origin, g_fixSiegeScoring.integer ? SIEGE_POINTS_OBJECTIVECOMPLETED_NEW : SIEGE_POINTS_OBJECTIVECOMPLETED);
	}
	SiegeBroadcast_OBJECTIVECOMPLETE(team, client, objective);
}

void AddSiegeWinningTeamPoints(int team, int winner)
{
	int i = 0;
	gentity_t *ent;

	while (i < MAX_CLIENTS)
	{
		ent = &g_entities[i];

		if (ent && ent->client && ent->client->sess.sessionTeam == team)
		{
			if (i == winner)
			{
				AddScore(ent, ent->client->ps.origin, g_fixSiegeScoring.integer ? SIEGE_POINTS_TEAMWONROUND_NEW+SIEGE_POINTS_FINALOBJECTIVECOMPLETED_NEW : SIEGE_POINTS_TEAMWONROUND+SIEGE_POINTS_FINALOBJECTIVECOMPLETED);
			}
			else
			{
				AddScore(ent, ent->client->ps.origin, g_fixSiegeScoring.integer ? SIEGE_POINTS_TEAMWONROUND_NEW : SIEGE_POINTS_TEAMWONROUND);
			}
		}

		i++;
	}
}

void SiegeClearSwitchData(void)
{
	memset(&g_siegePersistant, 0, sizeof(g_siegePersistant));
	trap_SiegePersSet(&g_siegePersistant);
	level.siegeRoundStartTime = 0;
	level.antiLamingTime = 0;
}

void SiegeDoTeamAssign(void)
{
	int i = 0;
	gentity_t *ent;

	//yeah, this is great...
	while (i < MAX_CLIENTS)
	{
		ent = &g_entities[i];

		if (ent->inuse && ent->client &&
			ent->client->pers.connected == CON_CONNECTED)
		{ //a connected client, switch his frickin teams around
			if (ent->client->sess.siegeDesiredTeam == SIEGETEAM_TEAM1)
			{
				ent->client->sess.siegeDesiredTeam = SIEGETEAM_TEAM2;
			}
			else if (ent->client->sess.siegeDesiredTeam == SIEGETEAM_TEAM2)
			{
				ent->client->sess.siegeDesiredTeam = SIEGETEAM_TEAM1;
			}

			if (ent->client->sess.sessionTeam == SIEGETEAM_TEAM1)
			{
				SetTeamQuick(ent, SIEGETEAM_TEAM2, qfalse);
			}
			else if (ent->client->sess.sessionTeam == SIEGETEAM_TEAM2)
			{
				SetTeamQuick(ent, SIEGETEAM_TEAM1, qfalse);
			}
		}
		i++;
	}
}

void SiegeTeamSwitch(int winTeam, int winTime)
{
	trap_SiegePersGet(&g_siegePersistant);
	if (g_siegePersistant.beatingTime)
	{ //was already in "switched" mode, change back
		//announce the winning team.
		//either the first team won again, or the second
		//team beat the time set by the initial team. In any
		//case the winTeam here is the overall winning team.
		SiegeSetCompleteData(winTeam);
		SiegeClearSwitchData();
	}
	else
	{ //go into "beat their time" mode
		g_siegePersistant.beatingTime = qtrue;
        g_siegePersistant.lastTeam = winTeam;
		g_siegePersistant.lastTime = winTime;

		trap_SiegePersSet(&g_siegePersistant);
	}
}

static void ComputeSiegePlayTimes(void) {
	int i;
	for (i = 0; i < MAX_CLIENTS; i++) {
		gclient_t *cl = &level.clients[i];
		if (cl->pers.connected != CON_CONNECTED || (cl->sess.sessionTeam != TEAM_RED && cl->sess.sessionTeam != TEAM_BLUE))
			continue;
		if (cl->sess.sessionTeam == TEAM_RED)
			cl->sess.siegeStats.oTime[GetSiegeStatRound()] = level.time - cl->pers.enterTime;
		else
			cl->sess.siegeStats.dTime[GetSiegeStatRound()] = level.time - cl->pers.enterTime;
	}
}

void G_SiegeRoundComplete(int winningteam, int winningclient)
{
	ComputeSiegePlayTimes();

	vec3_t nomatter;
	char teamstr[1024];
	int originalWinningClient = winningclient;

	level.antiLamingTime = 0;
	level.siegeRoundComplete = qtrue;

	if (level.siegeStage == SIEGESTAGE_ROUND1) {
		level.siegeStage = SIEGESTAGE_ROUND1POSTGAME;
		int realTime = 0;
		if (g_siegeTeamSwitch.integer && (imperial_time_limit || rebel_time_limit)) {
			if (imperial_time_limit)
				realTime = imperial_time_limit - (gImperialCountdown - level.time);
			else if (rebel_time_limit)
				realTime = rebel_time_limit - (gRebelCountdown - level.time);
			if (realTime < 1)
				realTime = 1;
		}
		trap_Cvar_Set("siege_r1_total", va("%i", realTime ? realTime : abs(level.time - level.siegeRoundStartTime)));
	}
	else if (level.siegeStage == SIEGESTAGE_ROUND2) {
		level.siegeStage = SIEGESTAGE_ROUND2POSTGAME;
		if (!level.siegeMatchWinner)
			level.siegeMatchWinner = OtherTeam(winningteam);
		trap_Cvar_Set("siege_r2_total", va("%i", abs(level.time - level.siegeRoundStartTime)));
	}

	level.siegeRoundStartTime = 0;

	if (winningclient != ENTITYNUM_NONE && g_entities[winningclient].client &&
		g_entities[winningclient].client->sess.sessionTeam != winningteam)
	{ //this person just won the round for the other team..
		winningclient = ENTITYNUM_NONE;
	}

	VectorClear(nomatter);

	SiegeBroadcast_ROUNDOVER(winningteam, winningclient);

	AddSiegeWinningTeamPoints(winningteam, winningclient);

	//Instead of exiting like this, fire off a target, and let it handle things.
	//Can be a script or whatever the designer wants.
   	if (winningteam == SIEGETEAM_TEAM1)
	{
		Com_sprintf(teamstr, sizeof(teamstr), team1);
	}
	else
	{
		Com_sprintf(teamstr, sizeof(teamstr), team2);
	}

	trap_SetConfigstring(CS_SIEGE_STATE, va("3|%i", level.time)); //ended
	gSiegeRoundBegun = qfalse;
	gSiegeRoundEnded = qtrue;
	gSiegeRoundWinningTeam = winningteam;

	if (BG_SiegeGetValueGroup(siege_info, teamstr, gParseObjectives))
	{
		if (!BG_SiegeGetPairedValue(gParseObjectives, "roundover_target", teamstr))
		{ //didn't find the name of the thing to target upon win, just logexit now then.
			LogExit( "Objectives completed" );
			//return;
			//returning here breaks round 2 timer if there's no target_siege_end
		}
		
		if (originalWinningClient && originalWinningClient == ENTITYNUM_NONE) //add null check
		{ //oh well, just find something active and use it then.
            int i = 0;
			gentity_t *ent;

			while (i < MAX_CLIENTS)
			{
				ent = &g_entities[i];

				if (ent->inuse)
				{ //sure, you'll do.
                    originalWinningClient = ent->s.number;
					break;
				}

				i++;
			}
		}
		G_UseTargets2(&g_entities[originalWinningClient], &g_entities[originalWinningClient], teamstr);
	}

	if (g_siegeTeamSwitch.integer &&
		(imperial_time_limit || rebel_time_limit))
	{ //handle stupid team switching crap
		int time = 0;
		if (imperial_time_limit)
		{
			time = imperial_time_limit-(gImperialCountdown-level.time);
		}
		else if (rebel_time_limit)
		{
			time = rebel_time_limit-(gRebelCountdown-level.time);
		}

		if (time < 1)
		{
			time = 1;
		}
		SiegeTeamSwitch(winningteam, time);
	}
	else
	{ //assure it's clear for next round
		SiegeClearSwitchData();
	}
}

void G_ValidateSiegeClassForTeam(gentity_t *ent, int team)
{
	siegeClass_t *scl;
	siegeTeam_t *stm;
	int newClassIndex = -1;
	if (ent->client->siegeClass == -1)
	{ //uh.. sure.
		return;
	}

	scl = &bgSiegeClasses[ent->client->siegeClass];

	stm = BG_SiegeFindThemeForTeam(team);
	if (stm)
	{
		int i = 0;

		while (i < stm->numClasses)
		{ //go through the team and see its valid classes, can we find one that matches our current player class?
			if (stm->classes[i])
			{
				if (!Q_stricmp(scl->name, stm->classes[i]->name))
				{ //the class we're using is already ok for this team.
					return;
				}
				if (stm->classes[i]->playerClass == scl->playerClass ||
					newClassIndex == -1)
				{
					newClassIndex = i;
				}
			}
			i++;
		}

		if (newClassIndex != -1)
		{ //ok, let's find it in the global class array
			ent->client->siegeClass = BG_SiegeFindClassIndexByName(stm->classes[newClassIndex]->name);
			strcpy(ent->client->sess.siegeClass, stm->classes[newClassIndex]->name);
		}
	}
}

//bypass most of the normal checks in SetTeam
void SetTeamQuick(gentity_t *ent, int team, qboolean doBegin)
{
	char userinfo[MAX_INFO_STRING];

	trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );

	if (g_gametype.integer == GT_SIEGE)
	{
		G_ValidateSiegeClassForTeam(ent, team);
	}

	ent->client->sess.sessionTeam = team;

	if (team == TEAM_SPECTATOR)
	{
		ent->client->sess.spectatorState = SPECTATOR_FREE;
		Info_SetValueForKey(userinfo, "team", "s");
	}
	else
	{
		ent->client->sess.spectatorState = SPECTATOR_NOT;
		if (team == TEAM_RED)
		{
			Info_SetValueForKey(userinfo, "team", "r");
		}
		else if (team == TEAM_BLUE)
		{
			Info_SetValueForKey(userinfo, "team", "b");
		}
		else
		{
			Info_SetValueForKey(userinfo, "team", "?");
		}
	}

	trap_SetUserinfo( ent->s.number, userinfo );

	ent->client->sess.spectatorClient = 0;

	ent->client->pers.teamState.state = TEAM_BEGIN;

	ClientUserinfoChanged( ent->s.number );

	if (doBegin)
	{
		ClientBegin( ent->s.number, qfalse );
	}
}

extern void SetSiegeClass(gentity_t *ent, char* className);

// account for raven being inconsistent
static int GetStupidClassNumber(int in) {
	switch (in) {
	case SPC_INFANTRY:		return 1;
	case SPC_HEAVY_WEAPONS:	return 2;
	case SPC_DEMOLITIONIST:	return 3;
	case SPC_VANGUARD:		return 4;
	case SPC_SUPPORT:		return 5;
	case SPC_JEDI:			return 6;
	}
	return 1;
}

static char *classNames[] = { "Assault", "Scout", "Tech", "Jedi", "Demolitions", "Heavy Weapons", "another class" };
void G_ChangePlayerFromExceededClass(gentity_t *ent) {
	if (!g_classLimits.integer)
		return;
	int current = bgSiegeClasses[ent->client->siegeClass].playerClass, lastLegit = level.lastLegitClass[ent - g_entities];

	// first, see if we can change back to our previous class
	siegeClass_t *new = BG_SiegeGetClass(ent->client->sess.siegeDesiredTeam, GetStupidClassNumber(lastLegit));
	if (new && lastLegit != -1 && !G_WouldExceedClassLimit(ent->client->sess.siegeDesiredTeam, lastLegit, qtrue, -1, NULL)) {
		trap_SendServerCommand(ent - g_entities, va("print \"The class you selected is full. You were automatically changed back to %s.\n\"", classNames[Com_Clampi(SPC_INFANTRY, SPC_MAX, new->playerClass)]));
		SetSiegeClass(ent, new->name);
		return;
	}
	else { // we were unable to switch back to our previous class; try to switch to something else
		int i, classPreferenceOrder[] = { SPC_INFANTRY, SPC_HEAVY_WEAPONS, SPC_DEMOLITIONIST, SPC_SUPPORT, SPC_VANGUARD, SPC_JEDI };
		for (i = 0; i < 6; i++) {
			new = BG_SiegeGetClass(ent->client->sess.siegeDesiredTeam, GetStupidClassNumber(classPreferenceOrder[i]));
			if (!new || new->playerClass == current || G_WouldExceedClassLimit(ent->client->sess.siegeDesiredTeam, new->playerClass, qtrue, -1, NULL))
				continue;
			// we found a valid target to switch to
			trap_SendServerCommand(ent - g_entities, va("print \"The class you selected is full. You were automatically changed to %s.\n\"", classNames[Com_Clampi(SPC_INFANTRY, SPC_MAX, new->playerClass)]));
			SetSiegeClass(ent, new->name);
			return;
		}
	}
	// morons playing game larger than 6v6 with limit of 1 on each class...
	Com_Error(ERR_DROP, "G_ChangePlayerFromExceededClass: unable to find a class that wouldn't exceed limit!");
}

static void CheckForClassesExceedingLimits(team_t team) {
	assert(team == TEAM_RED || team == TEAM_BLUE);
	int i;
	for (i = 0; i < SPC_MAX; i++) {
		int tries = 0;
		while (G_WouldExceedClassLimit(team, i, qfalse, -1, NULL)) {
			// this class's limit is exceeded; switch one player at a time to another class until the limit is no longer exceeded.

			if (tries > MAX_CLIENTS) // avoid infinite loop
				Com_Error(ERR_DROP, va("CheckForClassesExceedingLimits: recursive error for team %s, class %s)!", team == TEAM_RED ? "red" : "blue", classNames[i]));
			tries++;
			
			// method #1: the (dead) player who changed to it most recently gets switched off. should cover most real-world cases.
			int j, mostRecentChange = -1, mostRecentSwitcher = -1;
			for (j = 0; j < MAX_CLIENTS; j++) {
				gentity_t *dude = &g_entities[j];
				if (!dude || !dude->client || dude->client->sess.siegeDesiredTeam != team || bgSiegeClasses[dude->client->siegeClass].playerClass != i || level.tryChangeClass[j].class != i || level.tryChangeClass[j].team != team || (!level.inSiegeCountdown && dude->health > 0 && !(dude->client->tempSpectate > level.time)))
					continue;
				if (level.tryChangeClass[j].time > mostRecentChange) {
					mostRecentChange = level.tryChangeClass[j].time;
					mostRecentSwitcher = j;
				}
			}
			if (mostRecentSwitcher != -1) {
				G_ChangePlayerFromExceededClass(&g_entities[mostRecentSwitcher]);
				continue;
			}

			// method #2: the dead player who changed teams most recently gets switched off.
			int mostRecentTeamChange = -1, mostRecentTeamChanger = -1;
			for (j = 0; j < MAX_CLIENTS; j++) {
				gentity_t *dude = &g_entities[j];
				if (!dude || !dude->client || dude->client->sess.siegeDesiredTeam != team || bgSiegeClasses[dude->client->siegeClass].playerClass != i || (!level.inSiegeCountdown && dude->health > 0 && !(dude->client->tempSpectate > level.time)) || !level.teamChangeTime[j])
					continue;
				if (level.teamChangeTime[j] > mostRecentTeamChange) {
					mostRecentTeamChange = level.teamChangeTime[j];
					mostRecentTeamChanger = j;
				}
			}
			if (mostRecentTeamChanger != -1) {
				G_ChangePlayerFromExceededClass(&g_entities[mostRecentTeamChanger]);
				continue;
			}

			// method #3: the alive player who changed teams most recently gets switched off.
			mostRecentTeamChange = -1, mostRecentTeamChanger = -1;
			for (j = 0; j < MAX_CLIENTS; j++) {
				gentity_t *dude = &g_entities[j];
				if (!dude || !dude->client || dude->client->sess.siegeDesiredTeam != team || bgSiegeClasses[dude->client->siegeClass].playerClass != i || !level.teamChangeTime[j])
					continue;
				if (level.teamChangeTime[j] > mostRecentTeamChange) {
					mostRecentTeamChange = level.teamChangeTime[j];
					mostRecentTeamChanger = j;
				}
			}
			if (mostRecentTeamChanger != -1) {
				G_ChangePlayerFromExceededClass(&g_entities[mostRecentTeamChanger]);
				continue;
			}

			// fallback/sanity check; just pick someone.
			int selectedGuy = -1;
			for (j = 0; j < MAX_CLIENTS; j++) {
				gentity_t *dude = &g_entities[j];
				if (!dude || !dude->client || dude->client->sess.siegeDesiredTeam != team || bgSiegeClasses[dude->client->siegeClass].playerClass != i)
					continue;
				selectedGuy = j;
				break;
			}
			if (selectedGuy != -1) {
				G_ChangePlayerFromExceededClass(&g_entities[selectedGuy]);
				continue;
			}
		}
	}

	// sanity check; make sure that we actually resolved everything (i.e., that we didn't just shift everyone around to other limited classes)
	for (i = 0; i < SPC_MAX; i++) {
		if (G_WouldExceedClassLimit(team, i, qfalse, -1, NULL))
			Com_Error(ERR_DROP, va("CheckForClassesExceedingLimits: unable to resolve class limits for team %s, class %s!", team == TEAM_RED ? "red" : "blue", classNames[i]));
	}
}

void SiegeRespawn(gentity_t *ent)
{
	gentity_t *tent;

	if (g_classLimits.integer)
		CheckForClassesExceedingLimits(ent->client->sess.siegeDesiredTeam);

	level.lastLegitClass[ent - g_entities] = bgSiegeClasses[ent->client->siegeClass].playerClass;

	qboolean needUpdateInfo = g_delayClassUpdate.integer && (Q_stricmp(ent->client->sess.spawnedSiegeClass, ent->client->sess.siegeClass) || Q_stricmp(ent->client->sess.spawnedSiegeModel, ent->client->modelname)) ? qtrue : qfalse;

	if (ent->client->sess.siegeClass[0])
		Q_strncpyz(ent->client->sess.spawnedSiegeClass, ent->client->sess.siegeClass, sizeof(ent->client->sess.spawnedSiegeClass));
	else
		memset(&ent->client->sess.spawnedSiegeClass, 0, sizeof(ent->client->sess.spawnedSiegeClass));
	siegeClass_t *scl = BG_SiegeFindClassByName(ent->client->sess.spawnedSiegeClass);
	if (scl && scl->forcedModel[0])
		Q_strncpyz(ent->client->sess.spawnedSiegeModel, scl->forcedModel, sizeof(ent->client->sess.spawnedSiegeModel));
	else
		memset(&ent->client->sess.spawnedSiegeModel, 0, sizeof(ent->client->sess.spawnedSiegeModel));

	if (needUpdateInfo)
		ClientUserinfoChanged(ent - g_entities);

	if (ent->client->sess.sessionTeam != ent->client->sess.siegeDesiredTeam)
	{
		SetTeamQuick(ent, ent->client->sess.siegeDesiredTeam, qtrue);
	}
	else
	{
		ClientSpawn(ent);
		// add a teleportation effect
		tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
		tent->s.clientNum = ent->s.clientNum;
	}
}

void SiegeBeginRound(int entNum)
{ //entNum is just used as something to fire targets from.
	char targname[1024];
	memset(&level.lastLegitClass, -1, sizeof(level.lastLegitClass));

	if (!g_preroundState)
	{ //if players are not ingame on round start then respawn them now
		int i = 0;
		gentity_t *ent;
		level.inSiegeCountdown = qfalse;
		level.siegeRoundComplete = qfalse;
		if (!g_siegeTeamSwitch.integer)
			level.siegeStage = SIEGESTAGE_NONE;
		else if (CurrentSiegeRound() == 1)
			level.siegeStage = SIEGESTAGE_ROUND1;
		else
			level.siegeStage = SIEGESTAGE_ROUND2;
		//respawn everyone now
		level.siegeRespawnCheck = level.time + g_siegeRespawn.integer * 1000 - SIEGE_ROUND_BEGIN_TIME - 200;

#ifdef NEWMOD_SUPPORT
		UpdateNewmodSiegeTimers();
#endif

		// determine who needs to be spawned
		qboolean spawnEnt[MAX_CLIENTS] = { qfalse };
		for (i = 0; i < MAX_CLIENTS; i++) {
			ent = &g_entities[i];

			if (ent->inuse && ent->client) {
				if (ent->client->sess.sessionTeam != TEAM_SPECTATOR && !(ent->client->ps.pm_flags & PMF_FOLLOW))
					spawnEnt[i] = qtrue; //not a spec, just respawn them
				else if (ent->client->sess.sessionTeam == TEAM_SPECTATOR &&
					(ent->client->sess.siegeDesiredTeam == TEAM_RED || ent->client->sess.siegeDesiredTeam == TEAM_BLUE))
					spawnEnt[i] = qtrue; //spectator but has a desired team
			}
		}

		// switch people off classes that are exceeding their limits
		if (g_classLimits.integer) {
			CheckForClassesExceedingLimits(TEAM_RED);
			CheckForClassesExceedingLimits(TEAM_BLUE);
		}

		// spawn everyone who needs to be spawned
		for (i = 0; i < MAX_CLIENTS; i++) {
			if (spawnEnt[i])
				SiegeRespawn(&g_entities[i]);
		}
	}

	//Now check if there's something to fire off at the round start, if so do it.
	if (BG_SiegeGetPairedValue(siege_info, "roundbegin_target", targname))
	{
		if (targname[0])
		{
			G_UseTargets2(&g_entities[entNum], &g_entities[entNum], targname);
		}
	}

	vmCvar_t	mapname;
	trap_Cvar_Register(&mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM);

	if (!Q_stricmp(mapname.string, "siege_codes"))
	{
		//hacky fix to remove icons for useless ammo generators that shouldn't be in the map
		SetIconFromClassname("misc_ammo_floor_unit", 2, qfalse);
		SetIconFromClassname("misc_ammo_floor_unit", 3, qfalse);
	}
	else if (!Q_stricmp(mapname.string, "siege_narshaddaa"))
	{
		SetIconFromClassname("misc_ammo_floor_unit", 2, qfalse);
		SetIconFromClassname("misc_ammo_floor_unit", 3, qfalse);
		SetIconFromClassname("misc_shield_floor_unit", 2, qfalse);
		SetIconFromClassname("misc_shield_floor_unit", 3, qfalse);
	}
	else if (!Q_stricmp(mapname.string, "mp/siege_desert"))
	{
		SetIconFromClassname("misc_ammo_floor_unit", 2, qfalse);
		SetIconFromClassname("misc_ammo_floor_unit", 3, qfalse);
		SetIconFromClassname("misc_ammo_floor_unit", 5, qfalse);
		SetIconFromClassname("misc_shield_floor_unit", 2, qfalse);
		SetIconFromClassname("misc_shield_floor_unit", 3, qfalse);
		SetIconFromClassname("misc_shield_floor_unit", 5, qfalse);
		SetIconFromClassname("misc_model_health_power_converter", 2, qfalse);
		SetIconFromClassname("misc_model_health_power_converter", 3, qfalse);
	}

	level.antiLamingTime = 0;


	// if round 1, reset both round 1 and round 2 cvars
	// if round 2, reset only round 2 cvars
	int i, j, currentRound = CurrentSiegeRound();
	for (i = 2; i >= currentRound; i--) {
		trap_Cvar_Set(va("siege_r%i_objscompleted", currentRound), "");
		trap_Cvar_Set(va("siege_r%i_heldformaxat", currentRound), "");
		trap_Cvar_Set(va("siege_r%i_heldformaxtime", currentRound), "");
		trap_Cvar_Set(va("siege_r%i_total", currentRound), "");
		for (j = 1; j <= MAX_STATS - 1; j++) {
			trap_Cvar_Set(va("siege_r%i_obj%i", i, j), "");
		}
		for (j = 0; j < MAX_CLIENTS; j++) {
			level.clients[j].sess.siegeStats.caps[i - 1] = 0;
			level.clients[j].sess.siegeStats.saves[i - 1] = 0;
			level.clients[j].sess.siegeStats.oKills[i - 1] = 0;
			level.clients[j].sess.siegeStats.dKills[i - 1] = 0;
			level.clients[j].sess.siegeStats.oDeaths[i - 1] = 0;
			level.clients[j].sess.siegeStats.dDeaths[i - 1] = 0;
			level.clients[j].sess.siegeStats.oDamageDealt[i - 1] = 0;
			level.clients[j].sess.siegeStats.oDamageTaken[i - 1] = 0;
			level.clients[j].sess.siegeStats.dDamageDealt[i - 1] = 0;
			level.clients[j].sess.siegeStats.dDamageTaken[i - 1] = 0;
			level.clients[j].sess.siegeStats.killer = -1;
			level.clients[j].sess.siegeStats.maxes[i - 1] = 0;
			level.clients[j].sess.siegeStats.maxed[i - 1] = 0;
			level.clients[j].sess.siegeStats.selfkills[i - 1] = 0;
			level.clients[j].sess.siegeStats.oTime[i - 1] = 0;
			level.clients[j].sess.siegeStats.dTime[i - 1] = 0;
			memset(&level.clients[j].sess.siegeStats.mapSpecific[i - 1], 0, sizeof(level.clients[j].sess.siegeStats.mapSpecific[i - 1]));
		}
	}

	level.siegeMatchWinner = SIEGEMATCHWINNER_NONE;
	trap_SetConfigstring(CS_SIEGE_STATE, va("0|%i", level.time)); //we're ready to g0g0g0
}

void SiegeCheckTimers(void)
{
	int i=0;
	gentity_t *ent;
	int numTeam1 = 0;
	int numTeam2 = 0;

	if (g_gametype.integer != GT_SIEGE)
	{
		return;
	}

	if (level.intermissiontime)
	{
		return;
	}

	if (gSiegeRoundEnded)
	{
		return;
	}

	if (!gSiegeRoundBegun)
	{ //check if anyone is active on this team - if not, keep the timer set up.
		i = 0;

		while (i < MAX_CLIENTS)
		{
			ent = &g_entities[i];

			if (ent && ent->inuse && ent->client &&
				ent->client->pers.connected == CON_CONNECTED &&
				ent->client->sess.siegeDesiredTeam == SIEGETEAM_TEAM1)
			{
				numTeam1++;
			}
			i++;
		}

		i = 0;

		while (i < MAX_CLIENTS)
		{
			ent = &g_entities[i];

			if (ent && ent->inuse && ent->client &&
				ent->client->pers.connected == CON_CONNECTED &&
				ent->client->sess.siegeDesiredTeam == SIEGETEAM_TEAM2)
			{
				numTeam2++;
			}
			i++;
		}

		if (g_siegeTeamSwitch.integer &&
			g_siegePersistant.beatingTime)
		{
			gImperialCountdown = level.time + g_siegePersistant.lastTime;
			gRebelCountdown = level.time + g_siegePersistant.lastTime;
		}
		else
		{
			gImperialCountdown = level.time + imperial_time_limit;
			gRebelCountdown = level.time + rebel_time_limit;
		}
	}

	if (imperial_time_limit)
	{ //team1
		if (gImperialCountdown < level.time) //they were held for 20 minutes, so let's notate this differently
		{
			int round = CurrentSiegeRound();
			trap_Cvar_Set(va("siege_r%i_heldformaxat", round), va("%i", G_FirstIncompleteObjective(round)));
			if (round == 2 && siege_r1_heldformaxat.integer) { //round 2 was held for max, and round 1 was previously held for max
				if (siege_r2_objscompleted.integer > siege_r1_objscompleted.integer)
					level.siegeMatchWinner = SIEGEMATCHWINNER_ROUND2OFFENSE;
				else if (siege_r1_objscompleted.integer > siege_r2_objscompleted.integer)
					level.siegeMatchWinner = SIEGEMATCHWINNER_ROUND1OFFENSE;
				else
					level.siegeMatchWinner = SIEGEMATCHWINNER_TIE;
			}
			PrintObjStat(0, qtrue);
			G_SiegeRoundComplete(SIEGETEAM_TEAM2, ENTITYNUM_NONE);
			imperial_time_limit = 0;
			return;
		}
	}

	if (rebel_time_limit)
	{ //team2
		if (gRebelCountdown < level.time)//they were held for 20 minutes, so let's notate this differently
		{
			int round = CurrentSiegeRound();
			trap_Cvar_Set(va("siege_r%i_heldformaxat", round), va("%i", G_FirstIncompleteObjective(round)));
			if (round == 2 && siege_r1_heldformaxat.integer) { //round 2 was held for max, and round 1 was previously held for max
				if (siege_r2_objscompleted.integer > siege_r1_objscompleted.integer)
					level.siegeMatchWinner = SIEGEMATCHWINNER_ROUND2OFFENSE;
				else if (siege_r1_objscompleted.integer > siege_r2_objscompleted.integer)
					level.siegeMatchWinner = SIEGEMATCHWINNER_ROUND1OFFENSE;
				else
					level.siegeMatchWinner = SIEGEMATCHWINNER_TIE;
			}
			PrintObjStat(0, qtrue);
			G_SiegeRoundComplete(SIEGETEAM_TEAM1, ENTITYNUM_NONE);
			rebel_time_limit = 0;
			return;
		}
	}

	if (!gSiegeRoundBegun)
	{
		static qboolean forcedInfoReload = qfalse;
		if (!numTeam1 && !numTeam2)
		{ //don't have people on both teams yet.
			memset(&level.lastLegitClass, -1, sizeof(level.lastLegitClass));
			memset(&level.tryChangeClass, -1, sizeof(level.tryChangeClass));
			gSiegeBeginTime = level.time + SIEGE_ROUND_BEGIN_TIME;
			trap_SetConfigstring(CS_SIEGE_STATE, "1"); //"waiting for players on both teams"
			level.inSiegeCountdown = qfalse;
			if (!g_siegeTeamSwitch.integer)
				level.siegeStage = SIEGESTAGE_NONE;
			else if (CurrentSiegeRound() == 1)
				level.siegeStage = SIEGESTAGE_PREROUND1;
			else
				level.siegeStage = SIEGESTAGE_PREROUND2;
		}
		else if (gSiegeBeginTime < level.time)
		{ //mark the round as having begun
			level.siegeRoundStartTime = gSiegeBeginTime;
			if (debug_duoTest.integer)
			{
				trap_Cvar_Set("g_siegeRespawn", "1");
			}
			gSiegeRoundBegun = qtrue;
			level.inSiegeCountdown = qtrue;
			level.siegeRoundComplete = qfalse;
			SiegeBeginRound(i); //perform any round start tasks
			for (i = 0; i < MAX_CLIENTS; i++) {
				if (g_entities[i].client && g_entities[i].client->pers.connected != CON_DISCONNECTED && g_entities[i].client->sess.skillBoost) {
					trap_SendServerCommand(-1, va("print \"^7%s^7 has a level ^5%d^7 skillboost.\n\"",
						g_entities[i].client->pers.netname, g_entities[i].client->sess.skillBoost));
				}
			}
			level.canShield[TEAM_RED] = CANSHIELD_YES;
			level.canShield[TEAM_BLUE] = CANSHIELD_YES;
			forcedInfoReload = qfalse;
		}
		else if (gSiegeBeginTime > (level.time + SIEGE_ROUND_BEGIN_TIME))
		{
			memset(&level.lastLegitClass, -1, sizeof(level.lastLegitClass));
			memset(&level.tryChangeClass, -1, sizeof(level.tryChangeClass));
			gSiegeBeginTime = level.time + SIEGE_ROUND_BEGIN_TIME;
			level.inSiegeCountdown = qtrue;
			level.siegeRoundComplete = qfalse;
			if (!g_siegeTeamSwitch.integer)
				level.siegeStage = SIEGESTAGE_NONE;
			else if (CurrentSiegeRound() == 1)
				level.siegeStage = SIEGESTAGE_PREROUND1;
			else
				level.siegeStage = SIEGESTAGE_PREROUND2;
		}
		else
		{
			memset(&level.lastLegitClass, -1, sizeof(level.lastLegitClass));
			trap_SetConfigstring(CS_SIEGE_STATE, va("2|%i", gSiegeBeginTime - SIEGE_ROUND_BEGIN_TIME)); //getting ready to begin
			level.inSiegeCountdown = qtrue;
			level.siegeRoundComplete = qfalse;
			if (!g_siegeTeamSwitch.integer)
				level.siegeStage = SIEGESTAGE_NONE;
			else if (CurrentSiegeRound() == 1)
				level.siegeStage = SIEGESTAGE_PREROUND1;
			else
				level.siegeStage = SIEGESTAGE_PREROUND2;
			if (!forcedInfoReload && g_delayClassUpdate.integer) { // it's now countdown, so force a resend of userinfo to hide classes for people already joined
				forcedInfoReload = qtrue;
				for (i = 0; i < MAX_CLIENTS; i++) {
					if (level.clients[i].pers.connected && !(g_entities[i].r.svFlags & SVF_BOT))
						ClientUserinfoChanged(i);
				}
			}
		}
	}
}

void SiegeObjectiveCompleted(int team, int objective, int final, int client) {

	int goals_completed, goals_required;

	trap_Cvar_Set(va("siege_r%i_obj%i", CurrentSiegeRound(), objective), va("%i", level.time - level.siegeRoundStartTime));
	PrintObjStat(objective, 0);

	if (objective == 5 && GetSiegeMap() == SIEGEMAP_HOTH)
	{
		level.hangarCompletedTime = level.time;
	}

	if (objective == 5 && GetSiegeMap() == SIEGEMAP_CARGO)
	{
		level.ccCompleted = qtrue;
	}

	if (objective == 2 && GetSiegeMap() == SIEGEMAP_DESERT && level.killerOfLastDesertComputer != NULL && level.killerOfLastDesertComputer->client && level.killerOfLastDesertComputer->client->sess.sessionTeam == TEAM_RED)
	{
		client = level.killerOfLastDesertComputer->s.number;
	}

	if (GetSiegeMap() == SIEGEMAP_URBAN)
	{
		if (objective == 2) {
			int i;
			for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++) {
				gentity_t *icon = &g_entities[i];
				if (VALIDSTRING(icon->targetname) && !Q_stricmp(icon->targetname, "hackicon") && VALIDSTRING(icon->classname) && !Q_stricmp(icon->classname, "info_siege_radaricon")) {
					icon->s.eFlags &= ~EF_RADAROBJECT;
					break;
				}
			}
		}
		if (objective < 5) {
			int i;
			for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++) {
				gentity_t *sentry = &g_entities[i];
				if (VALIDSTRING(sentry->classname) && !Q_stricmp(sentry->classname, "sentryGun") && level.time - sentry->genericValue8 < (TURRET_LIFETIME - 3000))
						sentry->genericValue8 = level.time - (TURRET_LIFETIME - 3000); // explode in 3 seconds
			}
		}
	}

	if (GetSiegeMap() == SIEGEMAP_CARGO)
	{
		if (level.totalObjectivesCompleted != 3) {
			int i;
			for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++) {
				gentity_t *sentry = &g_entities[i];
				if (VALIDSTRING(sentry->classname) && !Q_stricmp(sentry->classname, "sentryGun") && level.time - sentry->genericValue8 < (TURRET_LIFETIME - 3000))
					sentry->genericValue8 = level.time - (TURRET_LIFETIME - 3000); // explode in 3 seconds
			}
		}
	}

	if (client >= 0 && client < MAX_CLIENTS && &level.clients[client] && level.clients[client].pers.connected == CON_CONNECTED)
	{
		level.clients[client].pers.teamState.captures++;
		level.clients[client].rewardTime = level.time + REWARD_SPRITE_TIME;
		level.clients[client].ps.persistant[PERS_CAPTURES]++;
	}

	if (objective) {
		if (client >= 0 && client < MAX_CLIENTS && g_entities[client].client) {
			G_LogPrintf("Objective %i completed by client %i (%s)\n", objective, client, g_entities[client].client->pers.netname);
			g_entities[client].client->sess.siegeStats.caps[GetSiegeStatRound()]++;
		}
		else {
			G_LogPrintf("Objective %i completed by client %i\n", objective, client);
		}
		level.canShield[TEAM_RED] = CANSHIELD_YO_NOTPLACED;
		level.canShield[TEAM_BLUE] = CANSHIELD_YO_NOTPLACED;
	}

	if (gSiegeRoundEnded)
	{
		return;
	}

	//Update the configstring status
	G_SiegeSetObjectiveComplete(team, objective, qfalse);

	if (final != -1)
	{
		if (team == SIEGETEAM_TEAM1)
		{
			imperial_goals_completed++;
		}
		else
		{
			rebel_goals_completed++;
		}
	}

	if (team == SIEGETEAM_TEAM1)
	{
		goals_completed = imperial_goals_completed;
		goals_required = imperial_goals_required;
	}
	else
	{
		goals_completed = rebel_goals_completed;
		goals_required = rebel_goals_required;
	}
	if (final == 1 || goals_completed >= goals_required || (g_siegeTiebreakEnd.integer && g_siegePersistant.beatingTime && g_siegeTeamSwitch.integer && siege_r2_objscompleted.integer > siege_r1_objscompleted.integer))
	{
		SiegeBroadcast_OBJECTIVECOMPLETE(team, client, objective);
		G_SiegeRoundComplete(team, client);
	}
	else
	{
		BroadcastObjectiveCompletion(team, objective, final, client);
	}
}

void siegeTriggerUse(gentity_t *ent, gentity_t *other, gentity_t *activator)
{
	char			teamstr[64];
	char			objectivestr[64];
	char			desiredobjective[MAX_SIEGE_INFO_SIZE];
	int				clUser = ENTITYNUM_NONE;
	int				final = 0;
	int				i = 0;
	int				n;
	int				x;

	if (!siege_valid)
	{
		return;
	}

	if (level.siegeRoundComplete) // no completing objs while the round is already over
		return;

	vmCvar_t	mapname;
	trap_Cvar_Register(&mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM);

	if (!Q_stricmpn(mapname.string, "mp/siege_hoth", 13))
	{
		if (ent->objective <= 5)
		{
			for (n = 0; n < MAX_GENTITIES; n++)
			{
				if (&g_entities[n] && g_entities[n].objective && g_entities[n].objective == (ent->objective + 1))
				{
					//upon completion of a non-final objective, find the next objective and turn on its radar icon (fix for poor mapping practice on Hoth)
					g_entities[n].s.eFlags |= EF_RADAROBJECT;
				}
			}
		}
	}
	else if (!Q_stricmp(mapname.string, "siege_narshaddaa"))
	{
		if (ent->objective == 1 || ent->objective == 4 || ent->objective == 5)
		{
			for (n = 0; n < MAX_GENTITIES; n++)
			{
				if (&g_entities[n] && g_entities[n].objective && g_entities[n].objective == (ent->objective + 1))
				{
					//upon completion of a non-final objective, find the next objective and turn on its radar icon (fix for poor mapping practice on Nar)
					g_entities[n].s.eFlags |= EF_RADAROBJECT;
				}
			}
			if (ent->objective == 5)
			{
				//enable codes icon
				SetIconFromClassname("misc_siege_item", 1, qtrue);
			}
		}
		else if (ent->objective == 2)
		{
			for (n = 0; n < MAX_GENTITIES; n++)
			{
				if (&g_entities[n] && g_entities[n].objective && (g_entities[n].objective == 3 || g_entities[n].objective == 4))
				{
					//activate both station icons for nar, since these two objs can be completed in any order
					g_entities[n].s.eFlags |= EF_RADAROBJECT;
				}
			}
		}
	}
	else if (!Q_stricmp(mapname.string, "mp/siege_desert"))
	{
		if (ent->objective == 1)
		{
		}
		else if (ent->objective == 2 && !level.wallCompleted) //objective 2 is targeted to enable its icon
		{
			ent->s.eFlags |= EF_RADAROBJECT;
			return;
		}
		else if (ent->objective == 2 && level.wallCompleted) //objective 2 is targeted because it has actually been completed
		{
		}
		else if (!(ent->s.eFlags & EF_RADAROBJECT))
		{	//toggle radar on and exit if it is not showing up already
			//we still need to support the default behavior for this map (targeting inactive siege objs simply causes the icon to activate rather than completing the obj)
			ent->s.eFlags |= EF_RADAROBJECT;
			return;
		}
	}
	else if (GetSiegeMap() == SIEGEMAP_CARGO)
	{
		if (ent->objective == 6)
		{
			for (x = 0; x < MAX_GENTITIES; x++)
			{
				if (&g_entities[x] && g_entities[x].classname && g_entities[x].classname[0] && !Q_stricmp(g_entities[x].classname, "info_siege_radaricon") &&
					g_entities[x].targetname && g_entities[x].targetname[0] && Q_stricmp(g_entities[x].targetname, "wedidthenewhack") && Q_stricmp(g_entities[x].targetname, "ventcentergens"))
				{
					//remove all the now-useless hack icons once we reach the last obj
					g_entities[x].s.eFlags &= ~EF_RADAROBJECT;
					G_FreeEntity(&g_entities[x]);
				}
			}
		}
		if (!(ent->s.eFlags & EF_RADAROBJECT))
		{ //toggle radar on and exit if it is not showing up already
			ent->s.eFlags |= EF_RADAROBJECT;
			return;
		}
	}
	else
	{
		if (!(ent->s.eFlags & EF_RADAROBJECT))
		{ //toggle radar on and exit if it is not showing up already
			ent->s.eFlags |= EF_RADAROBJECT;
			return;
		}
	}

	level.totalObjectivesCompleted++;
	level.previousObjectiveCompleted = level.objectiveJustCompleted;
	level.objectiveJustCompleted = ent->objective;
	char *roundCvar = va("siege_r%i_objscompleted", CurrentSiegeRound());
	trap_Cvar_Set(roundCvar, va("%i", trap_Cvar_VariableIntegerValue(roundCvar) + 1));

	if (!Q_stricmpn(mapname.string, "mp/siege_hoth", 13))
	{
		if (ent->objective == 1)
		{
			SetIconFromClassname("misc_ammo_floor_unit", 5, qtrue);
			SetIconFromClassname("misc_shield_floor_unit", 2, qtrue);
		}
		else if (ent->objective == 3)
		{
			SetIconFromClassname("misc_ammo_floor_unit", 5, qfalse);
			SetIconFromClassname("misc_shield_floor_unit", 2, qfalse);
			SetIconFromClassname("misc_ammo_floor_unit", 4, qtrue);
			SetIconFromClassname("misc_shield_floor_unit", 1, qtrue);
		}
		else if (ent->objective == 4)
		{
			SetIconFromClassname("misc_ammo_floor_unit", 4, qfalse);
			SetIconFromClassname("misc_shield_floor_unit", 1, qfalse);
		}
		else if (ent->objective == 5)
		{
			SetIconFromClassname("misc_ammo_floor_unit", 1, qtrue);
			SetIconFromClassname("misc_ammo_floor_unit", 2, qtrue);
			SetIconFromClassname("misc_ammo_floor_unit", 3, qtrue);
		}
	}
	else if (!Q_stricmp(mapname.string, "siege_narshaddaa"))
	{
		if (ent->objective == 1)
		{
			SetIconFromClassname("misc_ammo_floor_unit", 3, qtrue);
			SetIconFromClassname("misc_shield_floor_unit", 2, qtrue);
		}
		else if (ent->objective == 2)
		{
			SetIconFromClassname("misc_ammo_floor_unit", 1, qfalse);
			SetIconFromClassname("misc_shield_floor_unit", 1, qfalse);
		}
		else if ((ent->objective == 3 || ent->objective == 4) && level.totalObjectivesCompleted == 4)
		{
			SetIconFromClassname("misc_ammo_floor_unit", 1, qfalse);
			SetIconFromClassname("misc_ammo_floor_unit", 3, qfalse);
			SetIconFromClassname("misc_shield_floor_unit", 2, qfalse);
		}
		else if (ent->objective == 5)
		{
			SetIconFromClassname("misc_siege_icon", 1, qtrue);
			SetIconFromClassname("misc_ammo_floor_unit", 2, qtrue);
			SetIconFromClassname("misc_shield_floor_unit", 3, qtrue);
		}
	}
	else if (!Q_stricmp(mapname.string, "mp/siege_desert"))
	{
		if (ent->objective == 1)
		{
			SetIconFromClassname("misc_ammo_floor_unit", 1, qfalse);
			SetIconFromClassname("misc_ammo_floor_unit", 4, qfalse);
			SetIconFromClassname("misc_shield_floor_unit", 1, qfalse);
			SetIconFromClassname("misc_shield_floor_unit", 4, qfalse);
			SetIconFromClassname("misc_model_health_power_converter", 1, qfalse);
			SetIconFromClassname("misc_ammo_floor_unit", 2, qtrue);
			SetIconFromClassname("misc_ammo_floor_unit", 3, qtrue);
			SetIconFromClassname("misc_shield_floor_unit", 2, qtrue);
			SetIconFromClassname("misc_shield_floor_unit", 3, qtrue);
			SetIconFromClassname("misc_model_health_power_converter", 2, qtrue);
		}
		else if (ent->objective == 2)
		{
			SetIconFromClassname("misc_ammo_floor_unit", 2, qfalse);
			SetIconFromClassname("misc_ammo_floor_unit", 3, qfalse);
			SetIconFromClassname("misc_shield_floor_unit", 2, qfalse);
			SetIconFromClassname("misc_shield_floor_unit", 3, qfalse);
			SetIconFromClassname("misc_model_health_power_converter", 2, qfalse);
			for (x = 0; x < MAX_GENTITIES; x++) //get rid of the wall icons if they are still up
			{
				if (&g_entities[x] && !Q_stricmp(g_entities[x].classname, "info_siege_radaricon") && !Q_stricmpn(g_entities[x].targetname, "walldestroy", 11))
				{
					g_entities[x].s.eFlags &= ~EF_RADAROBJECT;
				}
			}
		}
		else if (ent->objective == 3)
		{
			SetIconFromClassname("misc_ammo_floor_unit", 5, qtrue);
			SetIconFromClassname("misc_shield_floor_unit", 5, qtrue);
			SetIconFromClassname("misc_model_health_power_converter", 3, qtrue);
		}
	}



	if (activator) { //activator will hopefully be the person who triggered this event
		if (activator->s.NPC_class == CLASS_VEHICLE) { // if a vehicle captured the objective, the pilot should be credited for the objective
			if (activator->m_pVehicle && activator->m_pVehicle->m_pPilot) {
				clUser = activator->m_pVehicle->m_pPilot->s.number; // we have a pilot
			}
			else if (activator->lastPilot && activator->lastPilot - g_entities >= 0 && activator->lastPilot - g_entities < MAX_CLIENTS &&
				activator->lastPilot->client && activator->lastPilot->client->pers.connected == CON_CONNECTED) {
				clUser = activator->lastPilot->s.number; // the vehicle has no pilot, but it previously had a pilot (maybe the vehicle's own momentum carried it in)
			}
		}
		else if (activator->client) {
			clUser = activator->s.number;
		}
	}

	if (ent->side == SIEGETEAM_TEAM1)
	{
		Com_sprintf(teamstr, sizeof(teamstr), team1);
	}
	else
	{
		Com_sprintf(teamstr, sizeof(teamstr), team2);
	}

	if (BG_SiegeGetValueGroup(siege_info, teamstr, gParseObjectives))
	{
		Com_sprintf(objectivestr, sizeof(objectivestr), "Objective%i", ent->objective);

		if (BG_SiegeGetValueGroup(gParseObjectives, objectivestr, desiredobjective))
		{
			if (BG_SiegeGetPairedValue(desiredobjective, "final", teamstr))
			{
				final = atoi(teamstr);
			}

			if (BG_SiegeGetPairedValue(desiredobjective, "target", teamstr))
			{
				while (teamstr[i])
				{
					if (teamstr[i] == '\r' ||
						teamstr[i] == '\n')
					{
						teamstr[i] = '\0';
					}

					i++;
				}
				UseSiegeTarget(other, activator, teamstr);
			}

			if (ent->target && ent->target[0])
			{ //use this too
				UseSiegeTarget(other, activator, ent->target);
			}

			SiegeObjectiveCompleted(ent->side, ent->objective, final, clUser);
		}
	}

	if (ent->objective == 1 && !Q_stricmp(mapname.string, "mp/siege_desert"))
	{
		//second obj icon should always be disabled
		SetIconFromClassname("info_siege_objective", 2, qfalse);
		level.wallCompleted = qtrue;
	}
}

/*QUAKED info_siege_objective (1 0 1) (-16 -16 -24) (16 16 32) ? x x STARTOFFRADAR
STARTOFFRADAR - start not displaying on radar, don't display until used.

"objective" - specifies the objective to complete upon activation
"side" - set to 1 to specify an imperial goal, 2 to specify rebels
"icon" - icon that represents the objective on the radar
*/
void SP_info_siege_objective(gentity_t *ent)
{
	char* s;
	vmCvar_t	mapname;
	trap_Cvar_Register(&mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM);

	if (!siege_valid || g_gametype.integer != GT_SIEGE)
	{
		G_FreeEntity(ent);
		return;
	}

	ent->use = siegeTriggerUse;
	G_SpawnInt( "objective", "0", &ent->objective);
	G_SpawnInt( "side", "0", &ent->side);

	if (!ent->objective || !ent->side)
	{ //j00 fux0red something up
		G_FreeEntity(ent);
		G_Printf("ERROR: info_siege_objective without an objective or side value\n");
		return;
	}

	if (!level.numSiegeObjectivesOnMap || ent->objective > level.numSiegeObjectivesOnMap)
		level.numSiegeObjectivesOnMap = ent->objective;

	if (!Q_stricmpn(mapname.string, "mp/siege_hoth", 13))
	{
		if (ent->objective == 1)
		{
			//objective 1 on hoth
			ent->s.eFlags |= EF_RADAROBJECT;
		}
		else
		{
			//objectives 2-6 on hoth
			ent->s.eFlags &= ~EF_RADAROBJECT;
		}
	}
	else if (!Q_stricmp(mapname.string, "siege_narshaddaa"))
	{
		if (ent->objective == 1)
		{
			//objective 1 on nar
			ent->s.eFlags |= EF_RADAROBJECT;
		}
		else
		{
			//objectives 2-6 on nar
			ent->s.eFlags &= ~EF_RADAROBJECT;
		}
	}
	else if (!Q_stricmp(mapname.string, "mp/siege_desert"))
	{
		//they all start off radar
		//(obj 1 uses separate icons for each (left/right) wall segment
		ent->s.eFlags &= ~EF_RADAROBJECT;
	}
	else
	{
		//all other maps
		//Set it up to be drawn on radar
		if (!(ent->spawnflags & SIEGEITEM_STARTOFFRADAR))
		{
			ent->s.eFlags |= EF_RADAROBJECT;
		}
	}

	//All clients want to know where it is at all times for radar
	ent->r.svFlags |= SVF_BROADCAST;

	G_SpawnString( "icon", "", &s );
	
	if (s && s[0])
	{ 
		// We have an icon, so index it now.  We are reusing the genericenemyindex
		// variable rather than adding a new one to the entity state.
		ent->s.genericenemyindex = G_IconIndex(s);
	}

	ent->s.brokenLimbs = ent->side;
	ent->s.frame = ent->objective;
	trap_LinkEntity(ent);
}


void SiegeIconUse(gentity_t *ent, gentity_t *other, gentity_t *activator)
{ //toggle it on and off
	if (ent->s.eFlags & EF_RADAROBJECT)
	{
		ent->s.eFlags &= ~EF_RADAROBJECT;
		ent->r.svFlags &= ~SVF_BROADCAST;
	}
	else
	{
		ent->s.eFlags |= EF_RADAROBJECT;
		ent->r.svFlags |= SVF_BROADCAST;
	}
}

/*QUAKED info_siege_radaricon (1 0 1) (-16 -16 -24) (16 16 32) ? 
Used to arbitrarily display radar icons at placed location. Can be used
to toggle on and off.

"icon" - icon that represents the objective on the radar
"startoff" - if 1 start off
*/
void SP_info_siege_radaricon (gentity_t *ent)
{
	char* s;
	int i;

	if (!siege_valid || g_gametype.integer != GT_SIEGE)
	{
		G_FreeEntity(ent);
		return;
	}

	G_SpawnInt("startoff", "0", &i);

	if (!i)
	{ //start on then
		ent->s.eFlags |= EF_RADAROBJECT;
		ent->r.svFlags |= SVF_BROADCAST;
	}

	G_SpawnString( "icon", "", &s );
	if (!s || !s[0])
	{ //that's the whole point of the entity
        Com_Error(ERR_DROP, "misc_siege_radaricon without an icon");
		return;
	}

	ent->classname = "info_siege_radaricon";

	ent->use = SiegeIconUse;

	ent->s.genericenemyindex = G_IconIndex(s);

	trap_LinkEntity(ent);
}

void decompTriggerUse(gentity_t *ent, gentity_t *other, gentity_t *activator)
{
	int final = 0;
	char teamstr[1024];
	char objectivestr[64];
	char desiredobjective[MAX_SIEGE_INFO_SIZE];

	if (gSiegeRoundEnded)
	{
		return;
	}

	if (!G_SiegeGetCompletionStatus(ent->side, ent->objective))
	{ //if it's not complete then there's nothing to do here
		return;
	}

	//Update the configstring status
	G_SiegeSetObjectiveComplete(ent->side, ent->objective, qtrue);

	//Find out if this objective counts toward the final objective count
   	if (ent->side == SIEGETEAM_TEAM1)
	{
		Com_sprintf(teamstr, sizeof(teamstr), team1);
	}
	else
	{
		Com_sprintf(teamstr, sizeof(teamstr), team2);
	}

	if (BG_SiegeGetValueGroup(siege_info, teamstr, gParseObjectives))
	{
		Com_sprintf(objectivestr, sizeof(objectivestr), "Objective%i", ent->objective);

		if (BG_SiegeGetValueGroup(gParseObjectives, objectivestr, desiredobjective))
		{
			if (BG_SiegeGetPairedValue(desiredobjective, "final", teamstr))
			{
				final = atoi(teamstr);
			}
		}
	}

	//Subtract the goal num if applicable
	if (final != -1)
	{
		if (ent->side == SIEGETEAM_TEAM1)
		{
			imperial_goals_completed--;
		}
		else
		{
			rebel_goals_completed--;
		}
	}
}

/*QUAKED info_siege_decomplete (1 0 1) (-16 -16 -24) (16 16 32)
"objective" - specifies the objective to decomplete upon activation
"side" - set to 1 to specify an imperial (team1) goal, 2 to specify rebels (team2)
*/
void SP_info_siege_decomplete (gentity_t *ent)
{
	if (!siege_valid || g_gametype.integer != GT_SIEGE)
	{
		G_FreeEntity(ent);
		return;
	}

	ent->use = decompTriggerUse;
	G_SpawnInt( "objective", "0", &ent->objective);
	G_SpawnInt( "side", "0", &ent->side);

	if (!ent->objective || !ent->side)
	{ //j00 fux0red something up
		G_FreeEntity(ent);
		G_Printf("ERROR: info_siege_objective_decomplete without an objective or side value\n");
		return;
	}
}

void siegeEndUse(gentity_t *ent, gentity_t *other, gentity_t *activator)
{
	LogExit("Round ended");
}

/*QUAKED target_siege_end (1 0 1) (-16 -16 -24) (16 16 32)
Do a logexit for siege when used.
*/
void SP_target_siege_end (gentity_t *ent)
{
	if (!siege_valid || g_gametype.integer != GT_SIEGE)
	{
		G_FreeEntity(ent);
		return;
	}

	ent->use = siegeEndUse;
}

void SiegeItemRemoveOwner(gentity_t *ent, gentity_t *carrier)
{
	ent->genericValue2 = 0; //Remove picked-up flag
	ent->genericValue8 = ENTITYNUM_NONE; //Mark entity carrying us as none

	if (carrier)
	{
		carrier->client->holdingObjectiveItem = 0; //The carrier is no longer carrying us
		if (ent->genericValue15)
		{
			carrier->client->ps.fd.forcePowerRegenDebounceTime = 0; //start regenerating force immediately
		}
		carrier->r.svFlags &= ~SVF_BROADCAST;
#ifdef NEWMOD_SUPPORT
		UpdateNewmodSiegeItems();
#endif
	}
}

static void SiegeItemRespawnEffect(gentity_t *ent, vec3_t newOrg)
{
	ent->siegeItemSpawnTime = level.time;
	vec3_t upAng;

	if (ent->target5 && ent->target5[0])
	{
		G_UseTargets2(ent, ent, ent->target5);
	}

	if (!ent->genericValue10)
	{ //no respawn effect
		return;
	}

	VectorSet(upAng, 0, 0, 1);

	//Play it once on the current origin, and once on the origin we're respawning to.
	G_PlayEffectID(ent->genericValue10, ent->r.currentOrigin, upAng);
	G_PlayEffectID(ent->genericValue10, newOrg, upAng);
}

void SiegeItemRespawnOnOriginalSpot(gentity_t *ent, gentity_t *carrier)
{
	SiegeItemRespawnEffect(ent, ent->pos1);
	G_SetOrigin(ent, ent->pos1);
	SiegeItemRemoveOwner(ent, carrier);
	
	// Stop the item from flashing on the radar
	ent->s.time2 = 0;
}

void SiegeItemThink(gentity_t *ent)
{
	gentity_t *carrier = NULL;

	if (ent->genericValue12)
	{ //recharge health
		if (ent->health > 0 && ent->health < ent->maxHealth && ent->genericValue14 < level.time)
		{
            ent->genericValue14 = level.time + ent->genericValue13;
			ent->health += ent->genericValue12;
			if (ent->health > ent->maxHealth)
			{
				ent->health = ent->maxHealth;
			}
		}
	}

	if (ent->genericValue8 != ENTITYNUM_NONE)
	{ //Just keep sticking it on top of the owner. We need it in the same PVS as him so it will render bolted onto him properly.
		carrier = &g_entities[ent->genericValue8];

		if (carrier->inuse && carrier->client)
		{
			if (ent->specialIconTreatment)
			{
				ent->s.eFlags &= ~EF_RADAROBJECT;
			}
			VectorCopy(carrier->client->ps.origin, ent->r.currentOrigin);
			trap_LinkEntity(ent);
		}
	}
	else if (ent->genericValue1)
	{ //this means we want to run physics on the object
		G_RunExPhys(ent, ent->radius, ent->mass, ent->random, qfalse, NULL, 0);
	}

	//Bolt us to whoever is carrying us if a client
	if (ent->genericValue8 < MAX_CLIENTS)
	{
		ent->s.boltToPlayer = ent->genericValue8+1;
	}
	else
	{
		ent->s.boltToPlayer = 0;
	}


	if (carrier)
	{
		gentity_t *carrier = &g_entities[ent->genericValue8];

		//This checking can be a bit iffy on the death stuff, but in theory we should always
		//get a think in before the default minimum respawn time is exceeded.
		if (!carrier->inuse || !carrier->client ||
			(carrier->client->sess.sessionTeam != SIEGETEAM_TEAM1 && carrier->client->sess.sessionTeam != SIEGETEAM_TEAM2) ||
			(carrier->client->ps.pm_flags & PMF_FOLLOW))
		{ //respawn on the original spot
			ent->siegeItemCarrierTime = 0;
			if (ent->specialIconTreatment)
			{
				ent->s.eFlags |= EF_RADAROBJECT;
			}
			SiegeItemRespawnOnOriginalSpot(ent, NULL);
		}
		else if (carrier->health < 1)
		{ //The carrier died so pop out where he is (unless in nodrop).
			ent->siegeItemCarrierTime = 0;
			if (ent->target6 && ent->target6[0])
			{
				vmCvar_t mapname;
				trap_Cvar_Register(&mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM);
				if (!Q_stricmp(mapname.string, "mp/siege_desert") && !Q_stricmp(ent->target6, "c3podroppedprint"))
				{
					//droid part on desert
					char *part;
					if (ent->model && ent->model[0])
					{
						if (strstr(ent->model, "arm"))
						{
							part = "arm";
						}
						else if (strstr(ent->model, "head"))
						{
							part = "head";
						}
						else if (strstr(ent->model, "leg"))
						{
							part = "leg";
						}
						else if (strstr(ent->model, "torso"))
						{
							part = "torso";
						}
					}
					if (part && part[0])
					{
						trap_SendServerCommand(-1, va("cp \"Protocol droid %s has been dropped!\n\"", part));
					}
					else
					{
						G_UseTargets2(ent, ent, ent->target6);
					}
				}
				else
				{
					G_UseTargets2(ent, ent, ent->target6);
				}
			}
			if (ent->specialIconTreatment)
			{
				ent->s.eFlags |= EF_RADAROBJECT;
			}
			if ( trap_PointContents(carrier->client->ps.origin, carrier->s.number) & CONTENTS_NODROP )
			{ //In nodrop land, go back to the original spot.
				SiegeItemRespawnOnOriginalSpot(ent, carrier);
				if (!ent->genericValue16) //hacky...
				{
					ent->s.eFlags |= EF_NODRAW;
					ent->genericValue11 = 0;
					ent->s.eFlags &= ~EF_RADAROBJECT;
				}
			}
			else
			{
				G_SetOrigin(ent, carrier->client->ps.origin);
				ent->epVelocity[0] = Q_irand(-80, 80);
				ent->epVelocity[1] = Q_irand(-80, 80);
				ent->epVelocity[2] = Q_irand(40, 80);

				//We're in a nonstandard place, so if we go this long without being touched,
				//assume we may not be reachable and respawn on the original spot.
				ent->genericValue9 = level.time + ent->genericValue17;

				SiegeItemRemoveOwner(ent, carrier);
			}
		}
		// update siege item carry time
		else if (ent->siegeItemCarrierTime && carrier->client && carrier - g_entities >= 0 && carrier - g_entities < MAX_CLIENTS) {
			if (GetSiegeMap() == SIEGEMAP_HOTH)
				carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_HOTH_CODESTIME] += (level.time - ent->siegeItemCarrierTime);
			else if (GetSiegeMap() == SIEGEMAP_DESERT)
				carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_DESERT_PARTSTIME] += (level.time - ent->siegeItemCarrierTime);
			else if (GetSiegeMap() == SIEGEMAP_KORRIBAN && VALIDSTRING(ent->goaltarget)) {
				if (!Q_stricmp(ent->goaltarget, "bluecrystaldelivery"))
					carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_KORRI_BLUETIME] += (level.time - ent->siegeItemCarrierTime);
				else if (!Q_stricmp(ent->goaltarget, "greencrystaldelivery"))
					carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_KORRI_GREENTIME] += (level.time - ent->siegeItemCarrierTime);
				else if (!Q_stricmp(ent->goaltarget, "redcrystaldelivery"))
					carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_KORRI_REDTIME] += (level.time - ent->siegeItemCarrierTime);
				else if (!Q_stricmp(ent->goaltarget, "staffplace"))
					carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_KORRI_SCEPTERTIME] += (level.time - ent->siegeItemCarrierTime);
			}
			else if (GetSiegeMap() == SIEGEMAP_NAR)
				carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_NAR_CODESTIME] += (level.time - ent->siegeItemCarrierTime);
			else if (GetSiegeMap() == SIEGEMAP_CARGO)
				carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_CARGO2_CODESTIME] += (level.time - ent->siegeItemCarrierTime);
			else if (GetSiegeMap() == SIEGEMAP_BESPIN)
				carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_BESPIN_CODESTIME] += (level.time - ent->siegeItemCarrierTime);
			else if (GetSiegeMap() == SIEGEMAP_URBAN)
				carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_URBAN_MONEYTIME] += (level.time - ent->siegeItemCarrierTime);
			else if (GetSiegeMap() == SIEGEMAP_ANSION)
				carrier->client->sess.siegeStats.mapSpecific[GetSiegeStatRound()][SIEGEMAPSTAT_ANSION_CODESTIME] += (level.time - ent->siegeItemCarrierTime);
			ent->siegeItemCarrierTime = level.time;
		}
	}
	else
		ent->siegeItemCarrierTime = 0;

	if (ent->genericValue9 && ent->genericValue9 < level.time && ent->genericValue17 != -1)
	{ //time to respawn on the original spot then
		ent->siegeItemCarrierTime = 0;
		if (ent->specialIconTreatment)
		{
			ent->s.eFlags |= EF_RADAROBJECT;
		}
		SiegeItemRespawnEffect(ent, ent->pos1);
		G_SetOrigin(ent, ent->pos1);
		if (!ent->genericValue16) //hacky...
		{
			ent->s.eFlags |= EF_NODRAW;
			ent->genericValue11 = 0;
			ent->s.eFlags &= ~EF_RADAROBJECT;
		}
		ent->genericValue9 = 0;
		
		// stop flashing on radar
		ent->s.time2 = 0;
	}

	ent->nextthink = level.time + FRAMETIME/2;
}

void SiegeItemTouch( gentity_t *self, gentity_t *other, trace_t *trace )
{
	short i1;
	if (!other || !other->inuse ||
		!other->client || other->s.eType == ET_NPC)
	{
		if (trace && trace->startsolid && (g_floatingItems.integer || self->siegeItemSpawnTime && level.time - self->siegeItemSpawnTime <= 1000)) // duo: allow items to float for 1sec after spawning, even with g_floatingItems set to 0
		{ //let me out! (ideally this should not happen, but such is life)
			vec3_t escapePos;
			VectorCopy(self->r.currentOrigin, escapePos);
			escapePos[2] += 1.0f;

			//I hope you weren't stuck in the ceiling.
			G_SetOrigin(self, escapePos);
		}
		return;
	}

	if (other->health < 1)
	{ //dead people can't pick us up.
		return;
	}

	if (other->client->holdingObjectiveItem)
	{ //this guy's already carrying a siege item
		return;
	}

	if ( other->client->ps.pm_type == PM_SPECTATOR )
	{//spectators don't pick stuff up
		return;
	}

	if (self->genericValue2)
	{ //Am I already picked up?
		return;
	}

	if (self->genericValue6 == other->client->sess.sessionTeam)
	{ //Set to not be touchable by players on this team.
		return;
	}

	if (!gSiegeRoundBegun)
	{ //can't pick it up if round hasn't started yet
		return;
	}

	if (other->client->sess.siegeDuelInProgress)
	{
		//people in a duel can't pick us up
		return;
	}

	if (self->s.eFlags & EF_NODRAW)
	{
		return;
	}

	if (level.zombies && GetSiegeMap() == SIEGEMAP_CARGO)
	{
		return;
	}

	if (self->idealClassType && self->idealClassType >= CLASSTYPE_ASSAULT && other->client->sess.sessionTeam == TEAM_RED)
	{
		i1 = bgSiegeClasses[other->client->siegeClass].playerClass;
		if (i1 + 10 != self->idealClassType)
		{
			return;
		}
	}

	if (self->idealClassTypeTeam2 && self->idealClassTypeTeam2 >= CLASSTYPE_ASSAULT && other->client->sess.sessionTeam == TEAM_BLUE)
	{
		i1 = bgSiegeClasses[other->client->siegeClass].playerClass;
		if (i1 + 10 != self->idealClassTypeTeam2)
		{
			return;
		}
	}

	if (self->noise_index)
	{ //play the pickup noise.
		G_Sound(other, CHAN_AUTO, self->noise_index);
	}

	if (GetSiegeMap() != SIEGEMAP_UNKNOWN)
		self->siegeItemCarrierTime = level.time;
	else
		self->siegeItemCarrierTime = 0;

	self->genericValue2 = 1; //Mark it as picked up.

	other->client->holdingObjectiveItem = self->s.number;
	other->r.svFlags |= SVF_BROADCAST; //broadcast player while he carries this
	self->genericValue8 = other->s.number; //Keep the index so we know who is "carrying" us

	self->genericValue9 = 0; //So it doesn't think it has to respawn.

#ifdef NEWMOD_SUPPORT
	UpdateNewmodSiegeItems();
#endif

	if (self->target2 && self->target2[0] && (!self->genericValue4 || !self->genericValue5))
	{ //fire the target for pickup, if it's set to fire every time, or set to only fire the first time and the first time has not yet occured.
		if (GetSiegeMap() == SIEGEMAP_DESERT && !Q_stricmp(self->target2, "c3postolenprint"))
		{
			//droid part on desert
			char *part;
			if (self->model && self->model[0])
			{
				if (strstr(self->model, "arm"))
				{
					part = "arm";
				}
				else if (strstr(self->model, "head"))
				{
					part = "head";
				}
				else if (strstr(self->model, "leg"))
				{
					part = "leg";
				}
				else if (strstr(self->model, "torso"))
				{
					part = "torso";
				}
			}
			if (part && part[0])
			{
				trap_SendServerCommand(-1, va("cp \"Protocol droid %s has been taken!\n\"", part));
			}
			else
			{
				G_UseTargets2(self, self, self->target2);
			}
		}
		else
		{
			G_UseTargets2(self, self, self->target2);
		}
		self->genericValue5 = 1; //mark it as having been picked up
	}	
	if (self->specialIconTreatment)
	{
		self->s.eFlags &= ~EF_RADAROBJECT;
	}
	// time2 set to -1 will blink the item on the radar indefinately
	self->s.time2 = 0xFFFFFFFF;

}

void SiegeItemPain(gentity_t *self, gentity_t *attacker, int damage)
{
	// Time 2 is used to pulse the radar icon to show its under attack
	self->s.time2 = level.time;
}

void SiegeItemDie( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	vmCvar_t	mapname;
	trap_Cvar_Register(&mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM);

	if (!Q_stricmp(mapname.string, "mp/siege_desert") && self->model && self->model[0] && strstr(self->model, "vjun/control_station"))
	{
		//killed a desert computer; note the client so we can give him points
		if (attacker && attacker->client && attacker->s.number < MAX_CLIENTS && attacker->client->sess.sessionTeam == TEAM_RED)
		{
			level.killerOfLastDesertComputer = attacker;
		}
		else
		{
			level.killerOfLastDesertComputer = NULL;
		}
	}

	self->takedamage = qfalse; //don't die more than once

	if (self->genericValue3)
	{ //An indexed effect to play on death
		vec3_t upAng;

		VectorSet(upAng, 0, 0, 1);
		G_PlayEffectID(self->genericValue3, self->r.currentOrigin, upAng);
	}

	self->neverFree = qfalse;
	self->think = G_FreeEntity;
	self->nextthink = level.time;

	//Fire off the death target if we've got one.
	if (self->target4 && self->target4[0])
	{
		G_UseTargets2(self, self, self->target4);
	}
}

void SiegeItemUse(gentity_t *ent, gentity_t *other, gentity_t *activator)
{ //once used, become active
	if (ent->spawnflags & SIEGEITEM_STARTOFFRADAR)
	{ //start showing on radar
		ent->s.eFlags |= EF_RADAROBJECT;

		if (!(ent->s.eFlags & EF_NODRAW))
		{ //we've nothing else to do here
			return;
		}
	}
	else
	{ //make sure it's showing up
		ent->s.eFlags |= EF_RADAROBJECT;
	}

	if (ent->genericValue11 || !ent->takedamage)
	{ //We want to be able to walk into it to pick it up then.
		ent->r.contents = CONTENTS_TRIGGER;
		ent->clipmask = CONTENTS_SOLID|CONTENTS_TERRAIN;
		if (ent->genericValue11)
		{
			ent->touch = SiegeItemTouch;
		}
	}
	else
	{ //Make it solid.
		ent->r.contents = MASK_PLAYERSOLID;
		ent->clipmask = MASK_PLAYERSOLID;
	}

	ent->think = SiegeItemThink;
	ent->nextthink = level.time + FRAMETIME/2;

	//take off nodraw
	ent->s.eFlags &= ~EF_NODRAW;

	if (ent->paintarget && ent->paintarget[0])
	{ //want to be on this guy's origin now then
		gentity_t *targ = G_Find (NULL, FOFS(targetname), ent->paintarget);
		
		if (targ && targ->inuse)
		{
			G_SetOrigin(ent, targ->r.currentOrigin);
			trap_LinkEntity(ent);
		}
	}
}

/*QUAKED misc_siege_item (1 0 1) (-16 -16 -24) (16 16 32) ? x x STARTOFFRADAR
STARTOFFRADAR - start not displaying on radar, don't display until used.

"model"				Name of model to use for the object
"mins"				Actual mins of the object. Careful not to place it into a solid,
					as these new mins will not be reflected visually in the editor.
					Default value is "-16 -16 -24".
"maxs"				Same as above for maxs. Default value is "16 16 32".
"targetname"		If it has a targetname, it will only spawn upon being used.
"target2"			Target to fire upon pickup. If none, nothing will happen.
"pickuponlyonce"	If non-0, target2 will only be fired on the first pickup. If the item is
					dropped and picked up again later, the target will not be fired off on
					the sequential pickup. Default value is 1.
"target3"			Target to fire upon delivery of the item to the goal point.
					If none, nothing will happen. (but you should always want something to happen)
"health"			If > 0, object can be damaged and will die once health reaches 0. Default is 0.
"showhealth"		if health > 0, will show a health meter for this item
"teamowner"			Which team owns this item, used only for deciding what color to make health meter
"target4"			Target to fire upon death, if damageable. Default is none.
"deathfx"			Effect to play on death, if damageable. Default is none.
"canpickup"			If non-0, item can be picked up. Otherwise it will just be solid and sit on the
					ground. Default is 1.
"pickupsound"		Sound to play on pickup, if any.
"goaltarget"		Must be the targetname of a trigger_multi/trigger_once. Once a player carrying
					this object is brought inside the specified trigger, then that trigger will be
					allowed to fire. Ideally it will target a siege objective or something like that.
"usephysics"		If non-0, run standard physics on the object. Default is 1.
"mass"				If usephysics, this will be the factored object mass. Default is 0.09.
"gravity"			If usephysics, this will be the factored gravitational pull. Default is 3.0.
"bounce"			If usephysics, this will be the factored bounce amount. Default is 1.3.
"teamnotouch"		If 1 don't let team 1 pickup, if 2 don't let team 2. By default both teams
					can pick this object up and carry it around until death.
"teamnocomplete"	Same values as above, but controls if this object can be taken into the objective
					area by said team.
"respawnfx"			Plays this effect when respawning (e.g. it is left in an unknown area too long
					and goes back to the original spot). If this is not specified there will be
					no effect. (expected format is .efx file)
"paintarget"		plop self on top of this guy's origin when we are used (only applies if the siege
					item has a targetname)
"noradar"			if non-0 this thing will not show up on radar

"forcelimit"		if non-0, while carrying this item, the carrier's force powers will be crippled.
"target5"			target to fire when respawning.
"target6"			target to fire when dropped by someone carrying this item.

"icon"				icon that represents the gametype item on the radar

health charge things only work with showhealth 1 on siege items that take damage.
"health_chargeamt"	if non-0 will recharge this much health every...
"health_chargerate"	...this many milliseconds
*/
void SP_misc_siege_item (gentity_t *ent)
{
	int		canpickup;
	int		noradar;
	char	*s;

	if (!siege_valid || g_gametype.integer != GT_SIEGE)
	{
		G_FreeEntity(ent);
		return;
	}

	if (!ent->model || !ent->model[0])
	{
		G_Error("You must specify a model for misc_siege_item types.");
	}

	G_SpawnInt("canpickup", "1", &canpickup);
	ent->canPickUp = canpickup;
	G_SpawnInt("usephysics", "1", &ent->genericValue1);
	G_SpawnInt("autorespawn", "1", &ent->genericValue16);
	G_SpawnInt("respawntime", "20000", &ent->genericValue17);
	ent->siegeItemCarrierTime = 0;

	ent->classname = "misc_siege_item";

	if (ent->genericValue1)
	{ //if we're using physics we want lerporigin smoothing
		ent->s.eFlags |= EF_CLIENTSMOOTH;
	}

	G_SpawnInt("noradar", "0", &noradar);

	vmCvar_t	mapname;
	trap_Cvar_Register(&mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM);

	if (!Q_stricmp(mapname.string, "siege_narshaddaa"))
	{
		//start off with icon hidden for nar shaddaa codes
		ent->s.eFlags &= ~EF_RADAROBJECT;
	}
	else
	{
		//Want it to always show up as a goal object on radar
		if (!noradar && !(ent->spawnflags & SIEGEITEM_STARTOFFRADAR))
		{
			ent->s.eFlags |= EF_RADAROBJECT;
		}
	}

	//All clients want to know where it is at all times for radar
	ent->r.svFlags |= SVF_BROADCAST;

	G_SpawnInt("pickuponlyonce", "1", &ent->genericValue4);

	G_SpawnInt("teamnotouch", "0", &ent->genericValue6);
	G_SpawnInt("teamnocomplete", "0", &ent->genericValue7);
	G_SpawnInt("hideiconwhilecarried", "0", &ent->specialIconTreatment);
	G_SpawnInt("removeFromOwnerOnUse", "1", &ent->removeFromOwnerOnUse);
	G_SpawnInt("removeFromGameOnUse", "1", &ent->removeFromGameOnUse);
	G_SpawnInt("despawnOnUse", "0", &ent->despawnOnUse);
	//Get default physics values.
	G_SpawnFloat("mass", "0.09", &ent->mass);
	G_SpawnFloat("gravity", "3.0", &ent->radius);
	G_SpawnFloat("bounce", "1.3", &ent->random);

	if (G_SpawnString("idealclasstype", "", &s))
	{
		if (s && s[0])
		{
			if (!Q_stricmp(s, "a"))
			{
				ent->idealClassType = CLASSTYPE_ASSAULT;
			}
			else if (!Q_stricmp(s, "h"))
			{
				ent->idealClassType = CLASSTYPE_HW;
			}
			else if (!Q_stricmp(s, "d"))
			{
				ent->idealClassType = CLASSTYPE_DEMO;
			}
			else if (!Q_stricmp(s, "s"))
			{
				ent->idealClassType = CLASSTYPE_SCOUT;
			}
			else if (!Q_stricmp(s, "t"))
			{
				ent->idealClassType = CLASSTYPE_TECH;
			}
			else if (!Q_stricmp(s, "j"))
			{
				ent->idealClassType = CLASSTYPE_JEDI;
			}
		}
	}

	if (G_SpawnString("idealclasstypeteam2", "", &s))
	{
		if (s && s[0])
		{
			if (!Q_stricmp(s, "a"))
			{
				ent->idealClassTypeTeam2 = CLASSTYPE_ASSAULT;
			}
			else if (!Q_stricmp(s, "h"))
			{
				ent->idealClassTypeTeam2 = CLASSTYPE_HW;
			}
			else if (!Q_stricmp(s, "d"))
			{
				ent->idealClassTypeTeam2 = CLASSTYPE_DEMO;
			}
			else if (!Q_stricmp(s, "s"))
			{
				ent->idealClassTypeTeam2 = CLASSTYPE_SCOUT;
			}
			else if (!Q_stricmp(s, "t"))
			{
				ent->idealClassTypeTeam2 = CLASSTYPE_TECH;
			}
			else if (!Q_stricmp(s, "j"))
			{
				ent->idealClassTypeTeam2 = CLASSTYPE_JEDI;
			}
		}
	}

	G_SpawnString( "pickupsound", "", &s );

	if (s && s[0])
	{ //We have a pickup sound, so index it now.
		ent->noise_index = G_SoundIndex(s);
	}

	G_SpawnString( "deathfx", "", &s );

	if (s && s[0])
	{ //We have a death effect, so index it now.
		ent->genericValue3 = G_EffectIndex(s);
	}

	G_SpawnString( "respawnfx", "", &s );

	if (s && s[0])
	{ //We have a respawn effect, so index it now.
		ent->genericValue10 = G_EffectIndex(s);
	}

	G_SpawnString( "icon", "", &s );
	
	if (s && s[0])
	{ 
		// We have an icon, so index it now.  We are reusing the genericenemyindex
		// variable rather than adding a new one to the entity state.
		ent->s.genericenemyindex = G_IconIndex(s);
	}
	
	ent->s.modelindex = G_ModelIndex(ent->model);
	G_SpawnInt("modelscale", "100", &ent->s.iModelScale);
	ent->s.iModelScale = Com_Clampi(0, 1023, ent->s.iModelScale);

	//Is the model a ghoul2 model?
	if (!Q_stricmp(&ent->model[strlen(ent->model) - 4], ".glm"))
	{ //apparently so.
        ent->s.modelGhoul2 = 1;
	}

	ent->s.eType = ET_GENERAL;

	//Set the mins/maxs with default values.
	G_SpawnVector("mins", "-16 -16 -24", ent->r.mins);
	G_SpawnVector("maxs", "16 16 32", ent->r.maxs);

	VectorCopy(ent->s.origin, ent->pos1); //store off the initial origin for respawning
	G_SetOrigin(ent, ent->s.origin);

	VectorCopy(ent->s.angles, ent->r.currentAngles);
	VectorCopy(ent->s.angles, ent->s.apos.trBase);

	G_SpawnInt("forcelimit", "0", &ent->genericValue15);

	G_SpawnFloat("speedMultiplier", "1", &ent->speedMultiplier);
	G_SpawnFloat("speedMultiplierTeam2", "1", &ent->speedMultiplierTeam2);

	if (ent->health > 0)
	{ //If it has health, it can be killed.
		int t;

		ent->pain = SiegeItemPain;
		ent->die = SiegeItemDie;
		ent->takedamage = qtrue;

		G_SpawnInt( "showhealth", "0", &t );
		if (t)
		{ //a non-0 maxhealth value will mean we want to show the health on the hud
			ent->maxHealth = ent->health;
			G_ScaleNetHealth(ent);

			G_SpawnInt( "health_chargeamt", "0", &ent->genericValue12);
			G_SpawnInt( "health_chargerate", "0", &ent->genericValue13);
		}
	}
	else
	{ //Otherwise no.
		ent->takedamage = qfalse;
	}

	if (ent->spawnflags & SIEGEITEM_STARTOFFRADAR)
	{
		ent->use = SiegeItemUse;
	}
	else if (ent->targetname && ent->targetname[0])
	{
		ent->s.eFlags |= EF_NODRAW; //kind of hacky, but whatever
		ent->genericValue11 = canpickup;
        ent->use = SiegeItemUse;
		ent->s.eFlags &= ~EF_RADAROBJECT;
	}

	if ( (!ent->targetname || !ent->targetname[0]) ||
		 (ent->spawnflags & SIEGEITEM_STARTOFFRADAR) )
	{
		if (canpickup || !ent->takedamage)
		{ //We want to be able to walk into it to pick it up then.
			ent->r.contents = CONTENTS_TRIGGER;
			ent->clipmask = CONTENTS_SOLID|CONTENTS_TERRAIN;
			if (canpickup)
			{
				ent->touch = SiegeItemTouch;
			}
		}
		else
		{ //Make it solid.
			ent->r.contents = MASK_PLAYERSOLID;
			ent->clipmask = MASK_PLAYERSOLID;
		}

		ent->think = SiegeItemThink;
		ent->nextthink = level.time + FRAMETIME/2;
	}

	ent->genericValue8 = ENTITYNUM_NONE; //initialize the carrier to none

	ent->neverFree = qtrue; //never free us unless we specifically request it.

	trap_LinkEntity(ent);
}

//sends extra data about other client's in this client's PVS
//used for support guy etc.
//with this formatting:
//sxd 16,999,999,999|17,999,999,999
//assumed max 2 chars for cl num, 3 chars per ammo/health/maxhealth, even a single string full of
//info for all 32 clients should not get much past 450 bytes, which is well within a
//reasonable range. We don't need to send anything about the max ammo or current weapon, because
//currentState.weapon can be checked for the ent in question on the client. -rww
void G_SiegeClientExData(gentity_t *msgTarg)
{
	gentity_t *ent;
	int count = 0;
	int i = 0;
	char str[MAX_STRING_CHARS];
	char scratch[MAX_STRING_CHARS];

    if ( level.pause.state != PAUSE_NONE )
            return;

	while (i < level.num_entities && count < MAX_EXDATA_ENTS_TO_SEND)
	{
		ent = &g_entities[i];

		if (ent->inuse && ent->client && msgTarg->s.number != ent->s.number &&
			ent->s.eType == ET_PLAYER && msgTarg->client->ps.persistant[PERS_TEAM] == ent->client->ps.persistant[PERS_TEAM] &&
			trap_InPVS(msgTarg->client->ps.origin, ent->client->ps.origin))
		{ //another client in the same pvs, send his jive
            if (count)
			{ //append a seperating space if we are not the first in the list
				Q_strcat(str, sizeof(str), " ");
			}
			else
			{ //otherwise create the prepended chunk
				strcpy(str, "sxd ");
			}

			//append the stats
			Com_sprintf(scratch, sizeof(scratch), "%i|%i|%i|%i", ent->s.number, ent->client->ps.stats[STAT_HEALTH],
				ent->client->ps.stats[STAT_MAX_HEALTH], ent->client->ps.ammo[weaponData[ent->client->ps.weapon].ammoIndex]);
			Q_strcat(str, sizeof(str), scratch);
			count++;
		}
		i++;
	}

	if (!count)
	{ //nothing to send
		return;
	}

	//send the string to him
	trap_SendServerCommand(msgTarg-g_entities, str);
}
