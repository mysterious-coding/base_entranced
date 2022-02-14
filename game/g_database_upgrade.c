#include "g_database.h"
#include "sqlite3.h"

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
	}
	
	return qtrue;
}