



#include "jve.h"




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
