#include "tpe.h"

struct tpe_token {
	char **tokens;
	int count;
};

static struct tpe_token *tpe_tokenize_string(const char *str, size_t len)
{
	return NULL;
}

int tpe_conf_process_line(const char *str, size_t len)
{
	return 0;
}

int tpe_init_conf()
{
	return 0;
}

void tpe_deinit_conf()
{
	return;
}
