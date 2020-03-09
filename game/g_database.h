#ifndef G_DATABASE_H
#define G_DATABASE_H

#include "g_local.h"

#define DB_FILENAME				"entranced.db"
#define DB_SCHEMA_VERSION		1
#define DB_SCHEMA_VERSION_STR	"1"
#define DB_OPTIMIZE_INTERVAL	( 60*60*3 ) // every 3 hours
#define DB_VACUUM_INTERVAL		( 60*60*24*7 ) // every week

qboolean G_DBOptimizeDatabaseIfNeeded(void);
void G_DBLoadDatabase( void );

qboolean G_DBUpgradeDatabaseSchema( int versionFrom, void* db );

// =========== ACCOUNTS ========================================================

typedef void( *DBListAccountSessionsCallback )( void *ctx,
	session_t* session,
	qboolean temporary );

qboolean G_DBGetAccountByID( const int id,
	account_t* account );

qboolean G_DBGetAccountByName( const char* name,
	account_t* account );

void G_DBCreateAccount( const char* name );

qboolean G_DBDeleteAccount( account_t* account );

qboolean G_DBGetSessionByID( const int id,
	session_t* session );

qboolean G_DBGetSessionByIdentifier( const sessionIdentifier_t identifier,
	session_t* session );

void G_DBCreateSession( const sessionIdentifier_t identifier,
	const char* info );

void G_DBLinkAccountToSession( session_t* session,
	account_t* account );

void G_DBUnlinkAccountFromSession( session_t* session );

void G_DBListSessionsForAccount( account_t* account,
	DBListAccountSessionsCallback callback,
	void* ctx );

// =========== METADATA ========================================================

void G_DBGetMetadata( const char *key,
	char *outValue,
	size_t outValueBufSize );

void G_DBSetMetadata( const char *key,
	const char *value );

// =========== NICKNAMES =======================================================

typedef void( *ListAliasesCallback )( void* context,
	const char* name,
	int duration );

void G_DBLogNickname( unsigned int ipInt,
	const char* name,
	int duration,
	const char* cuidHash );

void G_DBListAliases( unsigned int ipInt,
	unsigned int ipMask,
	int limit,
	ListAliasesCallback callback,
	void* context,
	const char* cuidHash );

// =========== FASTCAPS ========================================================

void G_DBLoadCaptureRecords(const char *mapname, CaptureCategoryFlags flags, qboolean strict, CaptureRecordsForCategory *out);

typedef void(*ListAllMapsCapturesCallback)(void *context, const char *mapname, const CaptureCategoryFlags flags, const CaptureCategoryFlags thisRecordFlags,
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

void G_DBListAllMapsCaptureRecords(CaptureCategoryFlags flags, int limit, int offset, ListAllMapsCapturesCallback callback, void *context);
void G_DBListLatestCaptureRecords(CaptureCategoryFlags flags, int limit, int offset, ListLastestCapturesCallback callback, void *context);

void G_DBSaveCaptureRecords(CaptureRecordsContext *context);

// =========== LOCKDOWN WHITELIST ==============================================

#define LOCKDOWNWHITELISTED_ID		(1 << 0)
#define LOCKDOWNWHITELISTED_CUID	(1 << 1)
int G_DBPlayerLockdownWhitelisted(unsigned long long uniqueID, const char* cuidHash);
void G_DBStorePlayerInLockdownWhitelist(unsigned long long uniqueID, const char* cuidHash, const char* name);
void G_DBRemovePlayerFromLockdownWhitelist(unsigned long long uniqueID, const char* cuidHash);


// =========== WHITELIST =======================================================

qboolean G_DBIsFilteredByWhitelist( unsigned int ip,
	char* reasonBuffer,
	int reasonBufferSize );

qboolean G_DBAddToWhitelist( unsigned int ip,
	unsigned int mask,
	const char* notes );

qboolean G_DBRemoveFromWhitelist( unsigned int ip,
	unsigned int mask );

// =========== BLACKLIST =======================================================

typedef void( *BlackListCallback )( unsigned int ip,
	unsigned int mask,
	const char* notes,
	const char* reason,
	const char* banned_since,
	const char* banned_until );

qboolean G_DBIsFilteredByBlacklist( unsigned int ip,
	char* reasonBuffer,
	int reasonBufferSize );

void G_DBListBlacklist( BlackListCallback callback );

qboolean G_DBAddToBlacklist( unsigned int ip,
	unsigned int mask,
	const char* notes,
	const char* reason,
	int hours );

qboolean G_DBRemoveFromBlacklist( unsigned int ip,
	unsigned int mask );

// =========== MAP POOLS =======================================================

typedef void( *ListPoolCallback )( void* context,
	int pool_id,
	const char* short_name,
	const char* long_name );

typedef void( *ListMapsPoolCallback )( void** context,
	const char* long_name,
	int pool_id,
	const char* mapname,
	int mapWeight );

typedef void( *MapSelectedCallback )( void *context,
	char* mapname );

typedef struct {
	int pool_id;
	char long_name[64];
} PoolInfo;

void G_DBListPools( ListPoolCallback callback,
	void* context );

void G_DBListMapsInPool( const char* short_name,
	const char* ignore,
	ListMapsPoolCallback callback,
	void** context );

qboolean G_DBFindPool( const char* short_name,
	PoolInfo* poolInfo );

qboolean G_DBSelectMapsFromPool( const char* short_name,
	const char* ignoreMap,
	const int mapsToRandomize,
	MapSelectedCallback callback,
	void* context );

qboolean G_DBPoolCreate( const char* short_name,
	const char* long_name );

qboolean G_DBPoolDeleteAllMaps( const char* short_name );

qboolean G_DBPoolDelete( const char* short_name );

qboolean G_DBPoolMapAdd( const char* short_name,
	const char* mapname,
	int weight );

qboolean G_DBPoolMapRemove( const char* short_name,
	const char* mapname );

#endif //G_DATABASE_H
