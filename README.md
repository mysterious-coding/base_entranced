# base_entranced

Siege server mod for Jedi Knight 3

by duo and exar

a fork of sil's fantastic [base_enhanced](https://github.com/TheSil/base_enhanced) mod, with extra features and bugfixes for siege.

#base_entranced features
These are unique features for base_entranced.

####`/g_siegeStats`
0 = no stats (default JK3)

1 = prints times for each objective after completion; writes all obj times at end of round. Compares times and highlights faster time in green in round 2.

####`/g_endSiege`
0 = no tiebreaking rules (JK3 default)

1 = enforce traditional siege community tiebreaking rule. If round 1 offense got held for maximum time(20 minutes on most maps), game will end once round 2 offense has completed one more objective. Round 2 offense will be declared the winner of the match.

####`/g_fixSiegeScoring`
0 = dumb default JK3 scoring (20 pts per obj, 30 pts for final obj, 10 pt bonus at end)

1 = improved/logical scoring (100 pts per obj)

####`/g_fixVoiceChat`
0 = enemies can hear your voice chats and see icon over your head (default JK3)

1 = only teammates can hear your voice chats and see icon over your head

####`/iLikeToDoorSpam`
0 = door spam prohibited for blobs, golan balls, rockets, conc primaries, thermals, and bowcaster alternates within a limited distance of enemies in your FOV. Wait until door opens to fire (skilled players already do this). Does not apply if a walker or someone using protect or mindtrick is nearby. Warning: turning on this setting will cause terrible players to complain.

1 = door spam allowed, have fun immediately getting hit to 13hp because some shitty was raining blobs/golans/etc on the door before you entered (default JK3)

####`/iLikeToMineSpam`
0 = mine spam prohibited within a limited distance of enemies in your FOV. No throwing mines at incoming enemies anymore (skilled players already refrain from this). Does not apply if a walker or someone using protect or mindtrick is nearby. Warning: turning on this setting will cause terrible players to complain.

1 = mine spam allowed, have fun insta-dying because some shitty was holding down mouse2 with mines before you entered the door. bonus points for "was planting, bro" excuse (default JK3)

####`/g_autoKorribanSpam`
0 = spam-related cvars are unaffected by map

1 = `/iLikeToDoorSpam` and `/iLikeToMineSpam` automatically get set to 1 for Korriban, and automatically get set to 0 for all other maps

####`/g_fixHothBunkerLift`
0 = normal lift behavior for Hoth codes bunker lift (default JK3)

1 = Hoth codes bunker lift requires pressing `+use` button (prevents you from killing yourself way too easily on this dumb lift)

####`/g_fixHothDoorSounds`
0 = Hoth bunker doors at first objective are silent (bug from default JK3)

1 = Hoth bunker doors at first objective use standard door sounds

####`/g_antiHothHangarLiftLame`
0 = normal behavior for Hoth hangar lift (default JK3)

1 = defense tech uses a 2 second hack to call up the lift. Returns to normal behavior after the hangar objective is completed.

2 = any player on defense is prevented from calling up the lift if any player on offense is nearby ("nearby" is defined as between the boxes in the middle of the hangar and the lift). Returns to normal behavior after the hangar objective is completed.

3 = use both 1 and 2 methods.

####`/g_autoKorribanFloatingItems`
0 = `g_floatingItems` is unaffected by map change

1 = `g_floatingItems` automatically gets set to 1 for Korriban, and automatically gets set to 0 for all other maps

####`/g_korribanRedRocksReverse`
0 = Korriban red crystal room rocks can only be pulled up (default JK3)

1 = Korriban red crystal room rocks can be pushed back down

####`/g_nextmapWarning`
0 = no warning (default JK3)

