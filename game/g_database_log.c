#include "g_database_log.h"
#include "sqlite3.h"
#include "time.h"

static sqlite3* fileDb = 0;
static sqlite3* db = 0;

const char* const logDbFileName = "jka_log.db";

const char* const sqlCreateLogDb =
"CREATE TABLE sessions (                                                        "
"    [session_id] INTEGER PRIMARY KEY AUTOINCREMENT,                            "
"    [session_start] DATETIME,                                                  "
"    [session_end] DATETIME,                                                    "
"    [ip_int] INTEGER,                                                          "
"    [ip_port] INTEGER,                                                         "
"    [client_id] INTEGER);                                                      "
"                                                                               "
"                                                                               "
"CREATE TABLE[hack_attempts](                                                   "
"    [session_id] INTEGER REFERENCES[sessions]( [session_id] ),                 "
"    [ip_text] TEXT,                                                            "
"    [description] TEXT );                                                      "
"                                                                               "
"                                                                               "
"CREATE TABLE [level_events] (                                                  "
"    [level_event_id] INTEGER PRIMARY KEY AUTOINCREMENT,                        "
"    [level_id] INTEGER REFERENCES [levels]([level_id]),                        "
"    [event_level_time] INTEGER,                                                "
"    [event_id] INTEGER,                                                        "
"    [event_context_i1] INTEGER,                                                "
"    [event_context_i2] INTEGER,                                                "
"    [event_context_i3] INTEGER,                                                "
"    [event_context_i4] INTEGER,                                                "
"    [event_context] TEXT);                                                     "
"                                                                               "
"                                                                               "
"CREATE TABLE [levels] (                                                        "
"  [level_id] INTEGER PRIMARY KEY AUTOINCREMENT,                                "
"  [level_start] DATETIME,                                                      "
"  [level_end] DATETIME,                                                        "
"  [mapname] TEXT,                                                              "
"  [restart] BOOL);                                                             "
"                                                                               "
"                                                                               "
"CREATE TABLE nicknames(                                                        "
"  [ip_int] INTEGER,                                                            "
"  [name] TEXT,                                                                 "
"  [duration] INTEGER,                                                          "
"  [cuid_hash2] TEXT );                                                         "
"                                                                               "
"CREATE TABLE playerwhitelist(                                                  "
"  [unique_id] BIGINT,                                                          "
"  [cuid_hash2] TEXT,                                                           "
"  [name] TEXT);                                                                ";
                                                                               

const char* const sqlLogLevelStart =
"INSERT INTO levels (level_start, mapname, restart) "
"VALUES (datetime('now'),?, ?)                      ";
         
const char* const sqlLogLevelEnd =
"UPDATE levels                      "
"SET level_end = datetime( 'now' )  "
"WHERE level_id = ? ;               ";       

const char* const sqllogSessionStart =
"INSERT INTO sessions (session_start, ip_int, ip_port, client_id)    "
"VALUES (datetime('now'),?,?, ?)                                     ";

const char* const sqllogSessionEnd =
"UPDATE sessions                      "
"SET session_end = datetime( 'now' )  "
"WHERE session_id = ? ;               ";

const char* const sqlAddLevelEvent =
"INSERT INTO level_events (level_id, event_level_time, event_id,           "
"event_context_i1, event_context_i2, event_context_i3, event_context_i4,   "
"event_context)                                                            "
"VALUES (?,?,?,?,?,?,?,?)                                                  ";

const char* const sqlAddNameNM =
"INSERT INTO nicknames (ip_int, name, duration, cuid_hash2)     "
"VALUES (?,?,?,?)                                               ";

const char* const sqlAddName =
"INSERT INTO nicknames (ip_int, name, duration)                 "
"VALUES (?,?,?)                                                 ";

const char* const sqlGetAliases =
"SELECT name, SUM( duration ) AS time                           "
"FROM nicknames                                                 "
"WHERE nicknames.ip_int & ?2 = ?1 & ?2                          "
"GROUP BY name                                                  "
"ORDER BY time DESC                                             "
"LIMIT ?3                                                       ";

const char* const sqlCountAliases =
"SELECT COUNT(*) FROM (                                         "
"SELECT name, SUM( duration ) AS time                           "
"FROM nicknames                                                 "
"WHERE nicknames.ip_int & ?2 = ?1 & ?2                          "
"GROUP BY name                                                  "
"ORDER BY time DESC                                             "
"LIMIT ?3                                                       "
")                                                              ";

const char* const sqlGetNMAliases =
"SELECT name, SUM( duration ) AS time                           "
"FROM nicknames                                                 "
"WHERE nicknames.cuid_hash2 = ?1                                "
"GROUP BY name                                                  "
"ORDER BY time DESC                                             "
"LIMIT ?2                                                       ";

const char* const sqlCountNMAliases =
"SELECT COUNT(*) FROM ("
"SELECT name, SUM( duration ) AS time                           "
"FROM nicknames                                                 "
"WHERE nicknames.cuid_hash2 = ?1                                "
"GROUP BY name                                                  "
"ORDER BY time DESC                                             "
"LIMIT ?2                                                       "
")                                                              ";

const char* const sqlTestCuidSupport =
"PRAGMA table_info(nicknames)                                   ";

const char* const sqlUpgradeToCuid2FromNoCuid =
"ALTER TABLE nicknames ADD cuid_hash2 TEXT                      ";

