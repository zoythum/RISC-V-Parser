#include "rvp-utility-json.h"

//Arguments: input file, output file
int main(int argc, char* argv[]) {
	FILE* in;
	FILE* out;

	line_encaps parsed_src;

	if(argc < 2)
		return 1;

	in = fopen(argv[1], "r");
	out = fopen(argv[2], "w");

	export_to_json(*parse(in), out);

	fclose(in);
	fclose(out);

	return 0;
}