1 = when nextmap vote is called in round 2, a warning message appears (so you don't accidentally reset the timer going up when starting round 2)

####`/g_fixFallingSounds`
0 = default JK3 sound (normal death sound)

1 = use falling death scream sound when damaged by a `trigger_hurt` entity for >= 100 damage (i.e., death pits). Also plays scream sound if selfkilling while affected by `/g_fixPitKills`

####`/g_fixEweb`
0 = default JK3 eweb behavior (huge annoying recoil, etc)

1 = remove eweb recoil, remove "unfolding" animation when pulling out eweb, make eweb crosshair start closer to normal crosshair

####`/g_hideSpecLocation`
0 = teamchat between spectators shows location (default JK3)

1 = remove useless location from teamchat between spectators, meaning you can now have a nice conversation in spec without annoying text like >Wrecked AT-AT< unnecessarily taking up space on every line

####`/g_denoteDead`
0 = normal teamchat when dead (default JK3)

1 = (DEAD) message in teamchats from dead teammates

####`/g_enableCloak`
0 = remove cloak from all siege classes (eliminates need for no-cloak PK3 patches)

1 = cloak enabled (default JK3)

####`/g_fixRancorCharge`
0 = default JK3 behavior - rancor can charge/jump through `BLOCKNPC` areas (e.g. desert arena door)

1 = rancor cannot charge/jump through `BLOCKNPC` areas

####`/g_infiniteCharge`
0 = no infinite charging bug with `+useforce`/`+button2` (bugfix from base_enhanced)

1 = infinite charging bug enabled (classic behavior, brought back by popular demand. hold `+useforce` or `+button2` to hold weapon charge indefinitely)

####`/g_fixGripKills`
0 = normal selfkilling while gripped (default JK3)

1 = selfkilling while gripped counts as a kill for the gripper. This prevents people from denying enemies' kills with selfkill (similar to `/g_fixPitKills` from base_enhanced)

####`/g_gripRefresh`
300 = default JK3 grip refresh (your velocity is updated every 300ms while being gripped, which equals 3.33Hz refresh rate)

Set for lower values to get smoother grip without lag (maybe 50, which equals 20Hz refresh rate)

####`/g_jk2SaberMoves`
0 = default JK3 saber special moves

1 = JK2-style special saber moves (yaw while doing saber moves, smaller jump height/distance for YDFA, blue lunge in mid-air, etc). Not perfect, but a decent simulation of JK2 saber combat.

Note that the game client does not currently predict yaw for these moves, so they will only be rendered in realtime as limited by server `/sv_fps` setting

####`/g_antiCallvoteTakeover`
0 = normal vote calling for `/map`, `/g_gametype`, `/pug`, `/pub`, and `/kick`/`/clientkick` votes (default JK3)

1 = calling a vote for `/map`, `/g_gametype`, `/pug`, `/pub`, or `/kick`/`/clientkick` when 6+ players are connected requires at least half of the server to be ingame. This prevents calling lame unpopular votes when most of the server is in spec unable to vote no.

####`/g_moreTaunts`
0 = default JK3 behavior for /taunt command

1 = `/taunt` command will randomly use taunt, gloat, or flourish animation.

2 = `/taunt` uses basejka behavior, but `/gloat`, `/flourish`, and `/bow` are enabled as separate commands

####`/g_tauntWhileMoving`
0 = no taunting while moving (default JK3)

1 = enable taunting while moving

####`/g_ammoCanisterSound`
0 = dispensing ammo cans is oddly silent (default JK3)

1 = dispensing an ammo can plays a small sound

####`/g_botJumping`
0 = bots jump around like crazy on maps without botroute support (default JK3)

1 = bots stay on the ground on maps without botroute support

####`/g_specAfterDeath`
0 = no going spec after death

1 = you can join spectators in the brief moment after you die (default JK3)

####`/g_sexyDisruptor`
0 = lethal sniper shots with full charge (1.5 seconds or more) cause incineration effect (fixed default JK3 setting, which was bugged)

1 = all lethal sniper shots cause incineration effect (this is just for fun/cool visuals and makes it like singeplayer)

####`/siege_restart`
rcon command that restarts the current map with siege timer going up from 00:00. Before this, there was no server command to reset siege to round 1, the only way was `/callvote nextmap` (lol)

####`/forceround2 mm:ss`
Restarts current map with siege timer going down from a specified time. For example, `/forceround2 7:30` starts siege in round 2 with the timer going down from 7:30. Can be executed from rcon or callvote.

####`/killturrets`
Removes all turrets from the map. Useful for capt duels. Can be executed from rcon or callvote.

####Custom team/class overrides
You can override the classes for any siege map. Use `/g_redTeam <teamName>` and `/g_blueTeam <teamName>`. For example, to use Korriban classes on any map, you could type `/g_redTeam Siege3_Jedi` and `/g_blueTeam Siege3_DarkJedi`.

To reset to base classes, use `0` or `none` as the argument.

This also works with votes; you can do `/callvote g_redTeam <teamName>`. Enable this vote with `/g_allow_vote_customTeams`.

A few important clientside bugs to be aware of:
* If custom teams/classes are in use, you cannot use the Join Menu to join that team. You must either use `/team r` or `/team b` (easiest method), autojoin, or use a CFG classbind.
* Ravensoft decided to combine force powers and items into one menu/cycle in JK3; however, if you have both items and force powers, it will only display the force powers. So for example if you are using Korriban classes on Hoth and want to place a shield as D tesh, you need to use a `/use_field` bind.
* If the server is using teams/class that you don't have at all (like completely new classes, or classes for a map you don't have), you will see people as using Kyle skin with no sounds and no class icons.

####`/g_autoResetCustomTeams`
0 = retain custom teams/classes between map change votes

1 = `/g_redTeam` and `/g_blueTeam` are automatically reset to normal classes when map is changed via `/callvote`

