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

// info
PSP_MODULE_INFO("DayViewerForGame", 0x1007, 0, 6);

// global
u32 impose_text_addr;
u32 ver_addr;
u32 stat_addr;
int time_count;
void *vram_top;
pspTime g_time;
SceSysTimerId timer;

// extern
extern u16 gray_bg;
extern DrawBuffer draw_buf;
extern Config conf;

// function_ptr
void (*ImposeDrawVolumebar)(void);
void (*ImposeClearDisplay)(int clear_mode_flag);

// struct
PatchAddress patch_addr[] =
{
	{ 0x658, 0x6350, 0x4AC8, 0x3CF4, 0x694C, 0x6980, 0xA060, 432, 392 }, //FW6.20
	{ 0x658, 0x64E8, 0x4B74, 0x3DA0, 0x6AE4, 0x6B18, 0xA230, 440, 400 }, //FW6.3X
	{ 0x5D0, 0x6470, 0x4AEC, 0x3D18, 0x6A6C, 0x6AA0, 0xA1D0, 440, 400 }, //FW6.60
};

PatchAddress patch_addr_go[] =
{
	{ 0x6DC, 0x7D70, 0x5DB0, 0x4D40, 0x8464, 0x8498, 0xC1D0, 444, 404 }, //FW6.20
	{ 0x750, 0x7F98, 0x5EEC, 0x4E68, 0x868C, 0x86C0, 0xC440, 452, 412 }, //FW6.3X
	{ 0x6C8, 0x7E60, 0x5E64, 0x4DE0, 0x8554, 0x8588, 0xC2D0, 452, 412 }, //FW6.60
};

// functions
void ClearCaches(void)
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int GetPatchAddr(void)
{
	int res, devkit, version;

	// バージョンを取得する - trans. Obtain version
	devkit = sceKernelDevkitVersion();
	version = (((devkit >> 24) & 0xF) << 8) | (((devkit >> 16) & 0xF) << 4) | ((devkit >> 8) & 0xF);

	switch(version)
	{
		case 0x620:
			res = 0;
			break;

		case 0x635:
		case 0x637:
		case 0x638:
		case 0x639:
			res = 1;
			break;

		case 0x660:
			res = 2;
			break;

		default:
			res = -1;
			break;
	}

	return res;
}

void InitGraphic(void)
{
	u16 *vram16;

	draw_buf.width = 480;
	draw_buf.height = 272;
	draw_buf.bufferwidth = 512;
	draw_buf.pixelformat = PSP_DISPLAY_PIXEL_FORMAT_5551;
	draw_buf.vram = vram_top;

	if(gray_bg == 0)
	{
		vram16 = (u16 *)vram_top;
		gray_bg = vram16[0];
	}

	return;
}

// HOME画面なら1を返す、それ以外は0を返す - Return 1 if screen, otherwise return 0
int ImposeGetHomeStatus(void)
{
	return _lw(_lw(impose_text_addr + ver_addr) + stat_addr);
}

void division(int m, int n, int *q, int *r)
{
	*q = 0;
	*r = 0;

	if(m < n)
		return;

	while(m >= n)
	{
		m -= n;
		*q += 1;
	}

	*r = m;
	return;
}