const char* const sqlUpgradeToCuid2FromCuid1 =
"BEGIN TRANSACTION;                                                                                      "
"CREATE TABLE nicknames_temp([ip_int] INTEGER, [name] TEXT, [duration] INTEGER);                         "
"INSERT INTO nicknames_temp (ip_int, name, duration) SELECT ip_int, name, duration FROM nicknames;       "
"DROP TABLE nicknames;                                                                                   "
"CREATE TABLE nicknames([ip_int] INTEGER, [name] TEXT, [duration] INTEGER, [cuid_hash2] TEXT);           "
"INSERT INTO nicknames (ip_int, name, duration) SELECT ip_int, name, duration FROM nicknames_temp;       "
"DROP TABLE nicknames_temp;                                                                              "
"COMMIT;                                                                                                 ";

const char* const sqlTestWhitelistSupport =
"PRAGMA table_info(playerwhitelist)";

const char* const sqlUpgradeToWhitelist =
"CREATE TABLE playerwhitelist(                                                  "
"  [unique_id] BIGINT,                                                          "
"  [cuid_hash2] TEXT,                                                           "
"  [name] TEXT);                                                                ";

const char* const sqlCheckNMPlayerWhitelisted =
"SELECT COUNT(*) FROM playerwhitelist WHERE cuid_hash2 = ?1";

const char* const sqlCheckPlayerWhitelisted =
"SELECT COUNT(*) FROM playerwhitelist WHERE unique_id = ?1";

const char* const sqlAddUniqueIDAndCuidToWhitelist =
"INSERT INTO playerwhitelist (unique_id, cuid_hash2, name)                     "
"VALUES (?1,?2,?3)                                                             ";

const char* const sqlAddCuidToWhitelist =
"INSERT INTO playerwhitelist (cuid_hash2, name)                                 "
"VALUES (?1,?2)                                                                 ";

const char* const sqlAddUniqueIDToWhitelist =
"INSERT INTO playerwhitelist (unique_id, name)                                    "
"VALUES (?1,?2)                                                                   ";

const char* const sqlDeleteNMPlayerFromWhitelist =
"DELETE FROM playerwhitelist WHERE unique_id = ?1 OR cuid_hash2 = ?2";

const char* const sqlDeletePlayerFromWhitelist =
"DELETE FROM playerwhitelist WHERE unique_id = ?1";

const char* const sqlTestSiegeFastcapsSupport =
"PRAGMA table_info(siegefastcaps)";

const char* const sqlCreateSiegeFastcapsTable =
"CREATE TABLE siegefastcaps (                                                   "
"    [fastcap_id] INTEGER PRIMARY KEY AUTOINCREMENT,                            "
"    [mapname] TEXT,                                                            "
"    [flags] INTEGER,                                                           "
"    [desc] TEXT,                                                               "
"    [player1_name] TEXT,                                                       "
"    [player1_ip_int] INTEGER,                                                  "
"    [player1_cuid_hash2] TEXT,                                                 "
"    [max_speed] INTEGER,                                                       "
"    [avg_speed] INTEGER,                                                       "
"    [player2_name] TEXT,                                                       "
"    [player2_ip_int] INTEGER,                                                  "
"    [player2_cuid_hash2] TEXT,                                                 "
"    [obj1_time] INTEGER,                                                       "
"    [obj2_time] INTEGER,                                                       "
"    [obj3_time] INTEGER,                                                       "
"    [obj4_time] INTEGER,                                                       "
"    [obj5_time] INTEGER,                                                       "
"    [obj6_time] INTEGER,                                                       "
"    [obj7_time] INTEGER,                                                       "
"    [obj8_time] INTEGER,                                                       "
"    [obj9_time] INTEGER,                                                       "
"    [total_time] INTEGER,                                                      "
"    [date] INTEGER,                                                            "
"    [match_id] TEXT,                                                           "
"    [player1_client_id] INTEGER,                                               "
"    [player2_client_id] INTEGER);                                              ";

const char* const sqlAddSiegeFastcapV2 =
"INSERT INTO siegefastcaps (                                                    "
"    mapname, flags, desc, player1_name, player1_ip_int,                        "
"    player1_cuid_hash2, max_speed, avg_speed,                                  "
"    player2_name, player2_ip_int, player2_cuid_hash2,                          "
"    obj1_time, obj2_time, obj3_time, obj4_time, obj5_time,                     "
"    obj6_time, obj7_time, obj8_time, obj9_time, total_time, date,              "
"    match_id, player1_client_id, player2_client_id)                            "
"VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)                     ";

const char* const sqlremoveSiegeFastcaps =
"DELETE FROM siegefastcaps WHERE mapname = ?1 AND flags = ?2                    ";

const char* const sqlGetSiegeFastcapsStrict =
"SELECT flags, player1_name, player1_ip_int,                                    "
"    player1_cuid_hash2, max_speed, avg_speed,                                  "
"    player2_name, player2_ip_int, player2_cuid_hash2,                          "
"    obj1_time, obj2_time, obj3_time, obj4_time, obj5_time,                     "
"    obj6_time, obj7_time, obj8_time, obj9_time, total_time, date,              "
"    match_id, player1_client_id, player2_client_id                             "
"FROM siegefastcaps                                                             "
"WHERE siegefastcaps.mapname = ?1 AND siegefastcaps.flags = ?2                  "
"ORDER BY total_time                                                            "
"LIMIT ?3                                                                       ";

