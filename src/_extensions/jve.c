



#include "jve.h"




jv jve_string_input (jv value, bool encode)
{
	if (encode) {
		return (jv_dump_string (value, JV_PRINT_ASCII | JV_PRINT_SORTED));
	} else if (jv_get_kind (value) == JV_KIND_STRING) {
		return (value);
	} else {
		jv_free (value);
		return (jv_invalid_with_msg (jv_string ("`jve_string_input` expected a string")));
	}
}


jv jve_string_output (jv value, bool decode)
{
	if (decode) {
		if (jv_get_kind (value) == JV_KIND_STRING)
			return (jv_parse (jv_string_value (value)));
		else {
			jv_free (value);
			return (jv_invalid_with_msg (jv_string ("`jve_string_output` expected a string")));
		}
	} else if (jv_get_kind (value) == JV_KIND_STRING) {
		return (value);
	} else {
		jv_free (value);
		return (jv_invalid_with_msg (jv_string ("`jve_string_output` expected a string")));
	}
}