####`/g_requireMoreCustomTeamVotes`
0 = 51% yes votes required for all votes to pass (default JK3)

1 = custom team/class votes require 75% yes votes. This does not apply if the argument is `0` or `none` (resetting to normal classes)

####`/g_forceDTechItems`
This cvar helps custom team/class overrides by adding some extra weapons/items to the defense tech. Note: these do NOT apply to Korriban. The mod is hardcoded to ignore these values for Korriban. This cvar is only used when custom teams are in use, and does not affect any classes that already have demp/shield.

0 = no additional weapons/items

1 = only Hoth DTech gets demp only

2 = only Hoth DTech gets shield only

3 = only Hoth DTech gets demp and shield

4 = all DTechs get demp only

5 = all DTechs get shield only (default setting)

6 = all DTechs get demp and shield

7 = all DTechs get shield; only Hoth DTech gets demp also

####Weapon spawn preference
Clients can decide their own preference of which weapon they would like to be holding when they spawn. To define your own preference list, type into your client JA: `/setu prefer <15 letters>`
* `L` : Melee
* `S` : Saber
* `P` : Pistol
* `Y` : Bryar Pistol
* `E` : E-11
* `U` : Disruptor
* `B` : Bowcaster
* `I` : Repeater
* `D` : Demp
* `G` : Golan
* `R` : Rocket Launcher
* `C` : Concussion Rifle
* `T` : Thermal Detonators
* `M` : Trip Mines
* `K` : Det Packs

Your most-preferred weapons go at the beginning; least-preferred weapons go at the end. For example, you could enter `/setu prefer RCTIGDUEBSMKYPL`

Note that this must contain EXACTLY 15 letters(one for each weapon). Also note that the command is `setu` with the letter `U` (as in "universe") at the end. Add this to your autoexec.cfg if you want ensure that it runs every time. Clients who do not enter this, or enter an invalid value, will simply use default JK3 weapon priority.

####`/serverstatus2`

Displays many cvars to the client that are not shown with basejka `/serverstatus` command.

####Reset siege to round 1 on map change vote
No more changing maps with timer going down.

####Random teams/capts in siege
base_enhanced supports random teams/capts, but it doesn't work for siege mode. In base_entranced this is fixed and you can generate random teams/capts even in siege(players must set "ready" status by using `/ready` command)

####Unlimited class-changing during countdown
Removed the 5-second delay for class-changing during the countdown.

####`/forceready` and `/forceunready`
Use `/forceready <clientnumber>` and `/forceunready <clientnumber>` to force a player to have ready or not ready status. Use -1 to force everybody.

####`/g_allow_ready`
Use to enable/disable players from using the `/ready` command.

####Public server / Pug server modes
Use `/callvote pug` to exec serverside `pug.cfg` or `/callvote pub` to exec serverside `pub.cfg` (server admin must obviously create and configure these cfg files). Allow vote with `/g_allow_vote_pug` and `/g_allow_vote_pub`

####Improved `/tell` and `/forceteam`
Use partial client name with `/tell` or `/forceteam` (for example, `/tell pada hi` will tell the player Padawan a message saying "hi")

