#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include "jq.h"
#include <stdbool.h>




extern jv jqe_hello (jq_state *jq, jv input);




typedef struct jqe_lmdb jqe_lmdb;


extern jv jqe_lmdb_select (jq_state * jq, jv input, jv path, jv value_default);
extern jv jqe_lmdb_select_ll (jq_state * jq, jv input, jv path, jv key, jv value_default, jv decode);

extern jv jqe_lmdb_update (jq_state * jq, jv input, jv path, jv key);
extern jv jqe_lmdb_update_ll (jq_state * jq, jv input, jv path, jv key, jv value, jv encode);

extern jv jqe_lmdb_include (jq_state * jq, jv input, jv path, jv key);
extern jv jqe_lmdb_include_ll (jq_state * jq, jv input, jv path, jv key, jv value, jv encode);

extern jv jqe_lmdb_exclude (jq_state * jq, jv input, jv path);
extern jv jqe_lmdb_exclude_ll (jq_state * jq, jv input, jv path, jv key, jv encode);

extern jv jqe_lmdb_keys (jq_state * jq, jv input, jv path);
extern jv jqe_lmdb_keys_ll (jq_state * jq, jv input, jv path, jv encode);


extern jv jqe_lmdb_load_one_0 (jqe_lmdb * lmdb, jv key, jv value_default, bool decode);
extern jv jqe_lmdb_load_all_0 (jqe_lmdb * lmdb, jv key, jv value_default, bool decode);
extern jv jqe_lmdb_store_0 (jqe_lmdb * lmdb, jv key, jv value, bool update, bool encode);
extern jv jqe_lmdb_delete_0 (jqe_lmdb * lmdb, jv key, bool encode);
extern jv jqe_lmdb_keys_0 (jqe_lmdb * lmdb, bool encode);


extern jv jqe_lmdb_resolve (jq_state * jq, jv path, jqe_lmdb * * lmdb);




extern jv jve_string_input (jv value, bool encode);
extern jv jve_string_output (jv value, bool decode);




#endif
