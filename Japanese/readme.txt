[DayViewerForGame v7	by plum]

[プラグインについて]
{
	Total_Noob氏の[DayViewerプラグイン]を参考にHOME画面用のDayViewerなるものを作ってみました。

	しかし、名前にDayViewerとかついてますがフォントの関係でASCIIの文字しか表示できません。

	こんな貧弱なプラグインですがご容赦ください。

	※ HOME画面のフォントはXMBと違い、全て画像ファイルになっており、内蔵されているのが数字と簡単な記号だけだったので
		やむなくそれっぽいフォントを使うことにしました
}

[更新内容]
{
	・v7
	{
		文字の表示が正常に行われていなかったバグを修正した
	}

	・v6
	{
		文字を表示すると、バッテリーアイコンの表示がおかしくなっていたバグを修正した
	}

	・v5
	{
		文字列が画面からはみ出した場合に改行するようにした
		[align_right] -> [alignment]へ変更した
	}

	・v4
	{
		文字列が正しく表示されないバグを修正した
		[set_standard_pos]を有効にしていた場合にバッテリーアイコンが非表示になっていた問題を修正した
	}

	・v3
	{
		連続で改行した場合に正しく文字が表示されないバグを修正
		設定ファイルの[hide_battery_icon]のコメントを修正した
		設定ファイルに[hide_volume_bar]を追加した
	}

	・v2
	{
		文字列を右寄りに指定している時に3行以上の改行がズレていたバグを修正
		設定ファイルに[set_standard_pos]と[hide_battery_icon]を追加した
	}

	・v1
	{
		公開
	}
}

[インストール]
{
	MSにsepluginsフォルダをコピーしてプラグインを有効にしてください。
}

[設定ファイルについて]
{
	・設定ファイルのパス
	{
		設定ファイルはプラグインと同じフォルダに配置してください。
	}

	・パラメータ一覧
	{
		%weekday		dayviewer_game_conf.txtに記述した曜日を表示します。
		%monthname		dayviewer_game_conf.txtに記述した月を表示します。
		%year			現在の年を表示します。
		%month			現在の月を表示します。
		%day			現在の日を表示します。
		%hour			現在の時間を24時間表示で表示します。
		%hour12			現在の時間を12時間表示で表示します。
		%min			現在の分を表示します。
		%sec			現在の秒を表示します。
		%ampm			午前だと"AM"、午後だと"PM"と表示します。
		%batpercent		バッテリーが残り何%なのかを表示します。
		%batlifehour	バッテリーが残り何時間なのかを表示します。
		%batlifemin		バッテリーが残り何分なのかを表示します。
	}
}

[対応FW]
{
	CFW 6.XX
}

[Thanks]
{
	Thanks to Total_Noob.
	Thanks to Wildcard team.
	Thanks to junbo969 & estuibal & Bright_Net & mak1341 for betatesting.
}