const char* const sqlGetSiegeFastcapsRelaxed =
"SELECT flags, player1_name, player1_ip_int,                                    "
"    player1_cuid_hash2, max_speed, avg_speed,                                  "
"    player2_name, player2_ip_int, player2_cuid_hash2,                          "
"    obj1_time, obj2_time, obj3_time, obj4_time, obj5_time,                     "
"    obj6_time, obj7_time, obj8_time, obj9_time, total_time, date,              "
"    match_id, player1_client_id, player2_client_id                             "
"FROM siegefastcaps                                                             "
"WHERE siegefastcaps.mapname = ?1 AND (siegefastcaps.flags & ?2) = ?2           "
"ORDER BY total_time                                                            "
"LIMIT ?3                                                                       ";

const char* const sqlListBestSiegeFastcaps =
"SELECT flags, mapname, player1_name, player1_ip_int, player1_cuid_hash2,       "
"player2_name, player2_ip_int, player2_cuid_hash2,                              "
"MIN( total_time ) AS best_time, date                                           "
"FROM siegefastcaps                                                             "
"WHERE (siegefastcaps.flags & ?1) = ?1                                          "
"GROUP BY mapname                                                               "
"ORDER BY mapname ASC, date ASC                                                 "
"LIMIT ?2                                                                       "
"OFFSET ?3                                                                      ";

//
//  G_LogDbLoad
// 
//  Loads the database from disk, including creating of not exists
//  or if it is corrupted
//
void G_LogDbLoad()
{    
    int rc = -1;    

    rc = sqlite3_initialize();
    rc = sqlite3_open_v2( logDbFileName, &fileDb, SQLITE_OPEN_READWRITE, 0 );

	if (rc == SQLITE_OK) {
		G_LogPrintf("Successfully loaded log database %s\n", logDbFileName);
		// if needed, upgrade legacy servers that don't support cuid_hash yet
		sqlite3_stmt* statement;
		rc = sqlite3_prepare(fileDb, sqlTestCuidSupport, -1, &statement, 0);
		rc = sqlite3_step(statement);
		qboolean foundCuid2 = qfalse, foundCuid1 = qfalse;
		while (rc == SQLITE_ROW) {
			const char *name = (const char*)sqlite3_column_text(statement, 1);
			if (!Q_stricmp(name, "cuid_hash2"))
				foundCuid2 = qtrue;
			if (!Q_stricmp(name, "cuid_hash"))
				foundCuid1 = qtrue;
			rc = sqlite3_step(statement);
		}
		sqlite3_reset(statement);
		rc = sqlite3_prepare(fileDb, sqlTestWhitelistSupport, -1, &statement, 0);
		rc = sqlite3_step(statement);
		qboolean foundWhitelist = qfalse;
		while (rc == SQLITE_ROW) {
			const char *name = (const char*)sqlite3_column_text(statement, 1);
			if (VALIDSTRING(name)) {
				foundWhitelist = qtrue;
				break;
			}
			rc = sqlite3_step(statement);
		}
		if (foundCuid2) {
			//G_LogPrintf("Log database supports cuid 2.0, no upgrade needed.\n");
		}
		else if (foundCuid1) {
			G_LogPrintf("Automatically upgrading old log database: cuid 1.0 support ==> cuid 2.0 support.\n");
			sqlite3_exec(fileDb, sqlUpgradeToCuid2FromCuid1, 0, 0, 0);
		}
		else {
			G_LogPrintf("Automatically upgrading old log database: no cuid support ==> cuid 2.0 support.\n");
			sqlite3_exec(fileDb, sqlUpgradeToCuid2FromNoCuid, 0, 0, 0);
		}
		if (foundWhitelist) {
			//G_LogPrintf("Log database supports whitelist, no upgrade needed.\n");
		}
		else {
			G_LogPrintf("Automatically upgrading old log database: no player whitelist support ==> player whitelist support.\n");
			sqlite3_exec(fileDb, sqlUpgradeToWhitelist, 0, 0, 0);
		}

		sqlite3_reset(statement);
		rc = sqlite3_prepare(fileDb, sqlTestSiegeFastcapsSupport, -1, &statement, 0);
		rc = sqlite3_step(statement);
		qboolean foundFastcaps = qfalse;
		while (rc == SQLITE_ROW) {
			const char *name = (const char*)sqlite3_column_text(statement, 1);
			if (VALIDSTRING(name)) {
				foundFastcaps = qtrue;
				break;
			}
			rc = sqlite3_step(statement);
		}
		sqlite3_finalize(statement);

		// create the database IF NEEDED, since it might have been created before the feature was added
		if (foundFastcaps) {
			//G_LogPrintf("Log database supports whitelist, no upgrade needed.\n");
		}
		else {
			G_LogPrintf("Automatically upgrading old log database: adding siege fastcaps support.\n");
			sqlite3_exec(fileDb, sqlCreateSiegeFastcapsTable, 0, 0, 0);
		}
	}
	else {
		G_LogPrintf("Couldn't find log database %s, creating a new one\n", logDbFileName);
        // create new database
        rc = sqlite3_open_v2( logDbFileName, &fileDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0 );

        sqlite3_exec( fileDb, sqlCreateLogDb, 0, 0, 0 );
		sqlite3_exec(fileDb, sqlCreateSiegeFastcapsTable, 0, 0, 0);
    }
	
	// open in memory
	sqlite3_open_v2(":memory:", &db, SQLITE_OPEN_READWRITE, 0);
	sqlite3_backup *backup = sqlite3_backup_init(db, "main", fileDb, "main");
	if (backup) {
		sqlite3_backup_step(backup, -1);
		sqlite3_backup_finish(backup);
	}
}


