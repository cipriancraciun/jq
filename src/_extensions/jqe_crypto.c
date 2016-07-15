



#include "jve.h"
#include "jqe_crypto.h"

#include <stdint.h>
#include <string.h>

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/rand.h>




typedef unsigned char * (* jqe_crypto_hasher) (unsigned char const *, size_t, unsigned char *);
static jv jqe_crypto_hash (jq_state * jq, jqe_crypto_hasher hasher, size_t hash_size, jv input, jv encode);




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
	
	return (jve_string_hex (hash_data, hash_size));
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
	
	return (jve_string_hex (random_data, random_size));
}




static jv jqe_crypto_random_0 (unsigned int bits);

jv jqe_crypto_random_8 (jq_state * jq, jv input)
{
	jv_free (input);
	
	return (jqe_crypto_random_0 (8));
}

jv jqe_crypto_random_16 (jq_state * jq, jv input)
{
	jv_free (input);
	
	return (jqe_crypto_random_0 (16));
}

jv jqe_crypto_random_32 (jq_state * jq, jv input)
{
	jv_free (input);
	
	return (jqe_crypto_random_0 (32));
}

static jv jqe_crypto_random_0 (unsigned int bits)
{
	union { uint8_t uint8[4]; uint16_t uint16[2]; uint32_t uint32[1]; } random_data;
	memset (&random_data, 0, sizeof (random_data));
	
	if (!RAND_bytes ((unsigned char *) &random_data, sizeof (random_data)))
		return (jv_invalid_with_msg (jv_string ("`jqe_crypto_random` failed:  error encountered while generating random data")));
	
	uint32_t random_value;
	
	switch (bits) {
		case 8 :
			random_value = random_data.uint8[0] ^ random_data.uint8[1] ^ random_data.uint8[2] ^ random_data.uint8[3];
			break;
		case 16 :
			random_value = random_data.uint16[0] ^ random_data.uint16[1];
			break;
		case 32 :
			random_value = random_data.uint32[0];
			break;
		default :
			return (jv_invalid_with_msg (jv_string ("`jqe_crypto_random` failed:  assertion encountered")));
	}
	
	return (jv_number (random_value));
}
