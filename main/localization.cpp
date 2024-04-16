#include <string.h>
#include <stdlib.h>
#include "pserror.h"
#include "CFILE.H"
#include "stringtable.h"

struct {
	const char *tag;
	int len;
} Language_tags[] { { "!=!", -1 }, { "!G!", -1 }, { "!S!", -1 }, { "!I!", -1 }, { "!F!", -1 }};
int Num_languages = sizeof(Language_tags) / sizeof(Language_tags[0]);
int Localization_language;

int _parse_line_information(char *line)
{
	for (int i = 0; i < Num_languages; i++) {
		if (Language_tags[i].len == -1)
			Language_tags[i].len = strlen(Language_tags[i].tag);
		if (strncmp(line, Language_tags[i].tag, Language_tags[i].len) == 0)
			return i;
	}
	if (strncmp(line, "!/!", strlen("!/!")) == 0)
		return -3;
	return *line ? -1 : -2;
}

void FixFilenameCase(const char *filename, char *result) {
	if (strlen(filename) >= PSPATHNAME_LEN)
		abort();
	strcpy(result, filename);
}

bool CreateStringTable(const char *filename,char ***table,int *size) {
	char fixed_filename[PSPATHNAME_LEN];
	char line[1024 + 1];
	int line_count, cur;
	char **lines;
	CFILE *f;

	if (table)
		*table = NULL;
	if (size)
		*size = 0;
	if (!filename)
		return false;
	FixFilenameCase(filename, fixed_filename);

	for (;;) {
		if (!(f = cfopen(filename, "rt")))
			return false;
		line_count = 0;
		while (!cfeof(f)) {
			cf_ReadString(line, sizeof(line), f);
			if (_parse_line_information(line) == Localization_language)
				line_count++;
		}
		if (line_count)
			break;
		if (!Localization_language)
			return false;
		Localization_language = 0;
	}
	lines = (char **)malloc(line_count * 4);
	if (!(f = cfopen(filename, "rt"))) {
		free(lines);
		return false;
	}
	cur = 0;
	while (!cfeof(f) && cur < line_count) {
		cf_ReadString(line, sizeof(line), f);
		if (_parse_line_information(line) == Localization_language) {
			char *p = line + 3, *po;
			if (!(po = lines[cur++] = (char *)malloc(strlen(line) + 1)))
				abort();
			for (char c, *pn;;) {
				if (!(pn = strchr(p, '\\')))
					pn = p + strlen(p);
				memcpy(po, p, pn - p);
				po += pn - p;
				p = pn;
				if (!(c = *p++))
					break;
				c = *p++;
				if (c == 'n')
					c = '\n';
				else if (c == 'r')
					c = '\r';
				else if (c == 't')
					c = '\t';
				else if (c >= '0' && c <= '9') {
					pn = p - 1;
					c = 0;
					while (pn < p + 2 && *pn >= '0' && *pn <= '9')
						c = c * 10 + (*pn++ - '0');
					p = pn;
				}
				*po++ = c;
			}
			*po = 0;
		}
	}
	cfclose(f);
	*table = lines;
	*size = line_count;
	return true;
}

void DestroyStringTable(char **table,int size)
{
	for (int i = 0; i < size; i++)
		free(table[i]);
	free(table);

}

void Localization_SetLanguage(int lang)
{
	Localization_language = lang;
}

int Localization_GetLanguage(void)
{
	return Localization_language;
}

char **String_table;
int String_table_count;

int LoadStringTables() {
	CreateStringTable("D3.STR", &String_table, &String_table_count);
	return String_table_count;
}

char *GetStringFromTable(int index)
{
	if (String_table && index >= 0 && index < String_table_count)
		return String_table[index];
	static char errstr[] = "No String table!";
	return errstr;
}

void InitStringTable() {
	Localization_SetLanguage(0);
	if (!LoadStringTables())
		Error("Couldn't find the string table.");
}
