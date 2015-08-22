# base_entranced

by duo and exar

a fork of sil's fantastic [base_enhanced](https://github.com/TheSil/base_enhanced) JK3 server mod

This mod is primarily for siege, and is based on base_enhanced with some extra features

#Features unique to base_entranced
####g_fixsiegescoring
0 = dumb default JK3 scoring (20 pts per obj, 30 pts for final obj, 10 pt bonus at end)

1 = improved scoring (100 pts per obj)

####g_fixfallingsounds
0 = default JK3 sound (normal death sound)

1 = falling death scream sound for all trigger_hurt entities (such as death pits in siege)

####g_doorgreening
0 = no door greening (ctf flag-in-door fix enabled)

1 = door greening (default JK3 setting, ctf flag-in-door fix disabled)

####g_nextmapwarning
0 = no warning (default JK3)

1 = when nextmap vote is called in round 2, a warning message appears (so you don't accidentally reset the timer going up when starting round 2)

####g_sexydisruptor
0 = lethal sniper shots with full charge (1.5 seconds or more) cause incineration effect (fixed default JK3 setting, which was bugged)

1 = all lethal sniper shots cause incineration effect

####Reset siege to round 1 on /map vote
No more changing maps with timer going down.

####Bugfixes:
* Seeker no longer attacks walkers and fighters.

#Features that are also in base_enhanced
Many of these features were coded or conceived by us first, and then were added to base_enhanced by Sil later.

####g_rocket_surfing
0 = no rocket surfing (good)

1 = bullshit rocket surfing enabled (JK3 default)

####g_floating_items
0 = no floating siege items - useful for non-korriban maps

1 = siege items float up walls when dropped - useful for korriban (JK3 default)

####g_selfkill_penalty
Set to 0 so you don't lose points when you SK.

####"Joined the red/blue team" message
See when someone joined a team in the center of your screen.

####Pause / unpause
Use command /pause or /unpause (also can be called as vote) to stop the game temporarily.

####whois
Use command /whois to see all known aliases of a player.

####Auto-click on death
If you die 1 second before the spawn, the game now automatically "clicks" on your behalf to make the respawn

####Random teams/capts
Use /randomteams 2 2 for random 2v2, etc. and /randomcapts for random captains.

####Specall
Use /specall to force all players to spec.

####Start round with one player
No longer need two players to start running around ingame.

####Better logs
Log detailed user info, rcon commands, and crash attempts.

####Awards/medals
Humiliation, impressive, etc.

####Bug fixes:
* When you run someone over in the ATST, you get a kill.
* No more spying on the enemy team during siege countdown.
* Bugfix for not scoring points on Hoth first obj.
* No more getting stuck because you(or walker) spawned in a shield.
* "Was blasted" message when you get splashed by turrets instead of generic "died" message.
* No more seeker shooting at specs, teammates, or disconnected clients.
* No more rancor spec bug or SKing after rancor grab.
* No more weird camera and seeker glitch from walker.
* Bugfix for renaming causing saber style change.
* Bugfix for disconnecting in ATST and going invisible.
* No more SK when already dead.
* Bugfix for sentry placed in lift.
* Allow for many more siege maps/classes on server.
* Security/crash fixes.
* Map_restart 99999999 fix.
* Spec voting fix.
* Flechette stuck in wall fix.
* Probably more fixes.


###[Click here to download latest version (PK3)](https://drive.google.com/file/d/0B-vLJdPP0Uo8M05SQnh6QzJKTVU/view?usp=sharing)
Version: base_entranced-8-21-2015-build11 (stable) - add g_rocket_surfing, g_floating_items, reset siege to round 1 on /map vote, fix seeker attacking walker, fix seeker attacking disconnected clients, fix vehicles spawning in shield, fix turret splash kill message



Old versions:

Version: base_entranced-8-21-2015-build10 (stable) [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8ajRsbkx5TkRsaE0/view?usp=sharing) - add g_nextmapwarning

Version: base_entranced-8-20-2015-build9 (stable) [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8aTJJM2hjbGMtbmc/view?usp=sharing) - use sil's siege pause fix

Version: base_entranced-8-20-2015-build8 (stable) [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8dHVMZHZQOHZjZ3M/view?usp=sharing) - fix rancor bug, use sil's atst code

Version: base-entranced-8-20-2015-build7 (stable) [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8bzMtYXExcVh5QnM/view?usp=sharing) - add g_doorgreening

Version: base-entranced-8-19-2015-build6 (stable) - [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8TU1zTFpmX2p4LTA/view?usp=sharing) - fix hoth first obj points

Version: base-entranced-8-19-2015-build5 (stable) - [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8dERzQzNSVV9LR1E/view?usp=sharing) - add g_fixfallingsounds

Version: base_entranced-8-19-2015-build4 (stable) - [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8aGwtRzhNSXZzaUU/view?usp=sharing) - fix countdown teamchat

Version: base_entranced-8-19-2015-build3 (stable) - [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8ZlBTc3dDcy1lajA/view?usp=sharing) - fix ATST kills

Version:  base_entranced-8-19-2015-build2 (stable) - [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8bUhfR3dBcWtOWXc/view?usp=sharing) - add g_sexydisruptor and g_fixsiegescoring