void ConvertString(pspTime *time, char *buf, const char *str, int *params)
{
	char bath_degit[32], batm_degit[32];
	int i, week_pos, last_year, last_month, last_day;
	int hour, min, sec, life, per, bath, batm;
	int *arg[13];

	// 初期化 - Initialize
	week_pos = 0;
	last_year = 0;
	last_month = 0;
	last_day = 0;
	hour = 0;

	// バッテリー関連の情報を取得する - Acquire battery related information
	per = scePowerGetBatteryLifePercent();
	life = scePowerGetBatteryLifeTime();

	hour = time->hour;
	min = time->minutes;
	sec = time->seconds;

	// 曜日の取得 - Acquiring the day of the week
	if(time->year != last_year || time->month != last_month || time->day != last_day)
	{
		last_year = time->year;
		last_month = time->month;
		last_day = time->day;

		week_pos = sceRtcGetDayOfWeek(time->year, time->month, time->day);

		// 日曜日が基準なので-1して合わせる - Since Sunday is the standard, set -1 and match
		week_pos--;
		if(week_pos < 0) week_pos = 6;
	}

	// バッテリーの残り時間を取得する- Get the remaining time of the battery
	division(life, 60, &bath, &batm);

	if(bath < 0 || bath > 59)
		bath = 60;

	if(batm < 0 || batm > 59)
		batm = 60;

	for(i = 0; i < 13; i++)
		arg[i] = NULL;

	// int型のポインタに文字列や数値を全て割り振られた番号順に格納する
	for(i = 0; i < 13; i++)
	{
		if(params[i] == 13)
			break;

		switch(params[i])
		{
			case 0:
				arg[i] = (int *)conf.week_txt[week_pos];
				break;
			case 1:
				arg[i] = (int *)last_day;
				break;
			case 2:
				arg[i] = (int *)conf.month_txt[time->month - 1];
				break;
			case 3:
				arg[i] = (int *)last_month;
				break;
			case 4:
				arg[i] = (int *)last_year;
				break;
			case 5:
				arg[i] = (int *)(hour > 11 ? hour - 12 : hour);
				break;
			case 6:
				arg[i] = (int *)hour;
				break;
			case 7:
				arg[i] = (int *)min;
				break;
			case 8:
				arg[i] = (int *)sec;
				break;
			case 9:
				arg[i] = (int *)(hour < 12 ? "AM" : "PM");
				break;
			case 10:
				arg[i] = (int *)per;
				break;
			case 11:
				if(bath != 60)
				{
					sprintf(bath_degit, "%02u", bath);
					arg[i] = (int *)bath_degit;
				}
				else
				{
					arg[i] = (int *)"--";
				}
				break;
			case 12:
				if(batm != 60)
				{
					sprintf(batm_degit, "%02u", batm);
					arg[i] = (int *)batm_degit;
				}
				else
				{
					arg[i] = (int *)"--";
				}
				break;
		}
	}

	// 文字列を一括変換する
	sprintf(buf, str, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9], arg[10], arg[11], arg[12]);
	return;
}

int sceRtcGetCurrentClockLocalTimePatched(pspTime *time)
{
	// 時間を取得する
	int res = sceRtcGetCurrentClockLocalTime(time);

	// microseconds以外のメンバが変化した場合に文字列を生成する
	if(memcmp(&time, &g_time, 12) != 0)
		ConvertString(time, conf.string, conf.format, conf.params);

	// バックアップ
	memcpy(&g_time, &time, sizeof(pspTime));

	return res;
}

int TickHandler(void)
{
	time_count++;

	if(time_count > 59)
	{
		// 文字列を表示する
		if(ImposeGetHomeStatus() == ENABLE_HOME_SCREEN)
		{
			DrawString(conf.x_pos, conf.y_pos, 0xC0FFFFFF, 0xFF000000, conf.flag, conf.string);
		}

		time_count = 0;
	}

	return -1;
}

int HideVolumebarThread(SceSize args, void *argp)
{
	int hide_flag;
	clock_t clock;
	SceCtrlData pad;

	// 初期化
	hide_flag = 0;
	clock = 0;

	// ループ
	while(1)
	{
		if(ImposeGetHomeStatus() == ENABLE_HOME_SCREEN)
		{
			// ボタンを取得する
			sceCtrlPeekBufferPositive(&pad, 1);

			// 3秒後の時間を取得する
			if(pad.Buttons & (PSP_CTRL_VOLUP | PSP_CTRL_VOLDOWN))
			{
				hide_flag = 1;
				clock = sceKernelLibcClock() + 3000000;
			}
			else if(clock < sceKernelLibcClock() && hide_flag == 1)
			{
				hide_flag = 2;
			}

			// 音量バーの表示 & 非表示
			if(hide_flag == 1)
			{
				ImposeDrawVolumebar();
			}
			else if(hide_flag == 2)
			{
				ImposeClearDisplay(CLEAR_VOLUME_BAR);
				hide_flag = 0;
			}
		}

		sceKernelDelayThread(50000);
	}

	return 0;
}

