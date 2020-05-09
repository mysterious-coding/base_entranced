#include "g_database.h"
#include "sqlite3.h"

const char* const oldLogDbFilename = "jka_log.db";
const char* const oldCfgDbFilename = "jka_config.db";

// =========== METADATA ========================================================

const char* const sqlCopyOldTablesToNewV1DB =
"ATTACH DATABASE 'jka_log.db' AS logDb;                                      \n"
"ATTACH DATABASE 'jka_config.db' as cfgDb;                                   \n"
//"BEGIN TRANSACTION;                                                          \n"
"                                                                            \n"
"INSERT INTO main.siegefastcaps                                              \n"
"    (fastcap_id, mapname, flags, rank, desc, player1_name, player1_ip_int,  \n"
"    player1_cuid_hash2, max_speed, avg_speed, player2_name, player2_ip_int, \n"
"    player2_cuid_hash2, obj1_time, obj2_time, obj3_time, obj4_time,         \n"
"    obj5_time, obj6_time, obj7_time, obj8_time, obj9_time, total_time,      \n"
"    date, match_id, player1_client_id, player2_client_id, player3_name,     \n"
"    player3_ip_int, player3_cuid_hash2, player4_name, player4_ip_int,       \n"
"    player4_cuid_hash2)                                                     \n"
"SELECT                                                                      \n"
"    fastcap_id, mapname, flags, rank, desc, player1_name, player1_ip_int,   \n"
"    player1_cuid_hash2, max_speed, avg_speed, player2_name, player2_ip_int, \n"
"    player2_cuid_hash2, obj1_time, obj2_time, obj3_time, obj4_time,         \n"
"    obj5_time, obj6_time, obj7_time, obj8_time, obj9_time, total_time,      \n"
"    date, match_id, player1_client_id, player2_client_id, player3_name,     \n"
"    player3_ip_int, player3_cuid_hash2, player4_name, player4_ip_int,       \n"
"    player4_cuid_hash2                                                      \n"
"FROM logDb.siegefastcaps;                                                   \n"
"                                                                            \n"
"INSERT INTO main.playerwhitelist                                            \n"
"    (unique_id, cuid_hash2, name)                                           \n"
"SELECT                                                                      \n"
"    unique_id, cuid_hash2, name                                             \n"
"FROM logDb.playerwhitelist;                                                 \n"
"                                                                            \n"
"INSERT INTO main.nicknames                                                  \n"
"    (ip_int, name, duration, cuid_hash2)                                    \n"
"SELECT                                                                      \n"
"    ip_int, name, duration, cuid_hash2                                      \n"
"FROM logDb.nicknames;                                                       \n"
"                                                                            \n"
"INSERT INTO main.ip_whitelist                                               \n"
"    (ip_int, mask_int, notes)                                               \n"
"SELECT                                                                      \n"
"    ip_int, mask_int, notes                                                 \n"
"FROM cfgDb.ip_whitelist;                                                    \n"
"                                                                            \n"
"INSERT INTO main.ip_blacklist                                               \n"
"    (ip_int, mask_int, notes, reason, banned_since, banned_until)           \n"
"SELECT                                                                      \n"
"    ip_int, mask_int, notes, reason, banned_since, banned_until             \n"
"FROM cfgDb.ip_blacklist;                                                    \n"
"                                                                            \n"
"INSERT INTO main.pool_has_map                                               \n"
"    (pool_id, mapname, weight)                                              \n"
"SELECT                                                                      \n"
"    pool_id, mapname, weight                                                \n"
"FROM cfgDb.pool_has_map;                                                    \n"
"                                                                            \n"
"INSERT INTO main.pools                                                      \n"
"    (pool_id, short_name, long_name)                                        \n"
"SELECT                                                                      \n"
"    pool_id, short_name, long_name                                          \n"
"FROM cfgDb.pools;                                                           \n"
"                                                                            \n"
//"COMMIT TRANSACTION;                                                         \n"
"DETACH DATABASE logDb;                                                      \n"
"DETACH DATABASE cfgDb;                                                        ";

static qboolean UpgradeFromOldVersions( sqlite3* dbPtr ) {
	// special type of upgrade: upgrading from the two old file databases to version 1 of single database model

	int rc;

	// try to open both databases temporarily to make sure they exist, because ATTACH creates files that don't exist

	sqlite3* logDb;
	sqlite3* cfgDb;

	rc = sqlite3_open_v2( oldLogDbFilename, &logDb, SQLITE_OPEN_READONLY, NULL );

	if ( rc != SQLITE_OK ) {
		Com_Printf( "Failed to open logs database file %s for upgrading (code: %d)\n", oldLogDbFilename, rc );
		return qfalse;
	}


	rc = sqlite3_open_v2( oldCfgDbFilename, &cfgDb, SQLITE_OPEN_READONLY, NULL );

	if ( rc != SQLITE_OK ) {
		Com_Printf( "Failed to open config database file %s for upgrading (code: %d)\n", oldCfgDbFilename, rc );
		return qfalse;
	}

	sqlite3_close( logDb );
	sqlite3_close( cfgDb );

	// upgrade
	return sqlite3_exec( dbPtr, sqlCopyOldTablesToNewV1DB, NULL, NULL, NULL ) == SQLITE_OK;
}

// =============================================================================

static qboolean UpgradeDB( int versionTo, sqlite3* dbPtr ) {
	Com_Printf( "Upgrading database to version %d...\n", versionTo );

	switch ( versionTo ) {
		default:
			Com_Printf( "ERROR: Unsupported database upgrade routine\n" );
	}

	return qfalse;
}

qboolean G_DBUpgradeDatabaseSchema( int versionFrom, void* db ) {
	if (versionFrom == DB_SCHEMA_VERSION) {
		// already up-to-date
		return qtrue;
	} else if (versionFrom > DB_SCHEMA_VERSION) {
		// don't let older servers load more recent databases
		Com_Printf("ERROR: Database version is higher than the one used by this mod version!\n");
		return qfalse;
	}

	sqlite3* dbPtr = (sqlite3*)db;

	if (versionFrom > 0) {
		// database already exists, we are just upgrading between versions

		while (versionFrom < DB_SCHEMA_VERSION) {
			if (!UpgradeDB(++versionFrom, dbPtr)) {
				return qfalse;
			}
		}

		Com_Printf("Database upgrade successful\n");
	} else {
		// special case: database was just created, try to upgrade from the even older versions
		// but don't fail if it's not possible
		Com_Printf("Attempting to convert jka_log.db and jka_config.db...\n");
		UpgradeFromOldVersions(dbPtr);
	}
	
	return qtrue;
}