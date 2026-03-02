#include "vassert.h"
int main()
{
	// VASSERT(9 + 10 == 21);
	VASSERT_WARN(3 == 0);
	VASSERT_WARN_MSG(3 == 0, "what");
	VASSERT_MSG(9 + 10 == 21, "Hi %i", 3);
}
