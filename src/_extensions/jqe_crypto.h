#ifndef JQE_CRYPTO_H
#define JQE_CRYPTO_H


#include "../jq.h"
#include "../jv.h"


extern jv jqe_crypto_md5 (jq_state * jq, jv input);
extern jv jqe_crypto_md5_ll (jq_state * jq, jv input, jv encode);

extern jv jqe_crypto_sha1 (jq_state * jq, jv input);
extern jv jqe_crypto_sha1_ll (jq_state * jq, jv input, jv encode);

extern jv jqe_crypto_sha224 (jq_state * jq, jv input);
extern jv jqe_crypto_sha224_ll (jq_state * jq, jv input, jv encode);

extern jv jqe_crypto_sha256 (jq_state * jq, jv input);
extern jv jqe_crypto_sha256_ll (jq_state * jq, jv input, jv encode);

extern jv jqe_crypto_sha384 (jq_state * jq, jv input);
extern jv jqe_crypto_sha384_ll (jq_state * jq, jv input, jv encode);

extern jv jqe_crypto_sha512 (jq_state * jq, jv input);
extern jv jqe_crypto_sha512_ll (jq_state * jq, jv input, jv encode);


extern jv jqe_crypto_random (jq_state * jq, jv input);


#endif