####More custom character colors
Some models allow you to use custom color shading (for example, trandoshan and weequay). Basejka had a lower limit of 100 for these settings(to ensure colors couldn't be too dark); this limit has been removed in base_entranced. Now you can play as a black trandoshan if you want. As in basejka, use the clientside commands `char_color_red`, `char_color_green`, and `char_color_blue` (valid values are between 0-255)

####Additional tools for mapmakers
base_entranced provides mapmakers with powerful tools to have more control over their maps. You can do interesting things with these capabilities that are not possible in base JK3.

Mapmakers can add some new extra flags to .npc files for additional control over NPCs:

`specialKnockback 1` = NPC cannot be knockbacked by red team

`specialKnockback 2` = NPC cannot be knockbacked by blue team

`specialKnockback 3` = NPC cannot be knockbacked by any team

`victimOfForce 1` = Red team can use force powers on this NPC

`victimOfForce 2` = Blue team can use force powers on this NPC

`victimOfForce 3` = Everybody can use force powers on this NPC

`nodmgfrom <#>` = This NPC is immune to damage from these weapons

`noKnockbackFrom <#>` = This NPC is immune to knockback from these weapons

`doubleKnockbackFrom <#>` = This NPC receives 2x knockback from these weapons

`tripleKnockbackFrom <#>` = This NPC receives 3x knockback from these weapons

`quadKnockbackFrom <#>` = This NPC receives 4x knockback from these weapons

* Bit values for weapons:
* Melee				1
* Stun baton			2
* Saber				4
* Pistol				8
* E11				16
* Disruptor		32
* Bowcaster			64
* Repeater			128
* Demp				256
* Golan			512
* Rocket				1024
* Thermal			2048			
* Mine				4096
* Detpack			8192
* Conc				16384
* Dark Force			32768
* Vehicle			65536
* Falling			131072
* Crush				262144
* Trigger_hurt		524288
* Other (lava, sentry, etc)				1048576
* Demp freezing immunity (not fully tested)		2097152

For example, to make an NPC receive double knockback from melee, stun baton, and pistol, add 1+2+8=11, so use the flag `doubleKnockbackFrom 11`

Note that `specialKnockback` overrides any other 0x/2x/3x/4x knockback flags.

Special note on `nodmgfrom`: you can use -1 as shortcut for complete damage immunity(godmode).

Note that using -1 or "demp freezing immunity" will prevent demp from damaging NPC, knockbacking an NPC, or causing electrocution effect.

Mapmakers can add some new extra keys to `worldspawn` entity for additional control over their maps:

Mapmakers can set the new `forceOnNpcs` key in `worldspawn` to 1-3, which forces the server to execute `/g_forceOnNpcs` to a desired number. If set, this cvar overrides `victimOfForce` for all NPCs on the map. If this key is not set, it will default to 0 (no force on NPCs - JK3 default).

Mapmakers can set the new `siegeRespawn` key in `worldspawn`, which forces the server to execute `/g_siegeRespawn` to a desired number. If this key is not set, it will default to 20 (JK3 default).

Mapmakers can set the new `siegeTeamSwitch` key in `worldspawn`, which forces the server to execute `/g_siegeTeamSwitch` to a desired number. If this key is not set, it will default to 1 (JK3 default).

Mapmakers can add some new extra flags to .scl siege class files for additional control over siege classes:
* `ammoblaster <#>`
* `ammopowercell <#>`
* `ammometallicbolts <#>`
* `ammorockets <#>`
* `ammothermals <#>`
* `ammotripmines <#>`
* `ammodetpacks <#>`

For example, adding `ammorockets 5` will cause a class to spawn with 5 rockets, and it will only be able to obtain a maximum of 5 rockets from ammo dispensers and ammo canisters. Note that the `CFL_EXTRA_AMMO` classflag still works in conjunction with these custom ammo amounts; for example, `ammodetpacks 3` combined with `CFL_EXTRA_AMMO` will give 6 detpacks (plus double ammo for all other weapons)

Mapmakers can add some new extra keys to `misc_siege_item` for additional control over siege items:

`autorespawn 0` = item will not automatically respawn when return timer expires. Must be targeted again (e.g., by a hack) to respawn.

`autorespawn 1` = item will automatically respawn when return timer expires (default)

`respawntime <#>` = item will take this many milliseconds to return after dropped and untouched (defaults to 20000, which is the JK3 default). Use `respawntime -1` to make the item never return.

Mapmakers are advised to include the new `healingteam` key to healable `func_breakable`s. Because this key is missing from basejka, if the server is using custom team/class overrides, both teams are able to heal `func_breakable`s. For example, `healingteam 2` ensures only defense will be able to heal it. base_entranced includes hardcoded overrides for Hoth, Desert and Nar Shaddaa, which is why this bug is not noticeable there.

Mapmakers can use the new entity `target_delay_cancel` to cancel the pending target-firing of a `target_delay`. This can be used to create Counter-Strike-style bomb-defusal objectives in which one team must plant a bomb, and the other team must defuse it. For example, an offense hack(planting the bomb) could trigger a `target_delay` for a 10 second delay for the bomb detonation, and a defense hack(defusing the bomb) could trigger a `target_delay_cancel` to cancel the explosion.

Note that if a map includes these new special features, and is then played on a non-base_entranced server, those features will obviously not work.

####Additional control over vote-calling
In addition to the base_enhanced vote controls, you can use these:
* `/g_allow_vote_randomteams`
* `/g_allow_vote_randomcapts`
* `/g_allow_vote_cointoss`
* `/g_allow_vote_q`
* `/g_allow_vote_killturrets`
* `/g_allow_vote_pug`
* `/g_allow_vote_pub`
* `/g_allow_vote_customTeams`

####Bugfixes and other changes:
* Hoth bridge is forced to be crusher (prevents bridge lame).
* Fixed thermals bugging lifts
* Fixed seekers attacking walkers and fighters.
* Fixed sentries attacking rancors, wampas, walkers, and fighters.
* Fixed bug with `nextmap` failed vote causing siege to reset to round 1 anyway.
* Fixed bug with Sil's ctf flag code causing weird lift behavior(people not getting crushed, thermals causing lifts to reverse, etc)
* Fixed bug where round 2 timer wouldn't function properly on maps with `roundover_target` missing from the .siege file.
* Fixed bug where `nextmap` would change anyway when a gametype vote failed.
* Fixed base_enhanced ammo code causing rocket classes not to be able to obtain >10 rockets from ammo canisters.
* Removed Sil's frametime code fixes, which caused knockback launches to have high height and low distance.
* Fixed polls getting cut-off after the first word if you didn't use quotation marks. Also announce poll text for people without a compatible client mod.
* Fixed a bug in Sil's ammo code where `ammo_power_converter`s didn't check for custom maximum amounts (different thing from `ammo_floor_unit`s)
* Fixed a bug in Sil's ammo code where direct-contact `+use` ammo-dispensing didn't check for custom maximum amounts
* Fixed a bug where some people couldn't see spectator chat, caused by the countdown teamchat bugfix.
* Fixed bug with `/class` command not working during countdown.
* Added confirmation messages to the `/class` command
* Fixed Bryar pistol not having unlimited ammo.
* Changes to `g_allowVote` are now announced to the server.
* Fixed `/flourish` not working with gun equipped.
* You can no longer be `/forceteam`ed to the same team you are already on (prevents admin abusing forced selfkill).
* Fixed a bug with `/forceteam`/`/specall`/`/randomteams`/`/randomcapts`/auto inactivity timeout not working on dead players.
* Fixed jumping on breakable objects with the `thin` flag not breaking them as a result of Sil's base_enhanced code.
* Fixed `target_print` overflow causing server crashes
* Removed ability of clients with private password to be immune to votekick (added in base_enhanced by Sil)

#Features that are also in base_enhanced
These are features in base_entranced that are also available in base_enhanced. Many of these features were coded and/or conceived by us first, and then were added to base_enhanced by Sil later.

####`/class`
Clientside command. Use first letter of class to change, like `/class a` for assault, `/class s` for scout, etc. For maps with more than 6 classes, you can use `/class 7`, `/class 8`, etc.

####`/g_rocketSurfing`
0 = no rocket surfing (ideal setting)

1 = bullshit rocket surfing enabled; landing on top of a rocket will not explode the rocket (JK3 default)

####`/g_floatingItems`
0 = no floating siege items (ideal setting for most maps)

1 = siege items float up walls when dropped - annoying bug on most maps, but classic strategy for korriban (JK3 default)

####`/g_selfkillPenalty`
Set to 0 so you don't lose points when you SK.

####`/g_fixPitKills`
0 = normal pit kills (JK3 default)

1 = if you selfkill while above a pit, it grants a kill to whoever pushed you into the pit. This prevents people from denying enemies' kills with selfkill.

####"Joined the red/blue team" message
See when someone joined a team in the center of your screen in siege mode.

####`/pause` and `/unpause`
Use command `/pause` or `/unpause` (also can be called as vote) to stop the game temporarily. Useful if someone lags out. Stops game timer, siege timer, spawn timer, etc.

####`/whois`
Use command `/whois` to see all known aliases of a player.

####Auto-click on death
If you die 1 second before the spawn, the game now automatically "clicks" on your behalf to make the respawn.

####Random teams/capts
Use `/randomteams 2 2` for random 2v2, etc. and `/randomcapts` for random captains. Make sure clients use `/ready` to be eligible for selection (or use `/forceready` through rcon)

####`/specall`
Use `/specall` to force all players to spec.

####Start round with one player
No longer need two players to start running around ingame in siege mode.

####Better logs
Log detailed user info, rcon commands, and crash attempts. Use `g_hacklog <filename>`, `g_logclientinfo 1`, and `g_logrcon 1`.

####Awards/medals support
Humiliation, impressive, etc. if you use the clientside mod SMod and have `cg_drawRewards 1` enabled in your client game.

####`/cointoss`
Call `/cointoss` vote for random heads/tails result.

####Polls
Ask everyone a question with `/callvote q blahblahblah`

####HTTTP auto downloading
Set url with `/g_dlurl`; clients with SMod can download

####Quiet rcon
No more leaking stuff from rcon, use `/g_quietrcon`

####Lag icon above head
Players with 999 ping show a lag icon above their head in-game

####Fixed siege chat
* Spectator chat can be seen by people who are in-game
* Chat from dead players can be seen

####`/npc spawnlist`
Use this command to list possible npc spawns. Note that ingame console only lets you scroll up so far; use qconsole.log to see entire list.

####Control vote-calling
Prevent calling votes for some things:
* `/g_allow_vote_gametype`
* `/g_allow_vote_kick`
* `/g_allow_vote_restart`
* `/g_allow_vote_map`
* `/g_allow_vote_nextmap`
* `/g_allow_vote_timelimit`
* `/g_allow_vote_fraglimit`
* `/g_allow_vote_maprandom`
* `/g_allow_vote_warmup`

####Bug fixes and other changes:
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
* Bugfix for dying while RDFAing and getting camera stuck.
* Bugfix for disconnecting in ATST and going invisible.
* No more SK when already dead.
* No more weird spec glitch with possessing someone else's body.
* Bugfix for sentry placed in lift.
* Allow for many more siege maps/classes on server.
* Bugfix for `map_restart 999999` (now maximum 10)
* Bugfix for spec voting.
* Bugfix for flechette stuck in wall.
* Bugfix for bots getting messed up when changing gametypes.
* Bugfix for healing NPCs/vehicles in siege.
* Bugfix for glitched shield because someone bodyblocked your placement.
* Bugfix for teamnodmg for NPCs
* Bugfix for mindtricking when no enemies are around
* Golan alternate fire properly uses "was shrapnelled by" now
* Trip mine alternate fire properly uses "was mined by" now
* Bugfix for mines doing less splash damage when killed by player in slot 0.
* Miscellaneous slot 0 bugfixes
* Fixed bryar pistol animations
* Fixed players rolling/running through mines unharmed
* Fixed weird bug when getting incinerated while rolling.
* Fixed telefragging jumping players when spawning.
* Fixed NPCs attacking players in temporary dead mode.
* Fixed sentries having retarded height detection for enemies.
* Fixed `CFL_SINGLE_ROCKET` being able to obtain >1 rocket.
* Security/crash fixes.
* Probably more fixes.

# Downloads

####Sample server.cfg [[download]](https://sites.google.com/site/duosjk3siegemods/home/serverstuff)

This sample server.cfg contains recommended settings for all cvars. You should base your server's configuration on this file.

####Patched siege_narshaddaa [[download]](https://sites.google.com/site/duosjk3siegemods/home/serverstuff)

siege_narshaddaa has a bug that causes offense to always complete final obj according to `/g_siegeStats`. This patched pk3, which fixes this bug, is only required serverside. Clients do not need to update to this version.

####Patched siege_cargobarge [[download]](https://sites.google.com/site/duosjk3siegemods/home/serverstuff)

siege_cargobarge has a useless extra ammo flag for defense HW, which was ignored in basejka's buggy code. Since the ammo code is fixed in base_entranced, this results in D HW spawning with 20 rockets. This patched pk3, which fixes this bug, is only required serverside. Clients do not need to update to this version.

####Droid lame fix [[download]](https://sites.google.com/site/duosjk3siegemods/home/serverstuff)
base_entranced fixes teamnodmg, so for example, defense on Hoth cannot attack the droid. Unfortunately, this allows defense to lame the droid by knockbacking it into pits, unreachable spots, etc. This patch, which disables knockbacking the droid, is only required serverside.

####base_entranced pk3 [[download newest version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8eTNRbTNoNVhJOTg/view?usp=sharing)
Version: base_entranced-10-26-2015-build47 (experimental) - add `/iLikeToDoorspam`, add `/iLikeToMineSpam`, add `/g_autoKorribanSpam`, remove anti-votekick for private clients, additional fixes for `target_print` server crash, reverted to release compile settings

Old versions:

Version: base_entranced-10-23-2015-build46 (debug build) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8WnAwS2p2UVdENjg/view?usp=sharing) - add `/g_fixVoiceChat`, add `/g_fixHothDoorSounds`, add `/g_botJumping`, rename eweb fixing cvar to `/g_fixEweb` 

