



#include "jve.h"
#include "jqe_crypto.h"

#include <string.h>

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/rand.h>




typedef unsigned char * (* jqe_crypto_hasher) (unsigned char const *, size_t, unsigned char *);
static jv jqe_crypto_hash (jq_state * jq, jqe_crypto_hasher hasher, size_t hash_size, jv input, jv encode);

static jv jqe_crypto_hexstring (void const * data, size_t size);




jv jqe_crypto_md5 (jq_state * jq, jv input)
{
	return (jqe_crypto_md5_ll (jq, input, jv_true ()));
}

jv jqe_crypto_md5_ll (jq_state * jq, jv input, jv encode)
{
	return (jqe_crypto_hash (jq, MD5, MD5_DIGEST_LENGTH, input, encode));
}


jv jqe_crypto_sha1 (jq_state * jq, jv input)
{
	return (jqe_crypto_sha1_ll (jq, input, jv_true ()));
}

jv jqe_crypto_sha1_ll (jq_state * jq, jv input, jv encode)
{
	return (jqe_crypto_hash (jq, SHA1, SHA_DIGEST_LENGTH, input, encode));
}


jv jqe_crypto_sha224 (jq_state * jq, jv input)
{
	return (jqe_crypto_sha224_ll (jq, input, jv_true ()));
}

jv jqe_crypto_sha224_ll (jq_state * jq, jv input, jv encode)
{
	return (jqe_crypto_hash (jq, SHA224, SHA224_DIGEST_LENGTH, input, encode));
}


jv jqe_crypto_sha256 (jq_state * jq, jv input)
{
	return (jqe_crypto_sha256_ll (jq, input, jv_true ()));
}

jv jqe_crypto_sha256_ll (jq_state * jq, jv input, jv encode)
{
	return (jqe_crypto_hash (jq, SHA256, SHA256_DIGEST_LENGTH, input, encode));
}


jv jqe_crypto_sha384 (jq_state * jq, jv input)
{
	return (jqe_crypto_sha384_ll (jq, input, jv_true ()));
}

jv jqe_crypto_sha384_ll (jq_state * jq, jv input, jv encode)
{
	return (jqe_crypto_hash (jq, SHA384, SHA384_DIGEST_LENGTH, input, encode));
}


jv jqe_crypto_sha512 (jq_state * jq, jv input)
{
	return (jqe_crypto_sha512_ll (jq, input, jv_true ()));
}

jv jqe_crypto_sha512_ll (jq_state * jq, jv input, jv encode)
{
	return (jqe_crypto_hash (jq, SHA512, SHA512_DIGEST_LENGTH, input, encode));
}




jv jqe_crypto_hash (jq_state * jq, jqe_crypto_hasher hasher, size_t hash_size, jv input, jv encode)
{
	input = jve_string_input_1 (input, encode);
	if (!jv_is_valid (input))
		return (input);
	
	char const * string_data = jv_string_value (jv_copy (input));
	size_t string_size = jv_string_length_bytes (jv_copy (input));
	
	unsigned char hash_data[hash_size];
	memset (hash_data, 0, hash_size);
	
	hasher ((unsigned char const *) string_data, string_size, hash_data);
	
	jv_free (input);
	
	return (jqe_crypto_hexstring (hash_data, hash_size));
}




jv jqe_crypto_random (jq_state * jq, jv input)
{
	if (!jv_is_integer (input)) {
		jv_free (input);
		return (jv_invalid_with_msg (jv_string ("`jqe_crypto_random` expected integer input")));
	}
	
	int random_size = (int) jv_number_value (input);
	
	if ((random_size < 0) || (random_size > 1024))
		return (jv_invalid_with_msg (jv_string ("`jqe_crypto_random` invalid input (between 0 and 1024)")));
	
	unsigned char random_data[random_size];
	memset (random_data, 0, random_size);
	
	if (!RAND_bytes (random_data, random_size))
		return (jv_invalid_with_msg (jv_string ("`jqe_crypto_random` failed:  error encountered while generating random data")));
	
	return (jqe_crypto_hexstring (random_data, random_size));
}




static jv jqe_crypto_hexstring (void const * data, size_t size)
{
	static char digits[] = "0123456789abcdef";
	
	char text[size * 2 + 1];
	
	for (size_t i = 0; i < size; i++) {
		text[i * 2 + 0] = digits[(((unsigned char const *) data)[i] & 0xf0) >> 4];
		text[i * 2 + 1] = digits[(((unsigned char const *) data)[i] & 0x0f) >> 0];
	}
	
	return (jv_string_sized (text, size * 2));
}
