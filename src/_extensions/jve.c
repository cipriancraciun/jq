



#include "jve.h"
#include "../jv_alloc.h"

#include "_external/MurmurHash3.h"

#include <stdlib.h>
#include <string.h>




jv jve_string_input (jv value, bool encode)
{
	if (encode) {
		return (jv_dump_string (value, JV_PRINT_ASCII | JV_PRINT_SORTED));
	} else if (jv_get_kind (value) == JV_KIND_STRING) {
		return (value);
	} else {
		jv_free (value);
		return (jv_invalid_with_msg (jv_string ("`jve_string_input` expected a string `value`")));
	}
}


jv jve_string_output (jv value, bool decode)
{
	if (decode) {
		if (jv_get_kind (value) == JV_KIND_STRING)
			return (jv_parse (jv_string_value (value)));
		else {
			jv_free (value);
			return (jv_invalid_with_msg (jv_string ("`jve_string_output` expected a string `value`")));
		}
	} else if (jv_get_kind (value) == JV_KIND_STRING) {
		return (value);
	} else {
		jv_free (value);
		return (jv_invalid_with_msg (jv_string ("`jve_string_output` expected a string `value`")));
	}
}




jv jve_string_input_1 (jv value, jv encode)
{
	if ((jv_get_kind (encode) != JV_KIND_TRUE) && (jv_get_kind (encode) != JV_KIND_FALSE)) {
		jv_free (value);
		jv_free (encode);
		return (jv_invalid_with_msg (jv_string ("`jve_string_input` expected a boolean `encode`")));
	}
	bool encode_value = jv_get_kind (encode) == JV_KIND_TRUE;
	jv_free (encode);
	return (jve_string_input (value, encode_value));
}


jv jve_string_output_1 (jv value, jv decode)
{
	if ((jv_get_kind (decode) != JV_KIND_TRUE) && (jv_get_kind (decode) != JV_KIND_FALSE)) {
		jv_free (value);
		jv_free (decode);
		return (jv_invalid_with_msg (jv_string ("`jve_string_output` expected a boolean `decode`")));
	}
	bool decode_value = jv_get_kind (decode) == JV_KIND_TRUE;
	jv_free (decode);
	return (jve_string_output (value, decode_value));
}




jv jve_string_hex (void const * data, size_t size)
{
	static char digits[] = "0123456789abcdef";
	
	char text[size * 2 + 1];
	
	for (size_t i = 0; i < size; i++) {
		text[i * 2 + 0] = digits[(((unsigned char const *) data)[i] & 0xf0) >> 4];
		text[i * 2 + 1] = digits[(((unsigned char const *) data)[i] & 0x0f) >> 0];
	}
	
	return (jv_string_sized (text, size * 2));
}




static jv jve_hash_128_update (jv value, jve_hash_128_t * hash);

jv jve_hash_128 (jv value, jve_hash_128_t * hash)
{
	memset (hash, 0, sizeof (jve_hash_128_t));
	
	return (jve_hash_128_update (value, hash));
}

static int jve_object_record_compare (void const * left_, void const * right_);

