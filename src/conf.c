/*
	DayViewerForGame v7
	Copyright (C) 2012, plum

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// header
#include "dayviewer.h"

// global
Config conf;

// struct
ConvertTypes types[13] =
{
	{ "%weekday", "%s", 0 },
	{ "%day", "%02u", 1 },
	{ "%monthname", "%s", 2 },
	{ "%month", "%02u", 3 },
	{ "%year", "%04u", 4 },
	{ "%hour12", "%02u", 5 },
	{ "%hour", "%02u", 6 },
	{ "%min", "%02u", 7 },
	{ "%sec", "%02u", 8 },
	{ "%ampm", "%s", 9 },
	{ "%batpercent", "%02u", 10 },
	{ "%batlifehour", "%s", 11 },
	{ "%batlifemin", "%s", 12 },
};

// function
void *malloc_p(SceSize size)
{
	SceUID memid;
	void *ptr;

	memid = sceKernelAllocPartitionMemory(1, "block", 1, size + 4, NULL);

	if(memid < 0)
		return NULL;

	ptr = sceKernelGetBlockHeadAddr(memid);

	memcpy(ptr, &memid, 4);
	return ptr + 4;
}

int free_p(void *ptr)
{
	SceUID memid;

	if(!ptr)
		return -1;

	memcpy(&memid, ptr - 4, 4);
	return sceKernelFreePartitionMemory(memid);
}

int case_cmpn_string(const char *dest, const char *src, int n)
{
	int i;
	u8 s1, s2;

	if(!dest || !src)
		return 1;

	i = 0;
	s1 = 0;
	s2 = 0;

	do
	{
		if(i++ >= n)
			break;

		s1 = *dest++;
		s2 = *src++;

		if(s1 >= 'A' && s1 <= 'Z')
			s1 += 0x20;

		if(s2 >= 'A' && s2 <= 'Z')
			s2 += 0x20;
	}
	while(s1 == s2 && s1 != '\0');

	return s1 - s2;
}

int ReadLine(SceUID fd, char *buf, int n)
{
	char ch;
	int res, len;

	n -= 1;
	len = 0;

	do
	{
		res = sceIoRead(fd, &ch, 1);

		if(res == 1)
		{
			if(ch == '\r' || ch == '\n')
				break;

			else
			{
				buf[len++] = ch;
			}
		}
	}
	while(res == 1 && len < n);

	buf[len] = '\0';

	if(res == 1 && len == 0)
		len = 1;

	return len;
}

void ConvertParams(char *buf, const char *str, int *params)
{
	int i, n, txt_len, conv_len;

	if(!buf || !str)
		return;

	for(i = 0; i < 13; i++)
		params[i] = 13;

	n = 0;

	while(*str != '\0')
	{
		if(*str == '%')
		{
			if(str[1] == '%')
			{
				*buf++ = '%';
				*buf++ = '%';
				str += 2;
			}
			else
			{
				for(i = 0; i < 13; i++)
				{
					txt_len = strlen(types[i].txt);
					conv_len = strlen(types[i].conv);

					if(case_cmpn_string(str, types[i].txt, txt_len) == 0)
					{
						strcpy(buf, types[i].conv);

						str += txt_len;
						buf += conv_len;

						params[n++] = types[i].number;
						break;
					}
				}
			}
		}
		else
		{
			*buf++ = *str++;
		}
	}

	*buf = '\0';
	return;
}

int DayViewerForGameLoadConfig(const char *path)
{
	SceUID fd;
	char *buf, *ptr;
	int i, res, size, len;

	fd = sceIoOpen(path, PSP_O_RDONLY, 0);

	if(fd < 0)
		return -1;

	size = sceIoLseek(fd, 0, PSP_SEEK_END);
	sceIoLseek(fd, 0, PSP_SEEK_SET);

	buf = malloc_p(size + 1);

	if(!buf)
	{
		sceIoClose(fd);
		return -1;
	}

	do
	{
		res = ReadLine(fd, buf, size);

		if(buf[0] != '#' && buf[0] != '\0')
		{
			if(case_cmpn_string(buf, "[format] = ", 11) == 0)
			{
				ptr = (char *)buf + 11;
				len = strlen(ptr);

				conf.format = malloc_p(len + 1);
				conf.string = malloc_p(len + 1);

				if(conf.format != NULL && conf.string != NULL)
					ConvertParams(conf.format, ptr, conf.params);
			}
			else if(case_cmpn_string(buf, "[set_standard_pos] = ", 21) == 0)
			{
				ptr = (char *)buf + 21;
				conf.set_pos = (*ptr == '1') ? 1 : 0;
			}
			else if(case_cmpn_string(buf, "[x_pos] = ", 10) == 0)
			{
				ptr = (char *)buf + 10;
				conf.x_pos = atoi(ptr);
			}
			else if(case_cmpn_string(buf, "[y_pos] = ", 10) == 0)
			{
				ptr = (char *)buf + 10;
				conf.y_pos = atoi(ptr);
			}
			else if(case_cmpn_string(buf, "[alignment] = ", 14) == 0)
			{
				ptr = (char *)buf + 14;

				switch(*ptr)
				{
					case '2':
						conf.flag = FONT_CENTER;
						break;

					case '1':
						conf.flag = FONT_RIGHT;
						break;

					case '0':
					default:
						conf.flag = FONT_NORMAL;
						break;
				}
			}
			else if(case_cmpn_string(buf, "[hide_battery_icon] = ", 22) == 0)
			{
				ptr = (char *)buf + 22;
				conf.hide_battery = (*ptr == '1') ? 1 : 0;
			}
			else if(case_cmpn_string(buf, "[hide_volume_bar] = ", 20) == 0)
			{
				ptr = (char *)buf + 20;
				conf.hide_volume_bar = (*ptr == '1') ? 1 : 0;
			}
			else if(case_cmpn_string(buf, "[week]", 6) == 0)
			{
				for(i = 0; i < 7; i++)
				{
					len = ReadLine(fd, buf, size);

					if(buf[0] == '\0' && len == 1)
						len = ReadLine(fd, buf, size);

					conf.week_txt[i] = malloc_p(len + 1);

					if(conf.week_txt[i] != NULL)
						strcpy(conf.week_txt[i], buf);
				}
			}
			else if(case_cmpn_string(buf, "[month]", 7) == 0)
			{
				for(i = 0; i < 12; i++)
				{
					len = ReadLine(fd, buf, size);

					if(buf[0] == '\0' && len == 1)
						len = ReadLine(fd, buf, size);

					conf.month_txt[i] = malloc_p(len + 1);

					if(conf.month_txt[i] != NULL)
						strcpy(conf.month_txt[i], buf);
				}
			}
		}
	}
	while(res != 0);

	if(conf.set_pos == 1)
	{
		conf.x_pos = 0;
		conf.y_pos = 15;
		conf.flag = FONT_RIGHT;
		conf.hide_battery = 0;
	}

	sceIoClose(fd);
	free_p(buf);

	return 0;
}

