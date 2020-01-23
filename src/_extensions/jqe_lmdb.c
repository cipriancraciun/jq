



#include "jve.h"
#include "jqe_lmdb.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lmdb.h>




struct jqe_lmdb {
	char * path;
	MDB_env * environment;
	MDB_dbi database;
	bool multidata;
};


#define JQE_LMDB_STACK_SIZE_MAX 16
static jqe_lmdb jqe_lmdb_stack[JQE_LMDB_STACK_SIZE_MAX];
static int jqe_lmdb_stack_size = 0;


static jv jqe_lmdb_data_input (MDB_val * data, jv value, bool encode);
static jv jqe_lmdb_data_output (MDB_val * data, bool decode);




jv jqe_lmdb_select (jq_state * jq, jv input, jv path, jv on_missing)
{
	return (jqe_lmdb_select_ll (jq, input, path, jv_copy (input), on_missing, jv_true ()));
}

jv jqe_lmdb_update (jq_state * jq, jv input, jv path, jv key)
{
	return (jqe_lmdb_update_ll (jq, input, path, key, jv_copy (input), jv_true ()));
}

jv jqe_lmdb_include (jq_state * jq, jv input, jv path, jv key, jv on_existing)
{
	return (jqe_lmdb_include_ll (jq, input, path, key, jv_copy (input), on_existing, jv_true ()));
}

jv jqe_lmdb_exclude (jq_state * jq, jv input, jv path, jv on_missing)
{
	return (jqe_lmdb_exclude_ll (jq, input, path, jv_copy (input), on_missing, jv_true ()));
}

jv jqe_lmdb_keys (jq_state * jq, jv input, jv path)
{
	return (jqe_lmdb_keys_ll (jq, input, path, jv_true ()));
}




jv jqe_lmdb_select_ll (jq_state * jq, jv input, jv path, jv key, jv on_missing, jv encode_)
{
	jv outcome = jv_invalid ();
	
	if ((jv_get_kind (encode_) != JV_KIND_TRUE) && (jv_get_kind (encode_) != JV_KIND_FALSE)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_select` requires boolean `encode`"));
		goto exit;
	}
	
	bool encode = jv_get_kind (encode_) == JV_KIND_TRUE;
	
	jqe_lmdb * lmdb;
	jv_free (outcome);
	outcome = jqe_lmdb_resolve (jq, jv_copy (path), &lmdb);
	if (lmdb == NULL)
		goto exit;
	
	jv_free (outcome);
	if (lmdb->multidata)
		outcome = jqe_lmdb_read_all_0 (lmdb, jv_copy (key), jv_copy (on_missing), encode);
	else
		outcome = jqe_lmdb_read_one_0 (lmdb, jv_copy (key), jv_copy (on_missing), encode);
	
exit :
	
	jv_free (input);
	jv_free (path);
	jv_free (key);
	jv_free (on_missing);
	jv_free (encode_);
	
	return (outcome);
}


jv jqe_lmdb_update_ll (jq_state * jq, jv input, jv path, jv key, jv value, jv encode_)
{
	jv outcome = jv_invalid ();
	
	if ((jv_get_kind (encode_) != JV_KIND_TRUE) && (jv_get_kind (encode_) != JV_KIND_FALSE)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_update` requires boolean `encode`"));
		goto exit;
	}
	
	bool encode = jv_get_kind (encode_) == JV_KIND_TRUE;
	
	jqe_lmdb * lmdb;
	jv_free (outcome);
	outcome = jqe_lmdb_resolve (jq, jv_copy (path), &lmdb);
	if (lmdb == NULL)
		goto exit;
	
	jv_free (outcome);
	outcome = jqe_lmdb_write_0 (lmdb, jv_copy (key), jv_copy (value), true, jv_copy (input), jv_copy (input), encode);
	
exit :
	
	jv_free (input);
	jv_free (path);
	jv_free (key);
	jv_free (value);
	jv_free (encode_);
	
	return (outcome);
}


