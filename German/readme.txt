[DayViewerForGame v7 von plum]

Dieses Plugin erlaubt dir die Uhrzeitzeile zu bearbeiten und fügt weitere Feature wie Wochentag, Monatsname, Jahr, Batterie Prozent Anzeige, Restliche Batteriedauer und vieles mehr hinzu.


Dieses Plugin unterstützt nur ASCII zeichen.


Ich bin ab PSP CFW 6.XX verwendbar.



[Changelog v7]
- Das Anzeigen von Zeichen, in Bezug auf den vorherigen Fehler, wurde korrigiert.

[Changelog v6]
- Es wurde der Fehler behoben, bei denen das Batterie Symbol nicht korrekt angezeigt wurde, wenn dort Zeichen angezeigt wurden.

[Changelog v5]
- Als Charakterschnur von einem Bildschirm herausragte, begann ich eine neue Linie.
- Ich verwandelte es aus [align_right] in [alignment].

[Changelog v4]
- Weitere Anzeigefehler behoben.
- Ein Problem mit [set_standard_pos] und dem Batterie Symbol behoben. 

[Changelog v3]
- Einen (weiteren) Absatzfehler behoben.
- Einen [hide_battery_icon] Fehler behoben.
- Eine [hide_volume_bar] Funktion zur Konfigurationsdatei hinzugefugt.

[Changelog v2]
- Einen Absatzfehler behoben.
- Eine [set_standard_pos] und [hide_battery_icon] Funktion zur Konfigurationsdatei hinzugefugt.


[Instructions / Anleitung]
- Kopiere den 'seplugins' Ornder auf deinen Memory Stick und fur diese Zeile in der 'game.txt' hinzu:
'ms0:/seplugins/dayviewer_game.prx 1' (Ohne die ' Symbole).
- Nun wirf einen Blick auf die 'dayviewer_game_conf.txt' und bearbeite sie nach belieben.
- Starte die PSP neu und erfreue dich am Plugin :).


[Editing clock string / Bearbeitung der Uhrzeitinformationen]

Fur das Andern der Zeile mit den Uhrzeitinformationen, kann dieses genutzt werden:

%weekday
%day
%monthname
%month
%year
%hour12 (Zeitformat 12h)
%hour (Zeitformat 24h)
%min
%sec
%ampm
%batpercent
%batlifehour (Batterie Restdauer Stunde)
%batlifemin (Batterie Restdauer Minute)

Mochtest du das %-Symbol anzeigen musst du dies schreiben: '%%'.
Um einen neuen Absatz zu beginnen musst du dies schreiben: '\'.


[Thanks / Danksagungen]
- Danke an Total_Noob.
- Danke an Wildcard team.
- Danke junbo969 & estuibal & Bright_Net & mak1341 fur das Testen der Betaversion.