//
//  G_LogDbUnload
// 
//  Unloads the database from memory, includes flushing it 
//  to the file.
//
void G_LogDbUnload()
{
	sqlite3_backup *backup = sqlite3_backup_init(fileDb, "main", db, "main");
	if (backup) {
		sqlite3_backup_step(backup, -1);
		sqlite3_backup_finish(backup);
	}
	sqlite3_close(fileDb);
    sqlite3_close(db);
}

static int Clean_FileToMemory(void) {
	int rc = sqlite3_open_v2(":memory:", &db, SQLITE_OPEN_READWRITE, 0);
	if (rc == SQLITE_OK) {
		sqlite3_backup *backup = sqlite3_backup_init(db, "main", fileDb, "main");
		if (backup) {
			int rc = sqlite3_backup_step(backup, -1);
			sqlite3_backup_finish(backup);
			return rc == SQLITE_DONE ? SQLITE_OK : SQLITE_ERROR;
		}
		else {
			return SQLITE_ERROR;
		}
	}
	else {
		return SQLITE_ERROR;
	}
}

static int Clean_MemoryToFile(void) {
	if (!db || !fileDb)
		return SQLITE_ERROR;
	sqlite3_backup *backup = sqlite3_backup_init(fileDb, "main", db, "main");
	if (backup) {
		int rc = sqlite3_backup_step(backup, -1);
		sqlite3_backup_finish(backup);
		return rc == SQLITE_DONE ? SQLITE_OK : SQLITE_ERROR;
	}
	else {
		return SQLITE_ERROR;
	}
}

void G_LogDbClean(void) {
	Com_Printf("Opening database file %s...", logDbFileName);
	sqlite3_initialize();
	int rc = sqlite3_open_v2(logDbFileName, &fileDb, SQLITE_OPEN_READWRITE, 0);
	if (rc != SQLITE_OK) {
		Com_Printf("failed!\n");
		return;
	}
	rc = Clean_FileToMemory();
	if (rc != SQLITE_OK) {
		Com_Printf("failed to open in memory!\n");
		return;
	}
	Com_Printf("done.\n");

	char *query = "SELECT COUNT(*) FROM nicknames;";
	sqlite3_stmt* statement = NULL;
	sqlite3_prepare(db, query, -1, &statement, 0);
	rc = sqlite3_step(statement);
	if (rc != SQLITE_ROW) {
		Com_Printf("No rows found!\n");
		return;
	}
	const int initial = sqlite3_column_int(statement, 0);
	int existing = initial;
	Com_Printf("%d rows before cleaning.\n", existing);

	sqlite3_reset(statement);
	query = "SELECT COUNT(*) FROM nicknames WHERE ip_int = 0;";
	sqlite3_prepare(db, query, -1, &statement, 0);
	rc = sqlite3_step(statement);
	if (rc != SQLITE_ROW) {
		Com_Printf("No rows found!\n");
		return;
	}
	int bots = sqlite3_column_int(statement, 0);
	if (bots) {
		query = "DELETE FROM nicknames WHERE ip_int = 0;";
		sqlite3_exec(db, query, 0, 0, 0);
		Com_Printf("Bot pass: deleted %d bot rows.\n", bots);
		existing -= bots;
	}
	else {
		Com_Printf("Bot pass: no bot rows detected.\n");
	}

	int pass = 1;
	while (1) {
		int offset = 0;
		while (1) {
			sqlite3_reset(statement);
			query = va("SELECT * FROM nicknames LIMIT 1 OFFSET %i;", offset);
			sqlite3_prepare(db, query, -1, &statement, 0);
			rc = sqlite3_step(statement);
			if (rc != SQLITE_ROW) {
				sqlite3_reset(statement);
				query = "SELECT COUNT(*) FROM nicknames;";
				sqlite3_prepare(db, query, -1, &statement, 0);
				rc = sqlite3_step(statement);
				assert(rc == SQLITE_ROW);
				int current = sqlite3_column_int(statement, 0);
				int deleted = existing - current;
				if (deleted) {
					Com_Printf("Pass %d: deleted %d row%s.\n", pass, deleted, deleted == 1 ? "" : "s");
					existing = current;
					pass++;
					break;
				}
				else {
					int percentDeleted = (int)(100 - ((100 * ((double)current / (double)initial)) + 0.5));
					Com_Printf("Cleaning complete: took %d passes, deleted a total of %d rows (%d percent of the table) for a new total of %d row%s.\n", pass - 1, initial - current, percentDeleted, current, current == 1 ? "" : "s");
					sqlite3_finalize(statement);
					Com_Printf("Saving database to disk...");
					rc = Clean_MemoryToFile();
					if (rc != SQLITE_OK) {
						Com_Printf("failed!\n");
						return;
					}
					Com_Printf("Done.\n");
					return;
				}
			}
			int ip = sqlite3_column_int(statement, 0);
			char name[64] = { 0 };
			strcpy(name, (char *)sqlite3_column_text(statement, 1));
			char cuid[64] = { 0 };
			if (sqlite3_column_text(statement, 3))
				strcpy(cuid, (char *)sqlite3_column_text(statement, 3));
			sqlite3_reset(statement);
			if (cuid[0])
				query = va("SELECT SUM(duration) FROM nicknames WHERE ip_int = %i AND name = \"%s\" AND cuid_hash2 = \"%s\";", ip, name, cuid);
			else
				query = va("SELECT SUM(duration) FROM nicknames WHERE ip_int = %i AND name = \"%s\" AND cuid_hash2 IS NULL;", ip, name);
			sqlite3_prepare(db, query, -1, &statement, 0);
			rc = sqlite3_step(statement);
			if (rc == SQLITE_ROW) {
				int totalDuration = sqlite3_column_int(statement, 0);
				if (cuid[0])
					query = va("DELETE FROM nicknames WHERE ip_int = %i AND name = \"%s\" AND cuid_hash2 = \"%s\";", ip, name, cuid);
				else
					query = va("DELETE FROM nicknames WHERE ip_int = %i AND name = \"%s\" AND cuid_hash2 IS NULL;", ip, name);
				sqlite3_exec(db, query, 0, 0, 0);
				if (cuid[0])
					query = va("INSERT INTO nicknames (ip_int, name, duration, cuid_hash2) VALUES (%i, \"%s\", %i, \"%s\");", ip, name, totalDuration, cuid);
				else
					query = va("INSERT INTO nicknames (ip_int, name, duration) VALUES (%i, \"%s\", %i);", ip, name, totalDuration);
				sqlite3_exec(db, query, 0, 0, 0);
			}
			sqlite3_reset(statement);
			offset++;
		}
	}
}

