#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifndef PATH_MAX
// My system defines PATH_MAX in linux/limits.h
#define PATH_MAX 4096
#endif

typedef struct FHeader
{
	char magic[4];
	int32_t partnum;
} FHeader;

typedef struct FSection
{
	int32_t offset;
	int32_t size;
} FSection;

int min(int a, int b)
{
	return a < b ? a : b;
}

void my_sendfile(FILE* in, FILE* out, int size)
{
	char buf[4096];
	while (size >= sizeof(buf))
	{
		fread(buf, 1, sizeof(buf), in);
		fwrite(buf, 1, sizeof(buf), out);
		size -= sizeof(buf);
	}
	fread(buf, 1, size, in);
	fwrite(buf, 1, size, out);
}

int main(int argc, char** argv)
{
	if (argc != 3) return 1;
	int n = atoi(argv[2]);

	FILE *in = fopen(argv[1], "rb");

	FHeader h1;
	fread(&h1, sizeof(FHeader), 1, in);
	memcpy(&h1.magic, "CMC\x7f", 4);

	FSection *sections = malloc(sizeof(FSection) * h1.partnum);
	FSection *newsect = malloc(sizeof(FSection) * h1.partnum);
	fseek(in, -h1.partnum * sizeof(FSection), SEEK_END);
	fread(sections, sizeof(FSection), h1.partnum, in);

	int cnt = h1.partnum;
	int sectidx = 0;
	for (int i = 0; i < min(n, cnt); ++i)
	{
		char file[PATH_MAX];
		snprintf(file, sizeof(file), "%s-%d", argv[1], i + 1);
		FILE *out = fopen(file, "wb");
		h1.partnum = cnt / n + (i < cnt % n);
		fwrite(&h1, sizeof(FHeader), 1, out);
		for (int j = 0; j < cnt / n + (i < cnt % n); ++j)
		{
			newsect[j].offset = ftell(out);
			newsect[j].size = sections[sectidx].size;
			fseek(in, sections[sectidx].offset, SEEK_SET);
			my_sendfile(in, out, sections[sectidx].size);
			++sectidx;
		}
		for (int j = 0; j < cnt / n + (i < cnt % n); ++j)
		{
			fwrite(newsect + j, sizeof(FSection), 1, out);
		}
		fclose(out);
	}
	fclose(in);
	free(newsect);
	free(sections);
}
