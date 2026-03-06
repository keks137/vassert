// #define VNO_COLORS
// #define VNO_LOCATED_LOGS
#include "vassert.h"
int main()
{
	// VASSERT(9 + 10 == 21);
	if (!VALWAYS(9 + 10 == 21)) {
		printf("the hell?\n");
	}
	VASSERT_WARN(3 == 0);
	VASSERT_WARN_MSG(3 == 0, "what");
	VASSERT_MSG(9 + 10 == 21, "Hi %i", 3);
}
