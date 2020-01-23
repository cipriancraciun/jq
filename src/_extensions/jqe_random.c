



#include "jve.h"
#include "jqe_random.h"

#include "_external/mt19937ar.h"

#include <time.h>




static void jqe_random_initialize (void);




static jv jqe_random_0 (unsigned int bits);

jv jqe_random_8 (jq_state * jq, jv input)
{
	jv_free (input);
	
	return (jqe_random_0 (8));
}

jv jqe_random_16 (jq_state * jq, jv input)
{
	jv_free (input);
	
	return (jqe_random_0 (16));
}

jv jqe_random_32 (jq_state * jq, jv input)
{
	jv_free (input);
	
	return (jqe_random_0 (32));
}

static jv jqe_random_0 (unsigned int bits)
{
	union { uint8_t uint8[4]; uint16_t uint16[2]; uint32_t uint32[1]; } random_data;
	memset (&random_data, 0, sizeof (random_data));
	
	jqe_random_initialize ();
	
	random_data.uint32[0] = genrand_int32 ();
	
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
			return (jv_invalid_with_msg (jv_string ("`jqe_random` failed:  assertion encountered")));
	}
	
	return (jv_number (random_value));
}




jv jqe_random_real (jq_state * jq, jv input)
{
	jv_free (input);
	
	jqe_random_initialize ();
	
	double random_value = genrand_real1 ();
	
	return (jv_number (random_value));
}




static void jqe_random_initialize (void)
{
	static bool initialized = false;
	if (!initialized) {
		init_genrand (time (NULL));
		initialized = true;
	}
}