jv jqe_lmdb_include_ll (jq_state * jq, jv input, jv path, jv key, jv value, jv on_existing, jv encode_)
{
	jv outcome = jv_invalid ();
	
	if ((jv_get_kind (encode_) != JV_KIND_TRUE) && (jv_get_kind (encode_) != JV_KIND_FALSE)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_include` requires boolean `encode`"));
		goto exit;
	}
	
	bool encode = jv_get_kind (encode_) == JV_KIND_TRUE;
	
	jqe_lmdb * lmdb;
	jv_free (outcome);
	outcome = jqe_lmdb_resolve (jq, jv_copy (path), &lmdb);
	if (lmdb == NULL)
		goto exit;
	
	jv_free (outcome);
	outcome = jqe_lmdb_write_0 (lmdb, jv_copy (key), jv_copy (value), false, jv_copy (input), jv_copy (on_existing), encode);
	
exit :
	
	jv_free (input);
	jv_free (path);
	jv_free (key);
	jv_free (value);
	jv_free (on_existing);
	jv_free (encode_);
	
	return (outcome);
}


jv jqe_lmdb_exclude_ll (jq_state * jq, jv input, jv path, jv key, jv on_missing, jv encode_)
{
	jv outcome = jv_invalid ();
	
	if ((jv_get_kind (encode_) != JV_KIND_TRUE) && (jv_get_kind (encode_) != JV_KIND_FALSE)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_exclude` requires boolean `encode`"));
		goto exit;
	}
	
	bool encode = jv_get_kind (encode_) == JV_KIND_TRUE;
	
	jqe_lmdb * lmdb;
	jv_free (outcome);
	outcome = jqe_lmdb_resolve (jq, jv_copy (path), &lmdb);
	if (lmdb == NULL)
		goto exit;
	
	jv_free (outcome);
	outcome = jqe_lmdb_delete_0 (lmdb, jv_copy (key), jv_copy (input), jv_copy (on_missing), encode);
	
exit :
	
	jv_free (input);
	jv_free (path);
	jv_free (key);
	jv_free (on_missing);
	jv_free (encode_);
	
	return (outcome);
}


jv jqe_lmdb_keys_ll (jq_state * jq, jv input, jv path, jv encode_)
{
	jv outcome = jv_invalid ();
	
	if ((jv_get_kind (encode_) != JV_KIND_TRUE) && (jv_get_kind (encode_) != JV_KIND_FALSE)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_keys` requires boolean `encode`"));
		goto exit;
	}
	
	bool encode = jv_get_kind (encode_) == JV_KIND_TRUE;
	
	jqe_lmdb * lmdb;
	jv_free (outcome);
	outcome = jqe_lmdb_resolve (jq, jv_copy (path), &lmdb);
	if (lmdb == NULL)
		goto exit;
	
	jv_free (outcome);
	outcome = jqe_lmdb_keys_0 (lmdb, encode);
	
exit :
	
	jv_free (input);
	jv_free (path);
	jv_free (encode_);
	
	return (outcome);
}




jv jqe_lmdb_read_one_0 (jqe_lmdb * lmdb, jv key, jv on_missing, bool encode)
{
	jv outcome = jv_invalid ();
	jv key_encoded = jv_invalid ();
	MDB_txn * lmdb_transaction;
	MDB_val lmdb_key;
	MDB_val lmdb_value;
	int mdb_outcome;
	
	if (!encode && (jv_get_kind (key) != JV_KIND_STRING)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_read` requires string `key`"));
		goto exit;
	}
	
	key_encoded = jqe_lmdb_data_input (&lmdb_key, jv_copy (key), encode);
	if (!jv_is_valid (key_encoded)) {
		jv_free (outcome);
		outcome = jv_copy (key_encoded);
		goto exit;
	}
	
	unsigned int lmdb_transaction_flags = MDB_RDONLY;
	
	mdb_outcome = mdb_txn_begin (lmdb->environment, NULL, lmdb_transaction_flags, &lmdb_transaction);
	if (mdb_outcome != 0) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  error encountered while beginning transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	jv_free (outcome);
	mdb_outcome = mdb_get (lmdb_transaction, lmdb->database, &lmdb_key, &lmdb_value);
	if (mdb_outcome == 0)
		outcome = jqe_lmdb_data_output (&lmdb_value, encode);
	else if (mdb_outcome == MDB_NOTFOUND)
		outcome = jv_copy (on_missing);
	else if (mdb_outcome != MDB_NOTFOUND)
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  error encountered while reading record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
	else
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  assertion encountered while reading record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
	
	mdb_outcome = mdb_txn_commit (lmdb_transaction);
	if (mdb_outcome != 0) {
		if (jv_is_valid (outcome)) {
			jv_free (outcome);
			outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  error encountered while committing transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		}
		goto exit;
	}
	
exit :
	
	jv_free (key);
	jv_free (key_encoded);
	jv_free (on_missing);
	
	return (outcome);
}


