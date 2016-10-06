/*******************************************************************************
 *
 *                             Messenger Backend
 *     Copyright (C) 2016 Björn Petersen Software Design and Development
 *                   Contact: r10s@b44t.com, http://b44t.com
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/ .
 *
 *******************************************************************************
 *
 * File:    mrsqlite3.h
 * Authors: Björn Petersen
 * Purpose: MrSqlite3 wraps around SQLite
 *
 *******************************************************************************
 *
 * NB: In general, function names ending with a `_` are private functions and
 * should not be called directly from outside the library.
 * For functions with database access, this generally also implies that _no_
 * locking takes place inside the functions!  So the caller must make sure, the
 * database is locked as needed.
 *
 ******************************************************************************/


#ifndef __MRSQLITE3_H__
#define __MRSQLITE3_H__
#ifdef __cplusplus
extern "C" {
#endif


/*** library-private **********************************************************/

#include <sqlite3.h>
#include <libetpan/libetpan.h>
#include <pthread.h>
typedef struct mrmailbox_t mrmailbox_t; /*forward declaration*/


/* predefined statements */
enum
{
	 BEGIN_transaction = 0 /* must be first */
	,ROLLBACK_transaction
	,COMMIT_transaction

	,SELECT_v_FROM_config_k
	,INSERT_INTO_config_kv
	,UPDATE_config_vk
	,DELETE_FROM_config_k

	,SELECT_COUNT_FROM_contacts
	,SELECT_ine_FROM_contacts_i
	,SELECT_in_FROM_contacts_a
	,INSERT_INTO_contacts_ne
	,UPDATE_contacts_ni

	,SELECT_COUNT_FROM_chats
	,SELECT_itnifttsm_FROM_chatsNmsgs
	,SELECT_itn_FROM_chats_i

	,SELECT_a_FROM_chats_contacts_WHERE_i
	,SELECT_COUNT_FROM_chats_contacts_WHERE_i
	,UPDATE_chats_dd

	,SELECT_COUNT_FROM_msg_WHERE_assigned
	,SELECT_COUNT_FROM_msg_WHERE_unassigned
	,SELECT_i_FROM_msg_m
	,SELECT_icfttstpb_FROM_msg_i
	,INSERT_INTO_msg_mcfttsmp
	,INSERT_INTO_msg_to_mc

	,PREDEFINED_CNT /* must be last */
};


typedef struct mrsqlite3_t
{
	/* prepared statements - this is the favourite way for the caller to use SQLite */
	sqlite3_stmt* m_pd[PREDEFINED_CNT];

	/* m_sqlite is the database given as dbfile to Open() */
	sqlite3*      m_cobj;

	/* helper for MrSqlite3Transaction */
	int           m_transactionCount;

	mrmailbox_t*  m_mailbox;

	/* the user must make sure, only one thread uses sqlite at the same time!
	for this purpose, all calls must be enclosed by a locked m_critical; use mrsqlite3_lock() for this purpose */
	pthread_mutex_t m_critical_;
} mrsqlite3_t;


mrsqlite3_t*  mrsqlite3_new              (mrmailbox_t*);
void          mrsqlite3_unref            (mrsqlite3_t*);
int           mrsqlite3_open             (mrsqlite3_t*, const char* dbfile);
void          mrsqlite3_close            (mrsqlite3_t*);
int           mrsqlite3_is_open          (mrsqlite3_t*);

/* handle configurations, private */
int           mrsqlite3_set_config_      (mrsqlite3_t*, const char* key, const char* value);
int           mrsqlite3_set_config_int_  (mrsqlite3_t*, const char* key, int32_t value);
char*         mrsqlite3_get_config_      (mrsqlite3_t*, const char* key, const char* def); /* the returned string must be free()'d, returns NULL on errors */
int32_t       mrsqlite3_get_config_int_  (mrsqlite3_t*, const char* key, int32_t def);

/* tools, these functions are compatible to the corresponding sqlite3_* functions */
sqlite3_stmt* mrsqlite3_predefine        (mrsqlite3_t*, size_t idx, const char* sql); /*the result is resetted as needed and must not be freed. CAVE: you must not call this function with different strings for the same index!*/
sqlite3_stmt* mrsqlite3_prepare_v2_      (mrsqlite3_t*, const char* sql); /* the result mus be freed using sqlite3_finalize() */
int           mrsqlite3_execute          (mrsqlite3_t*, const char* sql);
int           mrsqlite3_table_exists     (mrsqlite3_t*, const char* name);
void          mrsqlite3_log_error        (mrsqlite3_t*);

/* tools for locking, may be called nested, see also m_critical_ above.
the user of MrSqlite3 must make sure that the MrSqlite3-object is only used by one thread at the same time.
In general, we will lock the hightest level as possible - this avoids deadlocks and massive on/off lockings.
Low-level-functions, eg. the MrSqlite3-methods, do not lock. */
void          mrsqlite3_lock             (mrsqlite3_t*); /* lock or wait; CAVE: These calls must not be nested in a single thrad*/
void          mrsqlite3_unlock           (mrsqlite3_t*);

/* nestable transactions, only the outest is really used */
void          mrsqlite3_begin_transaction(mrsqlite3_t*);
void          mrsqlite3_commit           (mrsqlite3_t*);
void          mrsqlite3_rollback         (mrsqlite3_t*);

#ifdef __cplusplus
} /* /extern "C" */
#endif
#endif /* __MRSQLITE3_H__ */