//
//  G_LogDbLogLevelStart
// 
//  Logs current level (map) to the database
//
int G_LogDbLogLevelStart(qboolean isRestart)
{
    sqlite3_stmt* statement;

    // load current map name
    char mapname[128];
    trap_Cvar_VariableStringBuffer( "mapname", mapname, sizeof( mapname ) );

    // prepare insert statement
    sqlite3_prepare( db, sqlLogLevelStart, -1, &statement, 0 );
    sqlite3_bind_text( statement, 1, mapname, -1, 0 );
    sqlite3_bind_int( statement, 2, isRestart ? 1 : 0 );

    sqlite3_step( statement );

    int levelId = sqlite3_last_insert_rowid( db );

    sqlite3_finalize( statement );   

    return levelId;
}

//
//  G_LogDbLogLevelEnd
// 
//  Logs current level (map) to the database
//
void G_LogDbLogLevelEnd( int levelId )
{
    sqlite3_stmt* statement;

    // prepare update statement
    sqlite3_prepare( db, sqlLogLevelEnd, -1, &statement, 0 );
    sqlite3_bind_int( statement, 1, levelId );

    sqlite3_step( statement );

    sqlite3_finalize( statement );
}

//
//  G_LogDbLogLevelEvent
// 
//  Logs level event
//
void G_LogDbLogLevelEvent( int levelId,
    int levelTime,
    int eventId,
    int context1,
    int context2,
    int context3,
    int context4,
    const char* contextText )
{
    sqlite3_stmt* statement;
    // prepare insert statement
    sqlite3_prepare( db, sqlAddLevelEvent, -1, &statement, 0 );

    sqlite3_bind_int( statement, 1, levelId );
    sqlite3_bind_int( statement, 2, levelTime );
    sqlite3_bind_int( statement, 3, eventId );
    sqlite3_bind_int( statement, 4, context1 );
    sqlite3_bind_int( statement, 5, context2 );
    sqlite3_bind_int( statement, 6, context3 );
    sqlite3_bind_int( statement, 7, context4 );
    sqlite3_bind_text( statement, 8, contextText, -1, 0 );

    sqlite3_step( statement );

    sqlite3_finalize( statement );
}
 
//
//  G_LogDbLogSessionStart
// 
//  Logs players connection session start
//
int G_LogDbLogSessionStart( unsigned int ipInt,
    int ipPort,
    int id )
{     
    sqlite3_stmt* statement;
    // prepare insert statement
    sqlite3_prepare( db, sqllogSessionStart, -1, &statement, 0 );

    sqlite3_bind_int( statement, 1, ipInt );
    sqlite3_bind_int( statement, 2, ipPort );
    sqlite3_bind_int( statement, 3, id );

    sqlite3_step( statement );

    int sessionId = sqlite3_last_insert_rowid( db );

    sqlite3_finalize( statement );
    
    return sessionId;
}

//
//  G_LogDbLogSessionEnd
// 
//  Logs players connection session end
//
void G_LogDbLogSessionEnd( int sessionId )
{
    sqlite3_stmt* statement;
    // prepare insert statement
    sqlite3_prepare( db, sqllogSessionEnd, -1, &statement, 0 );

    sqlite3_bind_int( statement, 1, sessionId );

    sqlite3_step( statement );

    sqlite3_finalize( statement );
}

//
//  G_LogDbLogNickname
// 
//  Logs a player's nickname (and cuid hash, if applicable)
//
void G_LogDbLogNickname( unsigned int ipInt,
    const char* name,
    int duration,
	const char* cuidHash)
{
    sqlite3_stmt* statement;

    // prepare insert statement
    sqlite3_prepare( db, VALIDSTRING(cuidHash) ? sqlAddNameNM : sqlAddName, -1, &statement, 0 );

    sqlite3_bind_int( statement, 1, ipInt );
    sqlite3_bind_text( statement, 2, name, -1, 0 );
    sqlite3_bind_int( statement, 3, duration );
	if (VALIDSTRING(cuidHash))
		sqlite3_bind_text(statement, 4, cuidHash, -1, 0);

    sqlite3_step( statement );

    sqlite3_finalize( statement ); 
}

