#include "contiki.h"

#include "node-id.h"
#include "scopes-types.h"
#include "scopes-membership.h"

static void
init(void)
{
  return;
}

static uint8_t
check(uint8_t *specs, uint8_t num)
{
	uint8_t i;
	for (i = 0; i < num; i++) {
		if (specs[i] == node_id)
			return 1;
	}
	return 0;
}

MEMBERSHIP(simple_membership, "simple membership", init, check);
