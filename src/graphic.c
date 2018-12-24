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
#include "font.h"

// global
u16 gray_bg;
DrawBuffer draw_buf;

// extern
extern Config conf;

// function
static __inline u32 ConvertColor_8888_5551(u16 col16)
{
	return ((col16 & 0x1F) << 3) | (((col16 >> 5) & 0x1F) << 11) | (((col16 >> 10) & 0x1F) << 19);
}

static __inline u16 ConvertColor_5551_8888(u32 color)
{
	return (R(color) >> 3) | ((G(color) >> 3) << 5) | ((B(color) >> 3) << 10);
}

static __inline u32 ConvertAlpha(u8 a, u32 color1, u32 color2)
{
	return (u32)((u8)(R(color1) * a / 255.0f + R(color2) * (255 - a) / 255.0f) | ((u8)(G(color1) * a / 255.0f + G(color2) * (255 - a) / 255.0f) << 8) | ((u8)(B(color1) * a / 255.0f + B(color2) * (255 - a) / 255.0f) << 16));
}

static __inline void PutPixel(int x, int y, u32 color)
{
	u32 col32;
	u16 *vram16;

	vram16 = (u16 *)draw_buf.vram + x + (y << 9);
	col32 = ConvertAlpha(A(color), color, ConvertColor_8888_5551(vram16[0]));
	vram16[0] = ConvertColor_5551_8888(col32);

	return;
}

void DrawLine(int x, int y, int len)
{
	int i, n, v;
	u16 *vram16;

	v = (conf.set_pos == 1) ? 444 : len;
	vram16 = (u16 *)draw_buf.vram + (y << 9);

	for(i = 0; i < 11; i++, vram16 += 512)
	{
		for(n = x; n < v; n++)
		{
			vram16[n] = gray_bg;
		}
	}

	return;
}

void DrawChar(int x, int y, u32 fg, u32 bg, int shadow, char cht)
{
	u16 *fontp;
	u8 ch, g[2], gray;
	int sx, sy, wid, i, j, c, d;

	if(cht < 0 || cht > 127)
		return;

	ch = cht - 0x20;
	fontp = (u16 *)fontp8;

	wid = font[fontp[ch] + 1] / 2 + ((font[fontp[ch] + 1] % 2) ? 1 : 0);

	for(i = 0; i < font[fontp[ch]]; i++)
	{
		for(j = 0; j < font[fontp[ch] + 1]; j += 2)
		{
			gray = font[fontp[ch] + 4 + i * wid + j / 2];

			g[0] = (gray & 0xF0) >> 4;
			g[1] = (gray & 0x0F);

			for(c = 0; c < 2 && (j + c) < font[fontp[ch] + 1]; c++)
			{
				if(g[c] > 0)
				{
					sx = x + font[fontp[ch] + 3] + j + c;
					sy = y - font[fontp[ch] + 2] + i;
					PutPixel(sx, sy, (((u8)(g[c] * 17) * A(fg) / 255) << 24) | BGR(fg));

					for(d = 3; d > 0 && shadow; d--)
					{
						sx++;
						sy++;
						PutPixel(sx, sy, (((u8)(g[c] * d) * A(bg) / 255) << 24) | BGR(bg));
					}
				}
			}
		}
	}

	return;
}

static __inline int count_str(const char *str, int back, int size)
{
	int len;
	for(len = 0; *str != '\\' && *str != '\0' && (back - len) > 8 && len < 472; str++, len += size);
	return len;
}

int DrawString(int x, int y, u32 fg, u32 bg, u16 flag, const char *str)
{
	char *ptr;
	int i, sx, count, ctr, back, shadow;

	ctr = (flag & FONT_CENTER) ? 240 : 472;
	back = (conf.set_pos == 1) ? 440 : ctr;
	sx = 0;
	count = 0;

	if(flag & FONT_CENTER)
	{
		x = back - count_str(str, back, 4);

		for(ptr = (char *)str, count = x; *ptr != '\\' && *ptr != '\0' && count <= 472; ptr++, count += 8);
		if(count > 472) count = 472;
	}
	else if(flag & FONT_RIGHT)
	{
		x = back - count_str(str, back, 8);

		for(ptr = (char *)str, count = x; *ptr != '\\' && *ptr != '\0' && count <= 472; ptr++, count += 8);
		if(count > 472) count = 472;
	}
	else
	{
		sx = x;

		for(ptr = (char *)str, count = x; *ptr != '\\' && *ptr != '\0' && count <= 472; ptr++, count += 8);
		if(count > 472) count = 472;
	}

	shadow = (flag & FONT_SHADOW) ? 1 : 0;

	DrawLine(x, y - 8, count);

	for(i = 0; str[i] != '\0'; i++, x += 8)
	{
		if(str[i] == '\\')
		{
			do
			{
				i++;
				y += 12;

				if(flag & FONT_CENTER)
				{
					x = back - count_str(str + i, back, 4);

					for(ptr = (char *)str + i, count = x; *ptr != '\\' && *ptr != '\0' && count <= 472; ptr++, count += 8);
					if(count > 472) count = 472;
				}
				else if(flag & FONT_RIGHT)
				{
					x = back - count_str(str + i, back, 8);

					for(ptr = (char *)str + i, count = x; *ptr != '\\' && *ptr != '\0' && count <= 472; ptr++, count += 8);
					if(count > 472) count = 472;
				}
				else
				{
					x = sx;

					for(ptr = (char *)str + i, count = x; *ptr != '\\' && *ptr != '\0' && count <= 472; ptr++, count += 8);
					if(count > 472) count = 472;
				}

				if(str[i] != '\\')
					DrawLine(x, y - 8, count);
			}
			while(str[i] == '\\');
		}
		else if(x >= 472)
		{
			y += 12;

			if(flag & FONT_CENTER)
			{
				x = back - count_str(str + i, back, 4);

				for(ptr = (char *)str + i, count = x; *ptr != '\\' && *ptr != '\0' && count <= 472; ptr++, count += 8);
				if(count > 472) count = 472;
			}
			else if(flag & FONT_RIGHT)
			{
				x = back - count_str(str + i, back, 8);

				for(ptr = (char *)str + i, count = x; *ptr != '\\' && *ptr != '\0' && count <= 472; ptr++, count += 8);
				if(count > 472) count = 472;
			}
			else
			{
				x = sx;

				for(ptr = (char *)str + i, count = x; *ptr != '\\' && *ptr != '\0' && count <= 472; ptr++, count += 8);
				if(count > 472) count = 472;
			}

			DrawLine(x, y - 8, count);
		}

		if(y >= draw_buf.height)
			break;

		if(flag & FONT_THICK)
		{
			DrawChar(x, y, fg, bg, 0, str[i]);
			DrawChar(++x, y, fg, bg, shadow, str[i]);
		}
		else
		{
			DrawChar(x, y, fg, bg, shadow, str[i]);
		}
	}

	return i;
}

