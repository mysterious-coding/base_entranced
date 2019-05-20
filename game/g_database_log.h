#ifndef G_DATABASE_H
#define G_DATABASE_H

#include "g_local.h"

// main loading routines
void G_LogDbLoad();
void G_LogDbUnload();

// level stuff   
int G_LogDbLogLevelStart( qboolean isRestart );
void G_LogDbLogLevelEnd( int levelId );

void G_LogDbLogLevelEvent( int levelId,
    int levelTime,
    int eventId,
    int context1,
    int context2,
    int context3,
    int context4,
    const char* contextText );

typedef enum
{
    levelEventNone,
    levelEventTeamChanged,
} LevelEvent;

// session stuff
int G_LogDbLogSessionStart( unsigned int ipInt,
    int ipPort,
    int id);

void G_LogDbLogSessionEnd( int sessionId );

void G_LogDbLogNickname(unsigned int ipInt,
	const char* name,
	int duration,
	const char* cuidHash);

typedef void( *ListAliasesCallback )(void* context,
    const char* name,
    int duration);

void G_CfgDbListAliases(unsigned int ipInt,
	unsigned int ipMask,
	int limit,
	ListAliasesCallback callback,
	void* context,
	const char* cuidHash);

#define WHITELISTED_ID		(1 << 0)
#define WHITELISTED_CUID	(1 << 1)
int G_DbPlayerWhitelisted(unsigned long long uniqueID, const char* cuidHash);
void G_DbStorePlayerInWhitelist(unsigned long long uniqueID, const char* cuidHash, const char* name);
void G_DbRemovePlayerFromWhitelist(unsigned long long uniqueID, const char* cuidHash);

void G_LogDbLoadCaptureRecords(const char *mapname, CaptureCategoryFlags flags, qboolean strict, CaptureRecordsForCategory *out);

typedef void( *ListAllMapsCapturesCallback )(void *context, const char *mapname, const CaptureCategoryFlags flags, const CaptureCategoryFlags thisRecordFlags,
	const char *recordHolderName1, unsigned int recordHolderIpInt1, const char *recordHolderCuid1,
	const char *recordHolderName2, unsigned int recordHolderIpInt2, const char *recordHolderCuid2,
	const char *recordHolderName3, unsigned int recordHolderIpInt3, const char *recordHolderCuid3,
	const char *recordHolderName4, unsigned int recordHolderIpInt4, const char *recordHolderCuid4,
	int bestTime, time_t bestTimeDate);

typedef void(*ListLastestCapturesCallback) (void *context, const char *mapname, const CaptureCategoryFlags flags, const CaptureCategoryFlags thisRecordFlags,
	const char *recordHolderName1, unsigned int recordHolderIpInt1, const char *recordHolderCuid1,
	const char *recordHolderName2, unsigned int recordHolderIpInt2, const char *recordHolderCuid2,
	const char *recordHolderName3, unsigned int recordHolderIpInt3, const char *recordHolderCuid3,
	const char *recordHolderName4, unsigned int recordHolderIpInt4, const char *recordHolderCuid4,
	int bestTime, time_t bestTimeDate, int rank);

void G_LogDbListAllMapsCaptureRecords(CaptureCategoryFlags flags, int limit, int offset, ListAllMapsCapturesCallback callback, void *context );
void G_LogDbListLatestCaptureRecords(CaptureCategoryFlags flags, int limit, int offset, ListLastestCapturesCallback callback, void *context);

void G_LogDbSaveCaptureRecords( CaptureRecordsContext *context );

#endif //G_DATABASE_H