static jv jve_hash_128_update (jv value, jve_hash_128_t * hash)
{
	jv outcome;
	
	// fprintf (stderr, ">> %08x %08x %08x %08x\n", hash->uint32[0], hash->uint32[1], hash->uint32[2], hash->uint32[3]);
	
	switch (jv_get_kind (value)) {
		
		case JV_KIND_INVALID : {
			outcome = value;
			break;
		}
		
		case JV_KIND_NULL : {
			MurmurHash3_x64_128 ("N", 2, MurmurHash3_UPDATE, hash);
			outcome = jv_null ();
			break;
		}
		
		case JV_KIND_TRUE : {
			MurmurHash3_x64_128 ("T", 2, MurmurHash3_UPDATE, hash);
			outcome = jv_null ();
			break;
		}
		
		case JV_KIND_FALSE : {
			MurmurHash3_x64_128 ("F", 2, MurmurHash3_UPDATE, hash);
			outcome = jv_null ();
			break;
		}
		
		case JV_KIND_STRING : {
			char const * string_data = jv_string_value (jv_copy (value));
			int string_size = jv_string_length_bytes (jv_copy (value));
			MurmurHash3_x64_128 ("S", 2, MurmurHash3_UPDATE, hash);
			MurmurHash3_x64_128 (string_data, string_size, MurmurHash3_UPDATE, hash);
			outcome = jv_null ();
			break;
		}
		
		case JV_KIND_NUMBER : {
			double number_value = jv_number_value (jv_copy (value));
			MurmurHash3_x64_128 ("D", 2, MurmurHash3_UPDATE, hash);
			MurmurHash3_x64_128 (&number_value, sizeof (number_value), MurmurHash3_UPDATE, hash);
			outcome = jv_null ();
			break;
		}
		
		case JV_KIND_ARRAY : {
			int array_size = jv_array_length (jv_copy (value));
			MurmurHash3_x64_128 ("A", 2, MurmurHash3_UPDATE, hash);
			MurmurHash3_x64_128 (&array_size, sizeof (array_size), MurmurHash3_UPDATE, hash);
			outcome = jv_null ();
			for (int array_index = 0; array_index < array_size; array_index++) {
				jv array_value = jv_array_get (jv_copy (value), array_index);
				outcome = jve_hash_128_update (array_value, hash);
				if (!jv_is_valid (outcome))
					break;
			}
			break;
		}
		
		case JV_KIND_OBJECT : {
			int object_size = jv_object_length (jv_copy (value));
			MurmurHash3_x64_128 ("O", 2, MurmurHash3_UPDATE, hash);
			MurmurHash3_x64_128 (&object_size, sizeof (object_size), MurmurHash3_UPDATE, hash);
			outcome = jv_null ();
			struct object_record { jv key; jv value; };
			struct object_record * object_records = jv_mem_alloc (sizeof (struct object_record) * object_size);
			for (
					int object_index = 0, object_iterator = jv_object_iter (value);
					((object_index < object_size) && jv_object_iter_valid (value, object_iterator));
					object_index++, object_iterator = jv_object_iter_next (value, object_iterator)
			) {
				object_records[object_index].key = jv_object_iter_key (value, object_iterator);
				object_records[object_index].value = jv_object_iter_value (value, object_iterator);
				// fprintf (stderr, "++ `%s`\n", jv_string_value (jv_copy (object_records[object_index].key)));
			}
			qsort (object_records, object_size, sizeof (struct object_record), jve_object_record_compare);
			for (int object_index = 0; object_index < object_size; object_index++) {
				// fprintf (stderr, ".. `%s`\n", jv_string_value (jv_copy (object_records[object_index].key)));
				outcome = jve_hash_128_update (object_records[object_index].key, hash);
				object_records[object_index].key = jv_null ();
				if (!jv_is_valid (outcome))
					break;
				outcome = jve_hash_128_update (object_records[object_index].value, hash);
				object_records[object_index].value = jv_null ();
				if (!jv_is_valid (outcome))
					break;
			}
			for (int object_index = 0; object_index < object_size; object_index++) {
				jv_free (object_records[object_index].key);
				jv_free (object_records[object_index].value);
			}
			jv_mem_free (object_records);
			break;
		}
		
		default : {
			outcome = jv_invalid_with_msg (jv_string_fmt ("`jve_hash` unknown value kind `%d`", jv_get_kind (value)));
			break;
		}
	}
	
	jv_free (value);
	
	// fprintf (stderr, "<< %08x %08x %08x %08x\n", hash->uint32[0], hash->uint32[1], hash->uint32[2], hash->uint32[3]);
	
	return (outcome);
}

static int jve_object_record_compare (void const * left_, void const * right_)
{
	struct object_record { jv key; jv value; };
	struct object_record const * left = (struct object_record *) left_;
	struct object_record const * right = (struct object_record *) right_;
	int left_length = jv_string_length_bytes (jv_copy (left->key));
	int right_length = jv_string_length_bytes (jv_copy (right->key));
	int min_length = (left_length < right_length) ? left_length : right_length;
	int outcome = memcmp (
			jv_string_value (jv_copy (left->key)),
			jv_string_value (jv_copy (right->key)),
			min_length);
	if (outcome == 0)
		outcome = left_length - right_length;
	return (outcome);
}
