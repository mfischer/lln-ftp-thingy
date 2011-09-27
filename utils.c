#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void
exit_error (const char *err_msg)
{
	printf ("Error: %s\n", err_msg);
	exit (EXIT_FAILURE);
}