jv jqe_lmdb_read_all_0 (jqe_lmdb * lmdb, jv key, jv on_missing, bool encode)
{
	jv outcome = jv_invalid ();
	jv key_encoded = jv_invalid ();
	MDB_txn * lmdb_transaction;
	MDB_cursor * lmdb_cursor;
	MDB_val lmdb_key;
	MDB_val lmdb_value;
	int mdb_outcome;
	
	if (!encode && (jv_get_kind (key) != JV_KIND_STRING)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_read` requires string `key`"));
		goto exit;
	}
	
	key_encoded = jqe_lmdb_data_input (&lmdb_key, jv_copy (key), encode);
	if (!jv_is_valid (key_encoded)) {
		jv_free (outcome);
		outcome = jv_copy (key_encoded);
		goto exit;
	}
	
	unsigned int lmdb_transaction_flags = MDB_RDONLY;
	
	mdb_outcome = mdb_txn_begin (lmdb->environment, NULL, lmdb_transaction_flags, &lmdb_transaction);
	if (mdb_outcome != 0) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  error encountered while beginning transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	mdb_outcome = mdb_cursor_open (lmdb_transaction, lmdb->database, &lmdb_cursor);
	if (mdb_outcome != 0) {
		if (jv_is_valid (outcome)) {
			jv_free (outcome);
			outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  error encountered while opening cursor  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		}
		goto abort_2;
	}
	
	mdb_outcome = mdb_cursor_get (lmdb_cursor, &lmdb_key, NULL, MDB_SET);
	if ((mdb_outcome != 0) && (mdb_outcome != MDB_NOTFOUND)) {
		if (jv_is_valid (outcome)) {
			jv_free (outcome);
			outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  error encountered while reading record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		}
		goto abort_1;
	}
	
	if (mdb_outcome == MDB_NOTFOUND) {
		
		jv_free (outcome);
		outcome = jv_copy (on_missing);
		
	} else if (mdb_outcome == 0) {
		
		// FIXME:  Try to better estimate outcome size!
		
		jv_free (outcome);
		outcome = jv_array_sized (1024);
		bool first = true;
		
		while (true) {
			
			if (first)
				mdb_outcome = mdb_cursor_get (lmdb_cursor, &lmdb_key, &lmdb_value, MDB_GET_CURRENT);
			else
				mdb_outcome = mdb_cursor_get (lmdb_cursor, &lmdb_key, &lmdb_value, MDB_NEXT_DUP);
			first = false;
			if ((mdb_outcome != 0) && (mdb_outcome != MDB_NOTFOUND)) {
				if (jv_is_valid (outcome)) {
					jv_free (outcome);
					outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  error encountered while reading record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
				}
				goto abort_1;
			}
			
			jv value;
			if (mdb_outcome == 0)
				value = jqe_lmdb_data_output (&lmdb_value, encode);
			else if (mdb_outcome == MDB_NOTFOUND)
				break;
			else
				value = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  assertion encountered while reading record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
			
			if (jv_is_valid (value)) {
				outcome = jv_array_append (outcome, value);
			} else {
				jv_free (outcome);
				outcome = value;
				goto abort_1;
			}
		}
		
	} else {
		
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  assertion encountered while reading record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		
	}
	
abort_1 :
	
	mdb_cursor_close (lmdb_cursor);
	
abort_2 :
	
	mdb_outcome = mdb_txn_commit (lmdb_transaction);
	if (mdb_outcome != 0) {
		if (jv_is_valid (outcome)) {
			jv_free (outcome);
			outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_read` failed:  error encountered while committing transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		}
		goto exit;
	}
	
exit :
	
	jv_free (key);
	jv_free (key_encoded);
	jv_free (on_missing);
	
	return (outcome);
}


