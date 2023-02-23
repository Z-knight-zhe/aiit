#include "encoder_util.h"

#ifdef RUN_ARGS
#include <vector>
#include <string.h>
void split_str(char *input_str, std::vector<const char *> &args)
{
	size_t len = strlen(input_str);
	const char delimit[] = {' ', '\t'};
	bool last_flag = false;

	for (u32 i = 0; i < len; i++) {
		bool flag = true;
		for (u32 j = 0; j < sizeof(delimit); j++) {
			if (input_str[i] == delimit[j]) {
				input_str[i] = '\0';
				flag = false;
				break;
			}
		}
		if ((last_flag == false) && (flag == true)) {
			args.push_back(&input_str[i]);
		}
		last_flag = flag;
	}
}
#endif //RUN_ARGS

int main(int argc, const char **argv)
{
	Multislice enc_top;

#ifdef RUN_ARGS
	printf("RUN_ARGS:%s\n", RUN_ARGS);
	char *input_str = strdup(RUN_ARGS);
	assert(input_str != NULL);
	std::vector<const char *> args;
	args.push_back(argv[0]);
	split_str(input_str, args);
	enc_top.run(args.size(), args.data());
	free(input_str);
#else
	enc_top.run(argc, argv);
#endif //RUN_ARGS
    return 0;
}
