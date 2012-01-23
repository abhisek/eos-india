#include "tpe.h"

#define TPE_MAX_TOKENS	10

struct tpe_token_set {
	char *tokens[TPE_MAX_TOKENS];
	int count;
};

static struct tpe_token *tpe_tokenize_string(const char *str, size_t len)
{
	struct tpe_token_set *token_set;
	int token_count;
	size_t tlen;
	char *kptr;
	char *kend;

	token_set = kzalloc(sizeof(struct tpe_token_set), GFP_KERNEL);
	if(unlikely(token_set == NULL)) {
		TPE_DEBUG("OOM");
		return NULL;
	}

	kptr = kend = str;
	tlen = len
	i = 0
	while(*kptr && tlen && (token_set->count < TPE_MAX_TOKENS)) {
		if(*kend == ' ') {
			int l = kptr - kend;
			token_set->tokens[i] = kzalloc(l + 1, GFP_KERNEL);
			strncpy(token_set->tokens[i], kptr, l);
			kptr = kend;
			i++;
			token_set->count++;
		}
		kend++;
		tlen--;
	}

	return token_set;
}

static void tpe_free_token_set(struct tpe_token *tk)
{
	int i;
	
	if(unlikely(tk == NULL))
		return;

	for(i = 0; i < tk->count; i++) {
		if(unlikely(tk->tokens[i] != NULL))
			kfree(tk->tokens[i]);
	}
	
	kfree(tk);
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
