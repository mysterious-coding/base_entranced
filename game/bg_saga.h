#define		MAX_SIEGE_INFO_SIZE					16384

#define		SIEGETEAM_TEAM1						1 //e.g. TEAM_RED
#define		SIEGETEAM_TEAM2						2 //e.g. TEAM_BLUE

#define		SIEGE_POINTS_OBJECTIVECOMPLETED		20 // guy who captures any objective gets this
#define		SIEGE_POINTS_FINALOBJECTIVECOMPLETED	30 // guy who captures the last objective gets this
#define		SIEGE_POINTS_TEAMWONROUND			10 // everyone on winning team gets this

#define     SIEGE_POINTS_OBJECTIVECOMPLETED_NEW     100
#define     SIEGE_POINTS_FINALOBJECTIVECOMPLETED_NEW 100
#define     SIEGE_POINTS_TEAMWONROUND_NEW     0

#define		SIEGE_ROUND_BEGIN_TIME				5000 //delay 5 secs after players are in game.

#define		MAX_EXDATA_ENTS_TO_SEND				MAX_CLIENTS //max number of extended data for ents to send

typedef struct
{
	char		name[512];
	siegeClass_t	*classes[MAX_SIEGE_CLASSES_PER_TEAM];
	int			numClasses;
	int			friendlyShader;
} siegeTeam_t;

#include "namespace_begin.h"

extern siegeClass_t bgSiegeClasses[MAX_SIEGE_CLASSES];
extern int bgNumSiegeClasses;

extern siegeTeam_t bgSiegeTeams[MAX_SIEGE_TEAMS];
extern int bgNumSiegeTeams;

int BG_SiegeGetValueGroup(char *buf, char *group, char *outbuf);
int BG_SiegeGetPairedValue(char *buf, char *key, char *outbuf);
void BG_SiegeStripTabs(char *buf);

void BG_SiegeLoadClasses(siegeClassDesc_t *descBuffer);
void BG_SiegeLoadTeams(void);

siegeTeam_t *BG_SiegeFindThemeForTeam(int team);
void BG_PrecacheSabersForSiegeTeam(int team);
siegeClass_t *BG_SiegeFindClassByName(const char *classname);
siegeClass_t *BG_SiegeGetClass(int team, stupidSiegeClassNum_t classNumber);
qboolean BG_SiegeCheckClassLegality(int team, char *classname);
void BG_SiegeSetTeamTheme(int team, char *themeName, char *backup);
int BG_SiegeFindClassIndexByName(const char *classname);

extern char	siege_info[MAX_SIEGE_INFO_SIZE];
extern int	siege_valid;

#include "namespace_end.h"
