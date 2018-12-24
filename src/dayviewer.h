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

#ifndef __DAYVIEWER_FOR_GAME_H__
#define __DAYVIEWER_FOR_GAME_H__

// header
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspsystimer.h>
#include <pspdisplay.h>
#include <psppower.h>
#include <pspctrl.h>
#include <pspinit.h>
#include <psprtc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// patch
#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a);

// define
#define A(color) (u8)(color >> 24 & 0xFF)
#define B(color) (u8)(color >> 16 & 0xFF)
#define G(color) (u8)(color >> 8 & 0xFF)
#define R(color) (u8)(color & 0xFF)
#define BGR(color) (color & 0x00FFFFFF)

// enum
enum FontMode
{
	FONT_NORMAL = 0x0000,
	FONT_SHADOW = 0x000F,
	FONT_THICK = 0x00F0,
	FONT_RIGHT = 0x0F00,
	FONT_CENTER = 0xF000,
};

enum ClearMode
{
	CLEAR_CLOCK_STRING = 0x01,
	CLEAR_WINDOW = 0x02,
	CLEAR_VOLUME_BAR = 0x04,
	CLEAR_BOTTOM_DIALOG = 0x08,
	CLEAR_CENTER_CURSOR = 0x20,
};

enum HomeStatus
{
	DISABLE_HOME_SCREEN = 0,
	ENABLE_HOME_SCREEN = 1,
};

// struct
typedef struct
{
	u32 clear_func;
	u32 draw_func;
	u32 vol_addr;
	u32 clock_addr;
	u32 icon_addr;
	u32 blit_addr;
	u32 ver_addr;
	u32 vram_addr;
	u32 stat_addr;
} PatchAddress;

typedef struct
{
	int width;
	int height;
	int bufferwidth;
	int pixelformat;
	void *vram;
} DrawBuffer;

typedef struct
{
	int set_pos;
	int x_pos;
	int y_pos;
	u16 flag;
	int hide_battery;
	int hide_volume_bar;
	int params[13];
	char *format;
	char *string;
	char *week_txt[7];
	char *month_txt[12];
} Config;

typedef struct
{
	char txt[20];
	char conv[5];
	int number;
} ConvertTypes;

// main.c
void ClearCaches(void);

// graphic.c
void DrawChar(int x, int y, u32 fg, u32 bg, int shadow, char cht);
int DrawString(int x, int y, u32 fg, u32 bg, u16 flag, const char *str);

// conf.c
void *malloc_p(SceSize size);
int free_p(void *ptr);
int DayViewerForGameLoadConfig(const char *path);

#endif

