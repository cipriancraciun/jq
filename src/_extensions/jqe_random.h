#ifndef JQE_RANDOM_H
#define JQE_RANDOM_H


#include "../jq.h"
#include "../jv.h"


extern jv jqe_random_8 (jq_state * jq, jv input);
extern jv jqe_random_16 (jq_state * jq, jv input);
extern jv jqe_random_32 (jq_state * jq, jv input);

extern jv jqe_random_real (jq_state * jq, jv input);


#endif