Version: base_entranced-10-22-2015-build45 (debug build) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8SUpoa1NfWTV6YTA/view?usp=sharing) - add `/g_korribanRedRocksReverse`, add `g_jk2SaberMoves`, fix crash caused by `target_print` in debug builds(e.g. on siege_teampicker and siege_cargobarge), allow `/killturrets` to work before round start, improve eweb aiming if `/g_fixEwebRecoil` is enabled, force Hoth bridge to do 9999 damage when blocked

Version: base_entranced-10-19-2015-build44 (debug build) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8TUs3djhfYlJXeGM/view?usp=sharing) - add `/g_fixEwebRecoil`, add `/g_tauntWhileMoving`, allow `/g_fixFallingSounds` scream to work with kills affected by `/g_fixPitKills`, require >= 100 damage from a `trigger_hurt` to cause scream with `/g_fixFallingSounds`, fix bug with cargo stations crashing server, remove some unused duel cvars

Version: base_entranced-10-13-2015-build41 (debug build) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8eXRhMFdiRTNnalE/view?usp=sharing) - add `/g_autoResetCustomTeams`, add `target_delay_cancel`

Version: base_entranced-10-12-2015-build40 (debug build) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8d2VDQXFYZUFNZ3M/view?usp=sharing) - add `/g_specAfterDeath`, add `/g_requireMoreCustomTeamVotes`, add `/g_antiCallvoteTakeover`, add `/g_antiHothHangarLiftLame`, prevent being `/forceteam`ed to same team(forced selfkill), fix bug with `/forceteam`/`/specall`/`/randomteams`/`/randomcapts`/auto inactivity timeout not working on dead players,  allow -1 `respawntime` for custom siege items