int PatchImposeDriver(void)
{
	SceUID thid;
	SceModule2 *mod;
	u32 text_addr, model, patch_fw, clear_func, draw_func, vol_addr, clock_addr, icon_addr, blit_addr, vram_addr;

	// モジュールを検索
	mod = sceKernelFindModuleByName("sceImpose_Driver");

	if(!mod)
		return -1;

	// text_addrを取得する
	text_addr = mod->text_addr;
	impose_text_addr = mod->text_addr;

	// indexの取得
	patch_fw = GetPatchAddr();

	if(patch_fw < 0)
		return -1;

	// PSPのモデルを取得する
	model = sceKernelGetModel();

	if(model != 4)
	{
		clear_func = patch_addr[patch_fw].clear_func;
		draw_func = patch_addr[patch_fw].draw_func;
		vol_addr = patch_addr[patch_fw].vol_addr;
		clock_addr = patch_addr[patch_fw].clock_addr;
		icon_addr = patch_addr[patch_fw].icon_addr;
		blit_addr = patch_addr[patch_fw].blit_addr;
		ver_addr = patch_addr[patch_fw].ver_addr;
		vram_addr = patch_addr[patch_fw].vram_addr;
		stat_addr = patch_addr[patch_fw].stat_addr;
	}
	else
	{
		clear_func = patch_addr_go[patch_fw].clear_func;
		draw_func = patch_addr_go[patch_fw].draw_func;
		vol_addr = patch_addr_go[patch_fw].vol_addr;
		clock_addr = patch_addr_go[patch_fw].clock_addr;
		icon_addr = patch_addr_go[patch_fw].icon_addr;
		blit_addr = patch_addr_go[patch_fw].blit_addr;
		ver_addr = patch_addr_go[patch_fw].ver_addr;
		vram_addr = patch_addr_go[patch_fw].vram_addr;
		stat_addr = patch_addr_go[patch_fw].stat_addr;
	}

	if(conf.hide_volume_bar == 1 && model != 4)
	{
		// patch ImposeDrawVolumebar(void)
		_sw(0, text_addr + vol_addr);

		// 関数を取得する
		ImposeClearDisplay = (void *)text_addr + clear_func;
		ImposeDrawVolumebar = (void *)text_addr + draw_func;

		// スレッド
		thid = sceKernelCreateThread("HideVolumebarThread", HideVolumebarThread, 16, 0x1000, 0, NULL);

		if(thid >= 0)
			sceKernelStartThread(thid, 0, NULL);
	}

	// patch ImposeClearDisplay(int clear_mode_flag)
	// andi $v1, $a0, 0x1 -> lui $v1, 0x0
	_sw(0x3C030000, text_addr + clear_func + 4);

	// patch sceRtcGetCurrentClockLocalTime(pspTime *time)
	MAKE_CALL(text_addr + clock_addr, sceRtcGetCurrentClockLocalTimePatched);

	// patch ImposeBlitPrintf(pspTime *time, int x, int y, int arg3)
	_sw(0, text_addr + blit_addr);

	// patch ImposeDrawBatteryIcon(void)
	if(conf.hide_battery == 1)
	{
		_sw(0, text_addr + icon_addr);
	}

	// HOME画面のVRAMのアドレスを取得する
	vram_top = (void *)_lw(_lw(text_addr + ver_addr) + vram_addr);

	// 初期化
	InitGraphic();

	// キャッシュクリア
	ClearCaches();
	return 0;
}

int module_start(SceSize args, void *argp)
{
	int i;
	char path[256], *ptr;

	if(sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_GAME)
	{
		// 初期化
		gray_bg = 0;
		time_count = 59;
		memset(&g_time, 0, sizeof(pspTime));
		memset(&draw_buf, 0, sizeof(DrawBuffer));

		// 設定ファイルへのパスを取得する
		strcpy(path, (char *)argp);
		ptr = strrchr(path, '/');

		if(!ptr)
			return -1;

		strcpy(ptr + 1, "dayviewer_game_conf.txt");

		// 設定を取得する
		if(DayViewerForGameLoadConfig(path) < 0)
			return -1;

		// パッチ
		if(PatchImposeDriver() < 0)
		{
			// メモリ開放
			free_p(conf.format);
			free_p(conf.string);

			for(i = 0; i < 7; i++)
				free_p(conf.week_txt[i]);

			for(i = 0; i < 12; i++)
				free_p(conf.month_txt[i]);

			return -1;
		}

		// タイマーをセットする
		timer = sceSTimerAlloc();
		sceSTimerStartCount(timer);
		sceSTimerSetHandler(timer, 799999, TickHandler, 0);
	}

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	sceSTimerStopCount(timer);
	sceSTimerFree(timer);
	return 0;
}

