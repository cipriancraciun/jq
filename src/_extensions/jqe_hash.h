#ifndef JQE_HASH_H
#define JQE_HASH_H


#include "../jq.h"
#include "../jv.h"


extern jv jqe_hash_8 (jq_state * jq, jv input);
extern jv jqe_hash_16 (jq_state * jq, jv input);
extern jv jqe_hash_32 (jq_state * jq, jv input);
extern jv jqe_hash_64 (jq_state * jq, jv input);
extern jv jqe_hash_128 (jq_state * jq, jv input);

extern jv jqe_hash_bucket_index (jq_state * jq, jv input, jv count);
extern jv jqe_hash_bucket_contains (jq_state * jq, jv input, jv count, jv index);


#endif