Version: base_entranced-10-11-2015-build39 (experimental) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8VXgxTEFJMWlYVnc/view?usp=sharing) - add `/g_redTeam`, add `/g_blueTeam`, add `/g_forceDTechItems`, add `/g_allow_vote_customTeams`, add `/g_moreTaunts 2`, add `/serverstatus2`, announce changes to `/g_allowVote`, fix bryar pistol not having unlimited ammo

Version: base_entranced-10-10-2015-build38 (experimental) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8VXcxdkZiQmVPdk0/view?usp=sharing) - add pug server and public server votes, add `/g_gripRefresh`

Version: base_entranced-10-5-2015-build37 (experimental) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8ZnVDQ0JrNng2QXc/view?usp=sharing) - fix bug with some people not seeing spectator chat, allow unlimited class-changing during countdown(remove 5-second delay), fix `/class` not working during countdown, add confirmation for class change

Version: base_entranced-10-3-2015-build36 (experimental) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8NHRRdjlfRlo0aEE/view?usp=sharing) - add `/g_autoKorribanFloatingItems`, fix thermals bugging lifts, fix +use ammo dispensing not checking for custom max ammo amounts, fix bug with `/g_fixGripKills` causing kills from unknown clients

Version: base_entranced-10-1-2015-build35 (experimental) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8UWt2R1lNT2dIbzg/view?usp=sharing) - fix bug with g_fixHothBunkerLift getting stuck if you held down +use, increase grip refresh rate, allow spaces in poll, announce poll

