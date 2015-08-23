# base_entranced

Siege server mod for Jedi Knight 3

by duo and exar

a fork of sil's fantastic [base_enhanced](https://github.com/TheSil/base_enhanced) mod with some extra features for siege.

#Features unique to base_entranced
####/g_fixSiegeScoring
0 = dumb default JK3 scoring (20 pts per obj, 30 pts for final obj, 10 pt bonus at end)

1 = improved scoring (100 pts per obj)

####/g_fixFallingSounds
0 = default JK3 sound (normal death sound)

1 = falling death scream sound for all trigger_hurt entities (such as death pits in siege)

####/g_doorGreening
0 = no door greening, good for pub servers to prevent lamers (ctf flag-in-door fix enabled)

1 = door greening, good for pug servers (default JK3 setting, ctf flag-in-door fix disabled)

####/g_nextmapWarning
0 = no warning (default JK3)

1 = when nextmap vote is called in round 2, a warning message appears (so you don't accidentally reset the timer going up when starting round 2)

####/g_sexyDisruptor
0 = lethal sniper shots with full charge (1.5 seconds or more) cause incineration effect (fixed default JK3 setting, which was bugged)

1 = all lethal sniper shots cause incineration effect (this is just for fun/cool visuals)

####/siege_restart
rcon command that restarts the current map with siege timer going up from 00:00. Before this, there was no server command to reset siege to round 1, the only way was /callvote nextmap (lol)

####/forceround2 mm:ss
Restarts current map with siege timer going down from a specified time. For example, /forceround2 7:30 starts siege in round 2 with the timer going down from 7:30. Can be executed from rcon or callvote.

####Reset siege to round 1 on /map vote
No more changing maps with timer going down.

####Bugfixes:
* Seeker no longer attacks walkers and fighters.
* Fixed bug with nextmap failed vote causing siege to reset to round 1 anyway.

#Features that are also in base_enhanced
Many of these features were coded and/or conceived by us first, and then were added to base_enhanced by Sil later.

####/g_rocketSurfing
0 = no rocket surfing (good)

1 = bullshit rocket surfing enabled (JK3 default)

####/g_floatingItems
0 = no floating siege items - useful for non-korriban maps

1 = siege items float up walls when dropped - useful for korriban (JK3 default)

####/g_selfkillPenalty
Set to 0 so you don't lose points when you SK.

####"Joined the red/blue team" message
See when someone joined a team in the center of your screen in siege mode.

####/pause and /unpause
Use command /pause or /unpause (also can be called as vote) to stop the game temporarily.

####/whois
Use command /whois to see all known aliases of a player.

####Auto-click on death
If you die 1 second before the spawn, the game now automatically "clicks" on your behalf to make the respawn

####Random teams/capts
Use /randomteams 2 2 for random 2v2, etc. and /randomcapts for random captains.

####/specall
Use /specall to force all players to spec.

####Start round with one player
No longer need two players to start running around ingame in siege mode.

####Better logs
Log detailed user info, rcon commands, and crash attempts. Use g_hacklog filename, g_logclientinfo 1, and g_logrcon 1.

####Awards/medals support
Humiliation, impressive, etc. if you use the clientside mod SMod.

####/cointoss
Call /cointoss vote for random heads/tails result.

####Polls
Ask everyone a question with /callvote q blahblahblah

####HTTTP auto downloading
Set url with g_dlurl

####Quiet rcon
No more leaking stuff from rcon, use g_quietrcon

####Disallow some votes
Use g_allow_vote_kick, etc. to prevent voting some things.

####Bug fixes:
* When you run someone over in the ATST, you get a kill.
* No more spying on the enemy teamchat during siege countdown.
* Bugfix for not scoring points on Hoth first obj.
* No more getting stuck because you spawned in a shield.
* "Was blasted" message when you get splashed by turrets instead of generic "died" message.
* No more seeker/sentry shooting at specs, teammates, or disconnected clients.
* No more rancor spec bug or SKing after rancor grab.
* No more weird camera and seeker glitch from walker.
* Bugfix for renaming causing saber style change.
* Bugfix for camera getting stuck when incinerated by sniper shot.
* Bugfix for disconnecting in ATST and going invisible.
* No more SK when already dead.
* No more weird spec glitch with possessing someone else's body.
* Bugfix for sentry placed in lift.
* Allow for many more siege maps/classes on server.
* Security/crash fixes.
* Bugfix for map_restart 999999 (now maximum 10)
* Bugfix for spec voting.
* Bugfix for flechette stuck in wall.
* Probably more fixes.


###[Click here to download latest version (PK3)](https://drive.google.com/file/d/0B-vLJdPP0Uo8SG1RdlRQVkJmY0k/view?usp=sharing)
Version: base_entranced-8-23-2015-build15 (stable) - fix nextmap vote failed bug, add /siege_restart, add /forceround2, some misc. engine fixes from sil

Old versions:

Version: base_entranced-8-22-2015-build13 (stable) [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8VUxRdTlOcEt2Rkk/view?usp=sharing) - revert sil's broken shield code, add /g_rocketSurfing, /g_floatingItems, change /g_selfkill_penalty to g_selfkillPenalty (no underscores), reset siege to round 1 on /map vote, fix seeker attacking walker, fix seeker/sentry attacking disconnected clients, fix turret splash kill message, fix sniper shot incineration camera bug, cvar overhaul

Version: base_entranced-8-21-2015-build10 (stable) [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8ajRsbkx5TkRsaE0/view?usp=sharing) - add /g_nextmapwarning

Version: base_entranced-8-20-2015-build9 (stable) [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8aTJJM2hjbGMtbmc/view?usp=sharing) - use sil's siege /pause fix

Version: base_entranced-8-20-2015-build8 (stable) [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8dHVMZHZQOHZjZ3M/view?usp=sharing) - fix rancor bug, use sil's atst code

Version: base-entranced-8-20-2015-build7 (stable) [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8bzMtYXExcVh5QnM/view?usp=sharing) - add /g_doorgreening

Version: base-entranced-8-19-2015-build6 (stable) - [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8TU1zTFpmX2p4LTA/view?usp=sharing) - fix hoth first obj points

Version: base-entranced-8-19-2015-build5 (stable) - [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8dERzQzNSVV9LR1E/view?usp=sharing) - add /g_fixfallingsounds

Version: base_entranced-8-19-2015-build4 (stable) - [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8aGwtRzhNSXZzaUU/view?usp=sharing) - fix countdown teamchat

Version: base_entranced-8-19-2015-build3 (stable) - [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8ZlBTc3dDcy1lajA/view?usp=sharing) - fix ATST kills

Version:  base_entranced-8-19-2015-build2 (stable) - [[download]](https://drive.google.com/file/d/0B-vLJdPP0Uo8bUhfR3dBcWtOWXc/view?usp=sharing) - add /g_sexydisruptor and /g_fixsiegescoring
