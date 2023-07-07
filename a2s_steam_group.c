#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ssq/a2s.h>

#define MAGIC_NUMBER 103582791429521408

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
		return 0;
	}

	const char *hostname = argv[1];
	uint16_t port = atoi(argv[2]);

	SSQ_SERVER *server = ssq_server_new(hostname, port);
	if (!ssq_server_eok(server))
	{
		fprintf(stderr, "ssq_server_new: %s: %s\n", hostname, ssq_server_emsg(server));
		return 0;
	}

	ssq_server_timeout(server, SSQ_TIMEOUT_RECV | SSQ_TIMEOUT_SEND, 10000);

	A2S_INFO *info = ssq_info(server);
	if (!ssq_server_eok(server))
	{
		fprintf(stderr, "ssq_info: %s\n", ssq_server_emsg(server));
		return 0;
	}

	uint16_t rule_count = 0;
	A2S_RULES *rules = ssq_rules(server, &rule_count);
	if (!ssq_server_eok(server))
	{
		fprintf(stderr, "ssq_rules: %s\n", ssq_server_emsg(server));
		return 0;
	}

	printf("------- %s (%s:%i) -------\n", info->name, hostname, port);
	
	int bFound = 0;
	for (uint16_t i = 0; i < rule_count; ++i)
	{
		if (strcmp(rules[i].name, "sv_steamgroup"))
			continue;

		bFound = 1;
		char* buffer = strdup(rules[i].value);
		char* sGroup = strtok(buffer, ",");
		while (sGroup)
		{
			long long iGroup32 = strtol(sGroup, NULL, 10);
			long long iGroup64 = MAGIC_NUMBER + iGroup32;
			printf("%-10lli | https://steamcommunity.com/gid/%lli\n", iGroup32, iGroup64);
			sGroup = strtok(NULL, ",");
		}
		free(buffer);
		break;
	}

	if (!bFound)
		printf("sv_steamgroup is not disclosed\n");

	ssq_info_free(info);
	ssq_rules_free(rules, rule_count);
	ssq_server_free(server);
	return 0;
}