Version: base_entranced-10-1-2015-build34 (experimental) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8SVd0SFRzbE9GM1U/view?usp=sharing) - add `prefer`, remove lower limit on custom character colors, add `/g_fixGripKills`, revert Sil's frametime fix code which caused knockback launches to have high height and low distance, fix sentries attacking things they shouldn't attack (rancors, wampas, walkers, fighters), remove some unused powerduel cvars

Version: base_entranced-9-29-2015-build33 (unstable) [download removed] - add `/g_fixRancorCharge`, add `/g_ammoCanisterSound`, fix problem with base_enhanced ammo code preventing HWs from getting >10 ammo from ammo canisters, add additional mapmaker tools for siege item spawning, add additional mapmaker tools for siege class ammo


Version: base_entranced-9-29-2015-build32 (unstable) [download removed] - add `/g_endSiege`, add `/g_moreTaunts`, fix repeatedly calling lift bug with `/g_fixHothBunkerLift`, add objective counter for tied timers in `/g_siegeStats`, fix `CFL_SINGLE_ROCKET` being able to obtain >1 rocket, fix nextmap changing on failed gametype vote, fix sentries having retarded height detection for enemies, fix rancor charging through `BLOCKNPC` areas (e.g. desert arena door), fix camera bug from getting disintegrated while rolling, add `/npc spawnlist`, fix npcs attacking players in tempdeath, frametime bugfixes from sil

Version: base_entranced-9-25-2015-build31 (experimental) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8ZEZTeTRVN3JGNk0/view?usp=sharing) - add preliminary `/g_siegeStats`, fix bryar animations, fix players rolling/running through tripmines unharmed

Version: base_entranced-9-24-2015-build30 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8X1BNa1pjdHdtMWs/view?usp=sharing) - revert bugged multiple idealclasses, fix spawn telefragging, fix RDFA camera bug, fix some minor slot 0 things

Version: base_entranced-9-21-2015-build29 (unstable) [download removed] - add `/g_infiniteCharge` by popular demand, fix bug with `idealclass` not supporting multiple classes (this introduced some new bugs, which are fixed in build 30)

Version: base_entranced-9-19-2015-build28 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8a1R4MmNnODRJeVk/view?usp=sharing) - add `/g_fixHothBunkerLift`, add `/g_enableCloak`, add `/g_allow_vote_killturrets`, fix bugged timer if `roundover_target` is missing in .siege file, patch hoth bridge lame

Version: base_entranced-9-14-2015-build27 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8Vkc0Tm00VG5rQVE/view?usp=sharing) - add `/killturrets` (rcon or callvote), allow partial name for `/tell` and `/forceteam`, add notifications for `/forceteam` and `/specall`

Version: base_entranced-9-12-2015-build26 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8dHY0RVJvby03Q2M/view?usp=sharing) - fix untouchable siege items, a bunch of mapmaking stuff(add .npc file flags `nodmgfrom` / `noKnockbackFrom` / `doubleKnockbackFrom` / `tripleKnockbackFrom` / `quadKnockbackFrom` / `victimOfForce`, add `/g_forceOnNpcs`, define s`iegeRespawn`/`siegeTeamSwitch`/`forceOnNpcs` in `worldspawn`)

