



#include "jve.h"
#include "jqe_hash.h"




static jv jqe_hash_128_generate (jv value, unsigned int bits);
static jv jqe_hash_128_outcome (jve_hash_128_t const * hash, unsigned int bits);




jv jqe_hash_8 (jq_state * jq, jv input)
{
	return (jqe_hash_128_generate (input, 8));
}

jv jqe_hash_16 (jq_state * jq, jv input)
{
	return (jqe_hash_128_generate (input, 16));
}

jv jqe_hash_32 (jq_state * jq, jv input)
{
	return (jqe_hash_128_generate (input, 32));
}

jv jqe_hash_64 (jq_state * jq, jv input)
{
	return (jqe_hash_128_generate (input, 64));
}

jv jqe_hash_128 (jq_state * jq, jv input)
{
	return (jqe_hash_128_generate (input, 128));
}




jv jqe_hash_bucket_index (jq_state * jq, jv input, jv count_)
{
	if (!jv_is_integer (count_)) {
		jv_free (input);
		jv_free (count_);
		return (jv_invalid_with_msg (jv_string ("`jqe_hash_bucket_index` expected an integer `count`")));
	}
	int count = jv_number_value (count_);
	if (count <= 0) {
		jv_free (input);
		return (jv_invalid_with_msg (jv_string ("`jqe_hash_bucket_index` expected a positive `count`")));
	}
	
	jv outcome;
	jve_hash_128_t hash;
	
	outcome = jve_hash_128 (input, &hash);
	if (!jv_is_valid (outcome))
		return (outcome);
	
	uint32_t hash_uint = hash.uint32[0] ^ hash.uint32[1];
	uint32_t index = hash_uint % count;
	
	return (jv_number (index));
}


jv jqe_hash_bucket_contains (jq_state * jq, jv input, jv count_, jv index_)
{
	if (!jv_is_integer (count_)) {
		jv_free (input);
		jv_free (count_);
		jv_free (index_);
		return (jv_invalid_with_msg (jv_string ("`jqe_hash_bucket_contains` expected an integer `count`")));
	}
	if (!jv_is_integer (index_)) {
		jv_free (input);
		jv_free (count_);
		jv_free (index_);
		return (jv_invalid_with_msg (jv_string ("`jqe_hash_bucket_contains` expected an integer `index`")));
	}
	
	int count = jv_number_value (count_);
	int index = jv_number_value (index_);
	if (count <= 0) {
		jv_free (input);
		return (jv_invalid_with_msg (jv_string ("`jqe_hash_bucket_contains` expected a positive `count`")));
	}
	if (index <= 0) {
		jv_free (input);
		return (jv_invalid_with_msg (jv_string ("`jqe_hash_bucket_contains` expected a positive `index`")));
	}
	if (index >= count) {
		jv_free (input);
		return (jv_invalid_with_msg (jv_string ("`jqe_hash_bucket_contains` expected an `index` lesser than `count`")));
	}
	
	jv outcome;
	jve_hash_128_t hash;
	
	outcome = jve_hash_128 (input, &hash);
	if (!jv_is_valid (outcome))
		return (outcome);
	
	uint32_t hash_uint = hash.uint32[0] ^ hash.uint32[1];
	uint32_t index_actual = hash_uint % count;
	
	if (index_actual == index)
		return (jv_true ());
	else
		return (jv_false ());
}




jv jqe_hash_128_generate (jv value, unsigned int bits)
{
	jv outcome;
	jve_hash_128_t hash;
	
	outcome = jve_hash_128 (value, &hash);
	if (!jv_is_valid (outcome))
		return (outcome);
	
	return (jqe_hash_128_outcome (&hash, bits));
}


jv jqe_hash_128_outcome (jve_hash_128_t const * hash, unsigned int bits)
{
	jv outcome;
	
	switch (bits) {
		
		case 8 : {
			uint8_t value =
					hash->uint8[0] ^ hash->uint8[1] ^ hash->uint8[2] ^ hash->uint8[3] ^
					hash->uint8[4] ^ hash->uint8[5] ^ hash->uint8[6] ^ hash->uint8[7] ^
					hash->uint8[8] ^ hash->uint8[9] ^ hash->uint8[10] ^ hash->uint8[11] ^
					hash->uint8[12] ^ hash->uint8[13] ^ hash->uint8[14] ^ hash->uint8[15];
			outcome = jv_number (value);
			break;
		}
		
		case 16 : {
			uint16_t value =
					hash->uint16[0] ^ hash->uint16[1] ^ hash->uint16[2] ^ hash->uint16[3] ^
					hash->uint16[4] ^ hash->uint16[5] ^ hash->uint16[6] ^ hash->uint16[7];
			outcome = jv_number (value);
			break;
		}
		
		case 32 : {
			uint32_t value =
					hash->uint32[0] ^ hash->uint32[1] ^ hash->uint32[2] ^ hash->uint32[3];
			outcome = jv_number (value);
			break;
		}
		
		/*
		case 64 : {
			uint64_t value =
					hash->uint64[0] ^ hash->uint64[1];
			outcome = jv_number (value);
			break;
		}
		*/
		
		case 64 :
		case 128 : {
			outcome = jve_string_hex (hash, sizeof (jve_hash_128_t));
			break;
		}
		
		default : {
			outcome = jv_invalid_with_msg (jv_string_fmt ("`jqe_hash` invalid bits `%d`", bits));
			break;
		}
	}
	
	return (outcome);
}