void G_CfgDbListAliases( unsigned int ipInt,
    unsigned int ipMask,
    int limit,
    ListAliasesCallback callback,
    void* context,
	const char* cuidHash)
{
	sqlite3_stmt* statement;
	int rc;
	const char* name;
	int duration;
	if (VALIDSTRING(cuidHash)) { // newmod user; check for cuid matches first before falling back to checking for unique id matches
		int numNMFound = 0;
		rc = sqlite3_prepare(db, sqlCountNMAliases, -1, &statement, 0);
		sqlite3_bind_text(statement, 1, cuidHash, -1, 0);
		sqlite3_bind_int(statement, 2, limit);

		rc = sqlite3_step(statement);
		while (rc == SQLITE_ROW) {
			numNMFound = sqlite3_column_int(statement, 0);
			rc = sqlite3_step(statement);
		}
		sqlite3_reset(statement);

		if (numNMFound) { // we found some cuid matches; let's use these
			rc = sqlite3_prepare(db, sqlGetNMAliases, -1, &statement, 0);
			sqlite3_bind_text(statement, 1, cuidHash, -1, 0);
			sqlite3_bind_int(statement, 2, limit);

			rc = sqlite3_step(statement);
			while (rc == SQLITE_ROW) {
				name = (const char*)sqlite3_column_text(statement, 0);
				duration = sqlite3_column_int(statement, 1);

				callback(context, name, duration);

				rc = sqlite3_step(statement);
			}
			sqlite3_finalize(statement);
		}
		else { // didn't find any cuid matches; use the old unique id method
			rc = sqlite3_prepare(db, sqlGetAliases, -1, &statement, 0);
			sqlite3_bind_int(statement, 1, ipInt);
			sqlite3_bind_int(statement, 2, ipMask);
			sqlite3_bind_int(statement, 3, limit);

			rc = sqlite3_step(statement);
			while (rc == SQLITE_ROW) {
				name = (const char*)sqlite3_column_text(statement, 0);
				duration = sqlite3_column_int(statement, 1);

				callback(context, name, duration);

				rc = sqlite3_step(statement);
			}
			sqlite3_finalize(statement);
		}
	}
	else { // non-newmod; just use the old unique id method
		sqlite3_stmt* statement;
		// prepare insert statement
		int rc = sqlite3_prepare(db, sqlGetAliases, -1, &statement, 0);

		sqlite3_bind_int(statement, 1, ipInt);
		sqlite3_bind_int(statement, 2, ipMask);
		sqlite3_bind_int(statement, 3, limit);

		rc = sqlite3_step(statement);
		while (rc == SQLITE_ROW) {
			name = (const char*)sqlite3_column_text(statement, 0);
			duration = sqlite3_column_int(statement, 1);

			callback(context, name, duration);

			rc = sqlite3_step(statement);
		}

		sqlite3_finalize(statement);
	}
}

int G_DbPlayerWhitelisted(unsigned long long uniqueID, const char* cuidHash) {
	sqlite3_stmt* statement;
	int rc;
	int whitelistedFlags = 0;

	if (VALIDSTRING(cuidHash)) {
		// check for cuid whitelist
		rc = sqlite3_prepare(db, sqlCheckNMPlayerWhitelisted, -1, &statement, 0);
		sqlite3_bind_text(statement, 1, cuidHash, -1, 0);
		rc = sqlite3_step(statement);
		while (rc == SQLITE_ROW) {
			if (sqlite3_column_int(statement, 0) > 0)
				whitelistedFlags |= WHITELISTED_CUID;
			rc = sqlite3_step(statement);
		}
		sqlite3_reset(statement);
	}

	// check for ip whitelist
	rc = sqlite3_prepare(db, sqlCheckPlayerWhitelisted, -1, &statement, 0);
	sqlite3_bind_int64(statement, 1, (signed long long)uniqueID);
	rc = sqlite3_step(statement);
	while (rc == SQLITE_ROW) {
		if (sqlite3_column_int(statement, 0) > 0)
			whitelistedFlags |= WHITELISTED_ID;
		rc = sqlite3_step(statement);
	}
	sqlite3_finalize(statement);

	return whitelistedFlags;
}

void G_DbStorePlayerInWhitelist(unsigned long long uniqueID, const char* cuidHash, const char* name) {
	sqlite3_stmt* statement;
	int rc;

	if (uniqueID && VALIDSTRING(cuidHash)) {
		rc = sqlite3_prepare(db, sqlAddUniqueIDAndCuidToWhitelist, -1, &statement, 0);
		sqlite3_bind_int64(statement, 1, (signed long long)uniqueID);
		sqlite3_bind_text(statement, 2, cuidHash, -1, 0);
		sqlite3_bind_text(statement, 3, name, -1, 0);
		rc = sqlite3_step(statement);
	}
	else if (uniqueID) {
		rc = sqlite3_prepare(db, sqlAddUniqueIDToWhitelist, -1, &statement, 0);
		sqlite3_bind_int64(statement, 1, (signed long long)uniqueID);
		sqlite3_bind_text(statement, 2, name, -1, 0);
		rc = sqlite3_step(statement);
	}
	else if (VALIDSTRING(cuidHash)) {
		rc = sqlite3_prepare(db, sqlAddCuidToWhitelist, -1, &statement, 0);
		sqlite3_bind_text(statement, 1, cuidHash, -1, 0);
		sqlite3_bind_text(statement, 2, name, -1, 0);
		rc = sqlite3_step(statement);
	}

	sqlite3_finalize(statement);
}

void G_DbRemovePlayerFromWhitelist(unsigned long long uniqueID, const char* cuidHash) {
	sqlite3_stmt* statement;

	int rc = sqlite3_prepare(db, VALIDSTRING(cuidHash) ? sqlDeleteNMPlayerFromWhitelist: sqlDeletePlayerFromWhitelist, -1, &statement, 0);

	sqlite3_bind_int64(statement, 1, (signed long long)uniqueID);
	if (VALIDSTRING(cuidHash))
		sqlite3_bind_text(statement, 2, cuidHash, -1, 0);

	rc = sqlite3_step(statement);

	sqlite3_finalize(statement);
}