Version: base_entranced-9-9-2015-build25 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8X0R0cjB6RjRiXzQ/view?usp=sharing) - add `/forceready`, add `/forceunready`, add `/g_allow_ready`, fix "was shrapnelled by" message for golan alternate fire, fix "was mined by" message for tripmine alternate fire, fix mindtricking when no enemies are nearby, fix team joined message on class change during countdown

Version: base_entranced-9-8-2015-build24 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8ckRNQWYyZ1A4V0E/view?usp=sharing) - fix jesus godmode bug from `/pause`, fix players unable to connect during `/pause`, fix sentry expiry timer bug from `/pause`, add `dempProof` and `specialKnockback`

Version: base_entranced-9-5-2015-build23 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8NV9JbDU4am1ia3M/view?usp=sharing) - fix other players bugging shield, fix healing npcs, fix `teamnodmg` for npcs, some very minor bugfixes

Version: base_entranced-9-3-2015-build21 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8bklMRVFWUndoMXc/view?usp=sharing) - additional shield debug logging, experimental random teams support for siege

Version: base_entranced-9-1-2015-build20 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8VFVkYk1IeVppb2M/view?usp=sharing) - add `/g_hideSpecLocation`, add `/g_denoteDead`

Version: base_entranced-8-31-2015-build19 (experimental) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8WC1Obm55OHFOeTQ/view?usp=sharing) - fix weird lift behavior with people not getting crushed and dets making lifts reverse and other weird things, revert sil's walker-spawning-in-shield bugfix that re-added player-spawning-in-shield bug, remove `/g_doorGreening` (it is now permanently enabled), remove `/g_fixNodropDetpacks` (it is now permanently default JK3 behavior)

Version: base_entranced-8-30-2015-build18 (experimental) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8WEE3VDhHNUI4dkk/view?usp=sharing) - add `/class`, add `/g_fixNodropDetpacks`, add shield logging, some misc fixes

Version: base_entranced-8-25-2015-build16 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8SjVLV2pueUt1T00/view?usp=sharing) - add `/g_allow_vote_randomteams`, add `/g_allow_vote_randomcapts`, add `/g_allow_vote_q`, `add /g_allow_vote_allready`, add `/g_allow_vote_cointoss`, some minor bug fixes

Version: base_entranced-8-23-2015-build15 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8SG1RdlRQVkJmY0k/view?usp=sharing) - fix nextmap vote failed bug, add `/siege_restart`, add `/forceround2` (can be used from rcon or callvote), some misc. engine fixes from sil

Version: base_entranced-8-22-2015-build13 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8VUxRdTlOcEt2Rkk/view?usp=sharing) - revert sil's broken shield code, add `/g_rocketSurfing`, `/g_floatingItems`, change `/g_selfkill_penalty` to `g_selfkillPenalty` (no underscores), reset siege to round 1 on map change vote, fix seeker attacking walker, fix seeker/sentry attacking disconnected clients, fix turret splash kill message, fix sniper shot incineration camera bug, cvar overhaul

Version: base_entranced-8-21-2015-build10 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8ajRsbkx5TkRsaE0/view?usp=sharing) - add `/g_nextmapwarning`

Version: base_entranced-8-20-2015-build9 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8aTJJM2hjbGMtbmc/view?usp=sharing) - use sil's siege `/pause` fix

Version: base_entranced-8-20-2015-build8 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8dHVMZHZQOHZjZ3M/view?usp=sharing) - fix rancor bug, use sil's atst code

Version: base-entranced-8-20-2015-build7 (stable) [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8bzMtYXExcVh5QnM/view?usp=sharing) - add `/g_doorgreening`

Version: base-entranced-8-19-2015-build6 (stable) - [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8TU1zTFpmX2p4LTA/view?usp=sharing) - fix hoth first obj points

Version: base-entranced-8-19-2015-build5 (stable) - [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8dERzQzNSVV9LR1E/view?usp=sharing) - add `/g_fixfallingsounds`

Version: base_entranced-8-19-2015-build4 (stable) - [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8aGwtRzhNSXZzaUU/view?usp=sharing) - fix countdown teamchat

Version: base_entranced-8-19-2015-build3 (stable) - [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8ZlBTc3dDcy1lajA/view?usp=sharing) - fix ATST kills

Version:  base_entranced-8-19-2015-build2 (stable) - [[download old version]](https://drive.google.com/file/d/0B-vLJdPP0Uo8bUhfR3dBcWtOWXc/view?usp=sharing) - add `/g_sexydisruptor` and `/g_fixsiegescoring`
