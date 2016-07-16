#ifndef JQE_LMDB_H
#define JQE_LMDB_H


#include "../jq.h"
#include "../jv.h"

#include <stdbool.h>


typedef struct jqe_lmdb jqe_lmdb;


extern jv jqe_lmdb_select (jq_state * jq, jv input, jv path, jv on_missing);
extern jv jqe_lmdb_select_ll (jq_state * jq, jv input, jv path, jv key, jv on_missing, jv decode);

extern jv jqe_lmdb_update (jq_state * jq, jv input, jv path, jv key);
extern jv jqe_lmdb_update_ll (jq_state * jq, jv input, jv path, jv key, jv value, jv encode);

extern jv jqe_lmdb_include (jq_state * jq, jv input, jv path, jv key, jv on_existing);
extern jv jqe_lmdb_include_ll (jq_state * jq, jv input, jv path, jv key, jv value, jv on_existing, jv encode);

extern jv jqe_lmdb_exclude (jq_state * jq, jv input, jv path, jv on_missing);
extern jv jqe_lmdb_exclude_ll (jq_state * jq, jv input, jv path, jv key, jv on_missing, jv encode);

extern jv jqe_lmdb_keys (jq_state * jq, jv input, jv path);
extern jv jqe_lmdb_keys_ll (jq_state * jq, jv input, jv path, jv encode);


extern jv jqe_lmdb_read_one_0 (jqe_lmdb * lmdb, jv key, jv on_missing, bool decode);
extern jv jqe_lmdb_read_all_0 (jqe_lmdb * lmdb, jv key, jv on_missing, bool decode);
extern jv jqe_lmdb_write_0 (jqe_lmdb * lmdb, jv key, jv value, bool update, jv on_success, jv on_missupdate, bool encode);
extern jv jqe_lmdb_delete_0 (jqe_lmdb * lmdb, jv key, jv on_success, jv on_missing, bool encode);
extern jv jqe_lmdb_keys_0 (jqe_lmdb * lmdb, bool encode);


extern jv jqe_lmdb_resolve (jq_state * jq, jv path, jqe_lmdb * * lmdb);


#endif