jv jqe_lmdb_write_0 (jqe_lmdb * lmdb, jv key, jv value, bool update, jv on_success, jv on_missupdate, bool encode)
{
	jv outcome = jv_invalid ();
	jv key_encoded = jv_invalid ();
	jv value_encoded = jv_invalid ();
	MDB_txn * lmdb_transaction;
	MDB_val lmdb_key;
	MDB_val lmdb_value;
	int mdb_outcome;
	
	if (lmdb->multidata && update) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_write` does not allow updating multi-value database"));
		goto exit;
	}
	
	if (!encode && (jv_get_kind (key) != JV_KIND_STRING)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_write` requires string `key`"));
		goto exit;
	}
	if (!encode && (jv_get_kind (value) != JV_KIND_STRING)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_write` requires string `value`"));
		goto exit;
	}
	
	key_encoded = jqe_lmdb_data_input (&lmdb_key, jv_copy (key), encode);
	if (!jv_is_valid (key_encoded)) {
		jv_free (outcome);
		outcome = jv_copy (key_encoded);
		goto exit;
	}
	
	value_encoded = jqe_lmdb_data_input (&lmdb_value, jv_copy (value), encode);
	if (!jv_is_valid (value_encoded)) {
		jv_free (outcome);
		outcome = jv_copy (value_encoded);
		goto exit;
	}
	
	unsigned int lmdb_transaction_flags = 0;
	unsigned int lmdb_put_flags = (lmdb->multidata || update) ? 0 : MDB_NOOVERWRITE;
	
	mdb_outcome = mdb_txn_begin (lmdb->environment, NULL, lmdb_transaction_flags, &lmdb_transaction);
	if (mdb_outcome != 0) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_write` failed:  error encountered while beginning transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	mdb_outcome = mdb_put (lmdb_transaction, lmdb->database, &lmdb_key, &lmdb_value, lmdb_put_flags);
	if ((mdb_outcome != 0) && (mdb_outcome != MDB_KEYEXIST)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_write` failed:  error encountered while writing record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto abort;
	}
	
	jv_free (outcome);
	if (mdb_outcome == 0)
		outcome = jv_copy (on_success);
	else if (mdb_outcome == MDB_KEYEXIST)
		outcome = jv_copy (on_missupdate);
	else
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_write` failed:  assertion encountered while writing record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
	
abort :
	
	mdb_outcome = mdb_txn_commit (lmdb_transaction);
	if (mdb_outcome != 0) {
		if (jv_is_valid (outcome)) {
			jv_free (outcome);
			outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_write` failed:  error encountered while committing transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		}
		goto exit;
	}
	
exit :
	
	jv_free (key);
	jv_free (key_encoded);
	jv_free (value);
	jv_free (value_encoded);
	jv_free (on_success);
	jv_free (on_missupdate);
	
	return (outcome);
}


jv jqe_lmdb_delete_0 (jqe_lmdb * lmdb, jv key, jv on_success, jv on_missing, bool encode)
{
	jv outcome = jv_invalid ();
	jv key_encoded = jv_invalid ();
	MDB_txn * lmdb_transaction;
	MDB_val lmdb_key;
	int mdb_outcome;
	
	if (!encode && (jv_get_kind (key) != JV_KIND_STRING)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string ("`jqe_lmdb_delete` requires string `key`"));
		goto exit;
	}
	
	key_encoded = jqe_lmdb_data_input (&lmdb_key, jv_copy (key), encode);
	if (!jv_is_valid (key_encoded)) {
		jv_free (outcome);
		outcome = jv_copy (key_encoded);
		goto exit;
	}
	
	unsigned int lmdb_transaction_flags = 0;
	
	mdb_outcome = mdb_txn_begin (lmdb->environment, NULL, lmdb_transaction_flags, &lmdb_transaction);
	if (mdb_outcome != 0) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_delete` failed:  error encountered while beginning transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	mdb_outcome = mdb_del (lmdb_transaction, lmdb->database, &lmdb_key, NULL);
	if ((mdb_outcome != 0) && (mdb_outcome != MDB_NOTFOUND)) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_delete` failed:  error encountered while deleting record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto abort;
	}
	
	jv_free (outcome);
	if (mdb_outcome == 0)
		outcome = jv_copy (on_success);
	else if (mdb_outcome == MDB_NOTFOUND)
		outcome = jv_copy (on_missing);
	else
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_write` failed:  assertion encountered while writing record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
	
abort :
	
	mdb_outcome = mdb_txn_commit (lmdb_transaction);
	if (mdb_outcome != 0) {
		if (jv_is_valid (outcome)) {
			jv_free (outcome);
			outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_delete` failed:  error encountered while committing transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		}
		goto exit;
	}
	
exit :
	
	jv_free (key);
	jv_free (key_encoded);
	jv_free (on_success);
	jv_free (on_missing);
	
	return (outcome);
}


jv jqe_lmdb_keys_0 (jqe_lmdb * lmdb, bool encode)
{
	jv outcome = jv_invalid ();
	MDB_txn * lmdb_transaction;
	MDB_cursor * lmdb_cursor;
	MDB_val lmdb_key;
	MDB_val lmdb_value;
	int mdb_outcome;
	
	unsigned int lmdb_transaction_flags = MDB_RDONLY;
	
	mdb_outcome = mdb_txn_begin (lmdb->environment, NULL, lmdb_transaction_flags, &lmdb_transaction);
	if (mdb_outcome != 0) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_keys` failed:  error encountered while beginning transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	mdb_outcome = mdb_cursor_open (lmdb_transaction, lmdb->database, &lmdb_cursor);
	if (mdb_outcome != 0) {
		jv_free (outcome);
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_keys` failed:  error encountered while opening cursor  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto abort_2;
	}
	
	// FIXME:  Try to better estimate outcome size!
	
	jv_free (outcome);
	outcome = jv_array_sized (1024);
	
	while (true) {
		
		mdb_outcome = mdb_cursor_get (lmdb_cursor, &lmdb_key, &lmdb_value, MDB_NEXT_NODUP);
		if ((mdb_outcome != 0) && (mdb_outcome != MDB_NOTFOUND)) {
			if (jv_is_valid (outcome)) {
				jv_free (outcome);
				outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_keys` failed:  error encountered while reading record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
			}
			goto abort_1;
		}
		
		jv key;
		if (mdb_outcome == 0)
			key = jqe_lmdb_data_output (&lmdb_key, encode);
		else if (mdb_outcome == MDB_NOTFOUND)
			break;
		else
			key = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_keys` failed:  assertion encountered while reading record  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		
		if (jv_is_valid (key))
			outcome = jv_array_append (outcome, key);
		else {
			jv_free (outcome);
			outcome = key;
			goto abort_1;
		}
	}
	
abort_1 :
	
	mdb_cursor_close (lmdb_cursor);
	
abort_2 :
	
	mdb_outcome = mdb_txn_commit (lmdb_transaction);
	if (mdb_outcome != 0) {
		if (jv_is_valid (outcome)) {
			jv_free (outcome);
			outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_keys` failed:  error encountered while committing transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		}
		goto exit;
	}
	
exit :
	
	return (outcome);
}




jv jqe_lmdb_resolve (jq_state * jq, jv path, jqe_lmdb * * lmdb_)
{
	*lmdb_ = NULL;
	
	
	if (jv_get_kind (path) != JV_KIND_STRING) {
		jv_free (path);
		return (jv_invalid_with_msg (jv_string ("`jqe_lmdb_resolve` requires string `path`")));
	}
	
	char const * path_value = jv_string_value (path);
	
	
	for (int i = 0; i < jqe_lmdb_stack_size; i++)
		if (strcmp (jqe_lmdb_stack[i].path, path_value) == 0) {
			*lmdb_ = &jqe_lmdb_stack[i];
			return (jv_true ());
		}
	
	if (jqe_lmdb_stack_size >= JQE_LMDB_STACK_SIZE_MAX)
		return (jv_invalid_with_msg (jv_string ("`jqe_lmdb_resolve` failed:  too many opened databases")));
	
	
	char * lmdb_path = strdup (path_value);
	assert (lmdb_path != NULL);
	
	char lmdb_type = lmdb_path[0];
	if (lmdb_type == '\0')
		return (jv_invalid_with_msg (jv_string ("`jqe_lmdb_resolve` failed:  empty path")));
	
	
	// FIXME:  Make the environment size parameter configurable via an environment variable!
	size_t lmdb_environment_size = (8L * 1024L * 1024L * 1024L) / getpagesize () * getpagesize ();
	// FIXME:  Make the environment readers parameter configurable via an environment variable!
	unsigned int lmdb_environment_readers = 128;
	bool lmdb_multidata = false;
	
	
	if (lmdb_type == '+')
		lmdb_multidata = true;
	else if (lmdb_type == ':')
		lmdb_multidata = false;
	else
		return (jv_invalid_with_msg (jv_string ("`jqe_lmdb_resolve` failed:  invalid path type specifier")));
	
	
	unsigned int lmdb_environment_flags = 0;
	unsigned int lmdb_database_flags = lmdb_multidata ? MDB_DUPSORT : 0;
	unsigned int lmdb_transaction_flags = 0;
	mdb_mode_t lmdb_environment_mode = 0640;
	
	MDB_env * lmdb_environment = NULL;
	MDB_txn * lmdb_transaction = NULL;
	MDB_dbi lmdb_database = -1;
	int mdb_outcome;
	jv outcome;
	
	mdb_outcome = mdb_env_create (&lmdb_environment);
	if (mdb_outcome != 0) {
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_resolve` failed:  error encountered while creating environment  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	mdb_outcome = mdb_env_set_mapsize (lmdb_environment, lmdb_environment_size);
	if (mdb_outcome != 0) {
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_resolve` failed:  error encountered while initializing environment  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	mdb_outcome = mdb_env_set_maxreaders (lmdb_environment, lmdb_environment_readers);
	if (mdb_outcome != 0) {
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_resolve` failed:  error encountered while initializing environment  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	mdb_outcome = mdb_env_open (lmdb_environment, lmdb_path + 1, lmdb_environment_flags, lmdb_environment_mode);
	if (mdb_outcome != 0) {
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_resolve` failed:  error encountered while opening environment  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	mdb_outcome = mdb_txn_begin (lmdb_environment, NULL, lmdb_transaction_flags, &lmdb_transaction);
	if (mdb_outcome != 0) {
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_resolve` failed:  error encountered while beginning transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	mdb_outcome = mdb_dbi_open (lmdb_transaction, NULL, lmdb_database_flags, &lmdb_database);
	if (mdb_outcome != 0) {
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_resolve` failed:  error encountered while opening database  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	mdb_outcome = mdb_txn_commit (lmdb_transaction);
	if (mdb_outcome != 0) {
		outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_lmdb_resolve` failed:  error encountered while committing transaction  (LMDB: `%d` / `%s`)", mdb_outcome, mdb_strerror (mdb_outcome)));
		goto exit;
	}
	
	
	jqe_lmdb lmdb;
	lmdb.path = lmdb_path;
	lmdb.environment = lmdb_environment;
	lmdb.database = lmdb_database;
	lmdb.multidata = lmdb_multidata;
	
	lmdb_path = NULL;
	lmdb_environment = NULL;
	lmdb_transaction = NULL;
	lmdb_database = -1;
	
	jqe_lmdb_stack[jqe_lmdb_stack_size] = lmdb;
	*lmdb_ = &jqe_lmdb_stack[jqe_lmdb_stack_size];
	jqe_lmdb_stack_size++;
	
	outcome = jv_true ();
	
	
exit :
	
	
	if (lmdb_database != -1)
		mdb_dbi_close (lmdb_environment, lmdb_database);
	if (lmdb_transaction != NULL)
		mdb_txn_abort (lmdb_transaction);
	if (lmdb_environment != NULL)
		mdb_env_close (lmdb_environment);
	if (lmdb_path != NULL)
		free (lmdb_path);
	
	
	return (outcome);
}




static jv jqe_lmdb_data_input (MDB_val * data, jv value, bool encode)
{
	value = jve_string_input (value, encode);
	
	if (!jv_is_valid (value))
		return (value);
	
	char const * string_data = jv_string_value (jv_copy (value));
	size_t string_size = jv_string_length_bytes (jv_copy (value));
	
	data->mv_data = (void *) string_data;
	data->mv_size = string_size;
	
	return (value);
}


static jv jqe_lmdb_data_output (MDB_val * data, bool decode)
{
	jv value;
	
	char const * string_data = (char const *) data->mv_data;
	size_t string_size = data->mv_size;
	
	value = jv_string_sized (string_data, string_size);
	
	value = jve_string_output (value, decode);
	
	return (value);
}