// strict == qtrue:  only get records where flags in db is exactly equal to flags parameter
// strict == qfalse: only get records where flags in db contains (bitwise AND) flags parameter
void G_LogDbLoadCaptureRecords( const char *mapname, CaptureCategoryFlags flags, qboolean strict, CaptureRecordsForCategory *out ) {
	if (!flags) {
		assert(qfalse);
		return;
	}
	memset(out, 0, sizeof( *out) );

	if ( g_gametype.integer != GT_SIEGE || !g_saveCaptureRecords.integer ) {
		return;
	}

	extern const char *G_GetArenaInfoByMap( const char *map );
	const char *arenaInfo = G_GetArenaInfoByMap( mapname );

	if ( VALIDSTRING( arenaInfo ) ) {
		const char *mapFlags = Info_ValueForKey( arenaInfo, "b_e_flags" );
		
		// this flag disables toptimes on this map
		// TODO: if I ever make more flags, make an actual define in some header file...
		if ( VALIDSTRING( mapFlags ) && atoi( mapFlags ) & 1 ) {
			return;
		}
	}

	int loaded = 0;

	sqlite3_stmt* statement;
	int rc = sqlite3_prepare(db, strict ? sqlGetSiegeFastcapsStrict : sqlGetSiegeFastcapsRelaxed, -1, &statement, 0);

	sqlite3_bind_text( statement, 1, mapname, -1, 0 );
	sqlite3_bind_int( statement, 2, flags );
	sqlite3_bind_int( statement, 3, MAX_SAVED_RECORDS );

	rc = sqlite3_step( statement );

	int j = 0;
	while ( rc == SQLITE_ROW && j < MAX_SAVED_RECORDS ) {
		CaptureRecord *record = &out->records[j];
		int k = 0;
		record->flags = sqlite3_column_int(statement, k++);
		const char *player1_name = ( const char* )sqlite3_column_text( statement, k++);
		if (VALIDSTRING(player1_name))
			Q_strncpyz(record->recordHolder1Name, player1_name, sizeof(record->recordHolder1Name));
		record->recordHolder1IpInt  = sqlite3_column_int( statement, k++);
		const char *player1_cuid_hash2 = (const char*)sqlite3_column_text(statement, k++);
		if (VALIDSTRING(player1_cuid_hash2))
			Q_strncpyz(record->recordHolder1Cuid, player1_cuid_hash2, sizeof(record->recordHolder1Cuid));
		record->maxSpeed1 = sqlite3_column_int(statement, k++);
		record->avgSpeed1 = sqlite3_column_int(statement, k++);

		if (flags & CAPTURERECORDFLAG_COOP) {
			const char *player2_name = (const char*)sqlite3_column_text(statement, k++);
			if (VALIDSTRING(player2_name))
				Q_strncpyz(record->recordHolder2Name, player2_name, sizeof(record->recordHolder2Name));
			record->recordHolder2IpInt = sqlite3_column_int(statement, k++);
			const char *player2_cuid_hash2 = (const char*)sqlite3_column_text(statement, k++);
			if (VALIDSTRING(player2_cuid_hash2))
				Q_strncpyz(record->recordHolder2Cuid, player2_cuid_hash2, sizeof(record->recordHolder2Cuid));
		}
		else {
			k += 3;
		}

		if (flags & CAPTURERECORDFLAG_FULLMAP) { // round time includes times for each obj, too
			for (int l = 0; l < MAX_SAVED_OBJECTIVES; l++) {
				record->objTimes[k] = sqlite3_column_int(statement, k + l);
				k++;
			}
		}
		else {
			k += MAX_SAVED_OBJECTIVES;
		}

		record->totalTime = sqlite3_column_int(statement, k++);
		record->date = sqlite3_column_int64( statement, k++);
		const char *match_id = ( const char* )sqlite3_column_text( statement, k++);
		if (VALIDSTRING(match_id))
			Q_strncpyz(record->matchId, match_id, sizeof(record->matchId));
		record->recordHolder1ClientId = sqlite3_column_int( statement, k++);
		if (flags & CAPTURERECORDFLAG_COOP)
			record->recordHolder2ClientId = sqlite3_column_int(statement, k++);
		else
			k++;

		rc = sqlite3_step( statement );
		++loaded;
		++j;
	}
	sqlite3_finalize(statement);

	// write the remaining global fields
	//recordsToLoad->enabled = qtrue;

#ifdef _DEBUG
	G_Printf( "Loaded %d capture time records from database %s flags %d (%s)\n", loaded, strict ? "matching" : "containing", flags, GetLongNameForRecordFlags(mapname, flags, qfalse));
#endif
}

