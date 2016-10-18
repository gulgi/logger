#include "logging.h"

int main()
{
	Logging log;
	log.log("test");
	log.log("test 2");
	log.log("test 3");
	log.log("test ", 6.1f);
	log.log("test ", 4, "\\", 5);
}