void G_LogDbListAllMapsCaptureRecords(CaptureCategoryFlags flags, int limit, int offset, ListAllMapsCapturesCallback callback, void *context ) {
	if (!flags) {
		assert(qfalse);
		return;
	}
	sqlite3_stmt* statement;
	int rc = sqlite3_prepare( db, sqlListBestSiegeFastcaps, -1, &statement, 0 );

	sqlite3_bind_int( statement, 1, flags );
	sqlite3_bind_int( statement, 2, limit );
	sqlite3_bind_int( statement, 3, offset );

	rc = sqlite3_step( statement );
	while ( rc == SQLITE_ROW ) {
		int k = 0;
		const int thisRecordFlags = sqlite3_column_int(statement, k++);
		const char *mapname = ( const char* )sqlite3_column_text( statement, k++ );
		const char *player1_name = ( const char* )sqlite3_column_text( statement, k++);
		const unsigned int player1_ip_int = sqlite3_column_int( statement, k++);
		const char *player1_cuid_hash2 = ( const char* )sqlite3_column_text( statement, k++);
		const char *player2_name = (const char*)sqlite3_column_text(statement, k++);
		const unsigned int player2_ip_int = sqlite3_column_int(statement, k++);
		const char *player2_cuid_hash2 = (const char*)sqlite3_column_text(statement, k++);
		const int best_time = sqlite3_column_int( statement, k++);
		const time_t date = sqlite3_column_int64( statement, k++);

		callback( context, mapname, flags, thisRecordFlags, player1_name, player1_ip_int, player1_cuid_hash2, player2_name, player2_ip_int, player2_cuid_hash2, best_time, date );
		rc = sqlite3_step( statement );
	}

	sqlite3_finalize( statement );
}

static qboolean SaveCapturesForCategory(CaptureRecordsForCategory *recordsForCategory, void *mapname) {
	if (!recordsForCategory->flags) {
		assert(qfalse);
		return qtrue;
	}

	{
		// first, delete all of the old records for this category on this map, even those that didn't change
		sqlite3_stmt* statement;
		sqlite3_prepare(db, sqlremoveSiegeFastcaps, -1, &statement, 0);
		sqlite3_bind_text(statement, 1, mapname, -1, 0);
		sqlite3_bind_int(statement, 2, recordsForCategory->flags);
		int rc = sqlite3_step(statement);
		if (rc != SQLITE_DONE)
			Com_Printf("SaveCapturesForCategory: error deleting\n");
		sqlite3_finalize(statement);
	}

	int saved = 0;
	for (int j = 0; j < MAX_SAVED_RECORDS; ++j) {
		CaptureRecord *record = &recordsForCategory->records[j];
		if (!record->totalTime)
			continue; // not a valid record

		sqlite3_stmt *statement;
		sqlite3_prepare(db, sqlAddSiegeFastcapV2, -1, &statement, 0);

		int k = 1;
		sqlite3_bind_text(statement, k++, mapname, -1, 0);
		sqlite3_bind_int(statement, k++, recordsForCategory->flags);
		sqlite3_bind_text(statement, k++, GetLongNameForRecordFlags(level.mapCaptureRecords.mapname, recordsForCategory->flags, qtrue), -1, 0);

		sqlite3_bind_text(statement, k++, record->recordHolder1Name, -1, 0);
		sqlite3_bind_int(statement, k++, record->recordHolder1IpInt);
		if (VALIDSTRING(record->recordHolder1Cuid))
			sqlite3_bind_text(statement, k++, record->recordHolder1Cuid, -1, 0);
		else
			sqlite3_bind_null(statement, k++);
		sqlite3_bind_int(statement, k++, record->maxSpeed1);
		sqlite3_bind_int(statement, k++, record->avgSpeed1);

		if (recordsForCategory->flags & CAPTURERECORDFLAG_COOP) {
			sqlite3_bind_text(statement, k++, record->recordHolder2Name, -1, 0);
			sqlite3_bind_int(statement, k++, record->recordHolder2IpInt);
			if (VALIDSTRING(record->recordHolder2Cuid))
				sqlite3_bind_text(statement, k++, record->recordHolder2Cuid, -1, 0);
			else
				sqlite3_bind_null(statement, k++);
		}
		else {
			sqlite3_bind_null(statement, k++);
			sqlite3_bind_null(statement, k++);
			sqlite3_bind_null(statement, k++);
		}

		for (int l = 0; l < MAX_SAVED_OBJECTIVES; l++) {
			if (recordsForCategory->flags & CAPTURERECORDFLAG_FULLMAP && record->objTimes[l] != -1)
				sqlite3_bind_int(statement, k++, record->objTimes[l]);
			else
				sqlite3_bind_null(statement, k++);
		}

		sqlite3_bind_int(statement, k++, record->totalTime);
		sqlite3_bind_int64(statement, k++, record->date);

		if (VALIDSTRING(record->matchId))
			sqlite3_bind_text(statement, k++, record->matchId, -1, 0);
		else
			sqlite3_bind_null(statement, k++);

		sqlite3_bind_int(statement, k++, record->recordHolder1ClientId);
		if (recordsForCategory->flags & CAPTURERECORDFLAG_COOP)
			sqlite3_bind_int(statement, k++, record->recordHolder2ClientId);
		else
			sqlite3_bind_null(statement, k++);

		int rc = sqlite3_step(statement);
		if (rc != SQLITE_DONE)
			Com_Printf("SaveCapturesForCategory: error saving\n");
		sqlite3_finalize(statement);
		++saved;
	}

#ifdef _DEBUG
	G_Printf("Saved %d fastcap records with flags %d (%s) to database\n", saved, recordsForCategory->flags, GetLongNameForRecordFlags(mapname, recordsForCategory->flags, qtrue));
#endif

	return qtrue;
}

void G_LogDbSaveCaptureRecords( CaptureRecordsContext *context )
{
	if ( g_gametype.integer != GT_SIEGE || !g_saveCaptureRecords.integer || !context->changed ) {
		return;
	}

	ListForEach(&context->captureRecordsList, SaveCapturesForCategory, context->mapname);
}