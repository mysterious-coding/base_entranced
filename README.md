# base_entranced

a multiplayer server mod for Jedi Knight 3's Siege gametype

by Duo

a fork of Sil's old [base_enhanced](https://github.com/TheSil/base_enhanced) CTF server mod

# About base_entranced

base_entranced is intended mainly for the Siege gametype, although it can be played with any gametype. It is intended for **classic, competitive Siege gameplay**, not as a general-purpose "hangout server" mod.

base_entranced is the official server mod of the siege community [(www.jasiege.com)](http://www.jasiege.com). Due to its large amount of bugfixes and enhancements, playing siege on any other mod is severely buggy and frustrating, not to mention that many siege maps now *require* base_entranced's enhanced mapping framework to function properly.

base_entranced has three goals:
* Fixing bugs.
* Adding enhancements to basejka siege gameplay and server administration.
* Providing an enhanced mapping framework that allows for many more possilibities for siege mappers.

base_entranced strives to remain close to basejka gameplay. You won't see anything like kiss emotes or grappling hooks like JA+, nor is it wildly different from basejka like MB2. It is a simple improvement of basejka Siege, with competitive gameplay in mind.

The only sizable change to gameplay is the anti-spam cvars. Although base_entranced strives to remain faithful basejka Siege gameplay, doorspam and minespam are fundamental flaws in the game design, and needed to be addressed. Apart from those cvars, everything else generally adheres to the "stay-close-to-basejka" philosophy.

You can discuss base_entranced on the **base_entranced forum** at [www.jasiege.com](http://www.jasiege.com)

base_entranced is icensed under GPLv2 as free software. You are free to use, modify and redistribute base_entranced following the terms in LICENSE.txt.

# Notice to server admins

base_entranced is intended to be fully usable "out of the box." Most cvars default to their ideal/recommended setting for a pug server, as far as the Siege community is concerned. You shouldn't really need to change anything to get your server running, although the cvars are nevertheless there for you to change if you would like to customize your server to your liking. I do recommend that you download the "Droid lame fix" PK3 (see "Downloads" section below).

# Downloads

#### Download base_entranced
To download base_entranced, please find the "releases" button on the Github Repo, or simply go to [https://github.com/deathsythe47/base_entranced/releases](https://github.com/deathsythe47/base_entranced/releases)

#### Droid lame fix
[[download]](https://sites.google.com/site/duosjk3siegemods/home/serverstuff)
base_entranced fixes `teamnodmg`, so for example, defense on Hoth cannot attack the droid. Unfortunately, this allows defense to lame the droid by knockbacking it into pits, unreachable spots, etc. This patch, which disables knockbacking the droid, is only required serverside.

# base_entranced features
These are unique features for base_entranced.

#### `/g_autoStats`
0 = no stats (default JK3)

1 = (default) automatically prints stats at the end of each round, including objective times, kills, deaths, damage, etc. Some maps also have their own unique stats for that map. Clients can also access these stats manually by using the `stats` command.

#### `/g_classLimits`
0 = no limits (default JK3)

1 = (default) enable class limits + automatic overrides from map/server mod

2 = enable class limits + DO NOT use automatic overrides from map/server mod

If `/g_classLimits` is enabled, you can use twelve cvars to limit the number of people who can play a particular class. For example, `/dAssaultLimit 1` limits the number of defense assaults to one, `/oJediLimit 2` limits offense to two jedis, etc. With `/g_classLimits 1`, some automatic overrides for community maps will be applied from bsp files and from hardcoded limits in the server mod itself.

#### `/g_delayClassUpdate`
0 = instantly broadcast class changes

1 = (default) instantly broadcast class changes to teammates; delay class change broadcast for enemies until after the respawn. Prevents the stupid "oh you changed class 150ms before the respawn? Let me counter you by changing class 140ms before the respawn" garbage.

#### `/g_siegeTiebreakEnd`
0 = (default) no tiebreaking rules (JK3 default)

1 = enforce traditional siege community tiebreaking rule. If round 1 offense got held for maximum time(20 minutes on most maps), game will end once round 2 offense has completed one more objective. Round 2 offense will be declared the winner of the match.

#### `/g_fixSiegeScoring`
0 = dumb default JK3 scoring (20 pts per obj, 30 pts for final obj, 10 pt bonus at end)

1 = (default) improved/logical scoring (100 pts per obj)

#### `/g_fixVoiceChat`
0 = enemies can hear your voice chats and see icon over your head (default JK3)

1 = (default) only teammates can hear your voice chats and see icon over your head (except for air support, which is used to BM your enemies)

#### `/iLikeToDoorSpam`
0 = (default) door spam prohibited for blobs, golan balls, rockets, conc primaries, thermals, and bowcaster alternates within a limited distance of enemies in your FOV. Wait until door opens to fire (skilled players already do this). Does not apply if a walker, shield, or someone using protect or mindtrick is nearby. Warning: turning on this setting will cause terrible players to complain.

1 = door spam allowed, have fun immediately getting hit to 13hp because some shitty was raining blobs/golans/etc on the door before you entered (default JK3)

#### `/iLikeToMineSpam`
0 = (default) mine spam prohibited within a limited distance of enemies in your FOV. No throwing mines at incoming enemies anymore (skilled players already refrain from this). Does not apply if a walker, shield, or someone using protect or mindtrick is nearby. Warning: turning on this setting will cause terrible players to complain.

1 = mine spam allowed, have fun insta-dying because some shitty was holding down mouse2 with mines before you entered the door (bonus points for "was planting, bro" excuse) (default JK3)

#### `/iLikeToShieldSpam`
0 = (default) shield spam prohibited; you have to be killed by an enemy or walker explosion to place a new shield. You can place a new shield at each objective, with one freebie ("Yo shield") during the 20 seconds immediately after an objective.

1 = shield spam allowed, have fun getting shield spammed

#### `/g_autoKorribanSpam`
0 = spam-related cvars are unaffected by map

1 = (default) `/iLikeToDoorSpam` and `/iLikeToMineSpam` automatically get set to 1 for Korriban, and automatically get set to 0 for all other maps

#### `/g_fixShield`
0 = bugged basejka shield behavior; shields placed along the x-axis of a map are 25% taller (even though they do not visually reflect it in unpatched client mods)

1 = (default) fixed shield behavior; all shields are correct height

2 = break all shields; all shields are 25% taller

#### `/g_fixHothBunkerLift`
0 = normal lift behavior for Hoth codes bunker lift (default JK3)

1 = (default) Hoth codes bunker lift requires pressing `+use` button (prevents you from killing yourself way too easily on this dumb lift)

#### `/g_fixHothDoorSounds`
0 = Hoth bunker doors at first objective are silent (bug from default JK3)

1 = (default) Hoth bunker doors at first objective use standard door sounds

#### `/g_antiHothCodesLiftLame`
0 = normal behavior for Hoth codes delivery bunker lift

1 = (default) defenders cannot call up Hoth codes delivery bunker lift if a non-Jedi codes carrier is inside the bunker

#### `/g_antiHothHangarLiftLame`
0 = normal behavior for Hoth hangar lift (default JK3)

1 = defense tech uses a 2 second hack to call up the lift. Returns to normal behavior after the hangar objective is completed.

2 = any player on defense is prevented from calling up the lift if any player on offense is nearby ("nearby" is defined as between the boxes in the middle of the hangar and the lift). Returns to normal behavior after the hangar objective is completed.

3 = use both 1 and 2 methods.

4 = (default) use both 1 and 2 methods, plus, after the infirmary has been breached, only allow the defense to call the lift up once within 15 seconds of the infirmary breach. (default setting)

#### `/g_antiHothInfirmaryLiftLame`
0 = normal behavior for Hoth infirmary "short" lift (default JK3)

1 = (default) defense cannot call the "short" lift up with the top button; they must use the lower button

#### `/g_antiLaming`
0 = (default) no anti-laming provisions (default JK3, suggested setting for pug servers)

1 = laming codes/crystals/scepters/parts, objective skipping, and killing stations with swoops @ desert 1st obj is punished by automatically being killed.

2 = laming codes/crystals/scepters/parts, objective skipping, and killing stations with swoops @ desert 1st obj is punished by automatically being kicked from the server.

#### `/g_autoKorribanFloatingItems`
0 = `g_floatingItems` is unaffected by map change

1 = (default) `g_floatingItems` automatically gets set to 1 for Korriban, and automatically gets set to 0 for all other maps

#### `/g_nextmapWarning`
0 = no warning (default JK3)

1 = (default) when nextmap vote is called in round 2, a warning message appears (so you don't accidentally reset the timer going up when starting round 2)

#### `/g_improvedTeamchat`
0 = default JK3 team chat

1 = show selected class as "location" during countdown, show "(DEAD)" in teamchat for dead players, hide location from teamchat between spectators, hide location from teamchat during intermission

2 = (default) all of the above, plus show HP in teamchat for alive players

#### `/g_fixFallingSounds`
0 = default JK3 sound (normal death sound)

1 = (default) use falling death scream sound when damaged by a `trigger_hurt` entity for >= 100 damage (i.e., death pits). Also plays scream sound if selfkilling while affected by `/g_fixPitKills`

#### `/g_fixEweb`
0 = default JK3 eweb behavior (huge annoying recoil, etc)

1 = (default) remove eweb recoil, remove "unfolding" animation when pulling out eweb, make eweb crosshair start closer to normal crosshair

#### `/g_enableCloak`
0 = (default) remove cloak from all siege classes (eliminates need for no-cloak PK3 patches)

1 = cloak enabled (default JK3)

#### `/g_fixRancorCharge`
0 = (default) default JK3 behavior - rancor can charge/jump through `BLOCKNPC` areas (e.g. desert arena door)

1 = rancor cannot charge/jump through `BLOCKNPC` areas

#### `/g_infiniteCharge`
0 = no infinite charging bug with `+useforce`/`+button2` (bugfix from base_enhanced)

1 = (default) infinite charging bug enabled (classic behavior, brought back by popular demand. hold `+useforce` or `+button2` to hold weapon charge indefinitely)

#### `/g_fixGripKills`
0 = normal selfkilling while gripped (default JK3)

1 = (default) selfkilling while gripped counts as a kill for the gripper. This prevents people from denying enemies' kills with selfkill (similar to `/g_fixPitKills` from base_enhanced)

#### `/g_antiCallvoteTakeover`
0 = normal vote calling for `/map`, `/g_gametype`, `/pug`, `/pub`, `/kick`, `/clientkick`, and `lockteams` votes (default JK3)

1 = (default) calling a vote for `/map`, `/g_gametype`, `/pug`, `/pub`, `/kick`, `/clientkick`, or `lockteams` when 6+ players are connected requires at least 2+ people to be ingame. This prevents a lone player calling lame unpopular votes when most of the server is in spec unable to vote no.

#### `/g_moreTaunts`
0 = default JK3 behavior (only allow `/taunt` in non-duel gametypes)

1 = (default) enable `/gloat`, `/flourish`, and `/bow` in non-duel gametypes)

#### `/g_botJumping`
0 = (default) bots jump around like crazy on maps without botroute support (default JK3)

1 = bots stay on the ground on maps without botroute support

#### `/g_swoopKillPoints`
The number of points you gain from killing swoops (1 = default JK3). Set to 0 (the default) so you don't gain points from farming swoops.

#### `/g_sexyDisruptor`
0 = (default) lethal sniper shots with full charge (1.5 seconds or more) cause incineration effect (fixed default JK3 setting, which was bugged)

1 = all lethal sniper shots cause incineration effect (this is just for fun/cool visuals and makes it like singeplayer)

#### `/siege_restart`
rcon command that restarts the current map with siege timer going up from 00:00. Before this, there was no server command to reset siege to round 1, the only way was `/callvote nextmap` (lol)

#### `/forceround2 mm:ss`
Restarts current map with siege timer going down from a specified time. For example, `/forceround2 7:30` starts siege in round 2 with the timer going down from 7:30. Can be executed from rcon or callvote.

#### `/killturrets`
Removes all turrets from the map. Useful for capt duels. Can be executed from rcon or callvote.

#### `/greenDoors`
"Greens" (unlocks) all doors on the map. For testing purposes only; should not be used in live games.

#### `/autocfg_map`
0 = (default) no automatic cfg execution (default JK3)

1 = server will automatically execute `mapcfgs/mapname.cfg` at the beginning of any siege round according to whatever the current map is. For example, if you change to `mp/siege_desert`, the server will automatically execute `mapcfgs/mp/siege_desert.cfg` (if it exists). This should eliminate the need for map-specific cvars like `/g_autoKorribanFloatingItems`, etc.

#### `/autocfg_unknown`
0 = (default) if `autocfg_map` is enabled, but the server is unable to find `mapcfgs/mapname.cfg`, nothing will happen.

1 = if `autocfg_map` is enabled, but the server is unable to find `mapcfgs/mapname.cfg`, the server will instead execute `mapcfgs/unknown.cfg` as a fallback (if it exists).

#### `/g_defaultMap`
If specified, the server will change to this map after 60 seconds of being empty.

#### `/g_hothRebalance`
0 = (default) hoth classes are unchanged

bitflag 1 = o assault gets big bacta

bitflag 2 = o hw gets regular bacta and e11

bitflag 4 = d jedi gets heal 3

-1 = enable all

#### Custom team/class overrides
You can override the classes for any siege map. Use `/g_redTeam <teamName>` and `/g_blueTeam <teamName>`. For example, to use Korriban classes on any map, you could type `/g_redTeam Siege3_Jedi` and `/g_blueTeam Siege3_DarkJedi`.

To reset to base classes, use `0` or `none` as the argument.

This also works with votes; you can do `/callvote g_redTeam <teamName>`. Enable this vote with `/g_allow_vote_customTeams`.

Make sure to use the correct team name, which is written inside the .team file -- NOT filename of the .team file itself. The base classes leave out a final "s" in some of the filenames (`Siege1_Rebels` versus `Siege1_Rebel`).

A few important clientside bugs to be aware of:
* If custom teams/classes are in use, you cannot use the Join Menu to join that team. You must either use `/team r` or `/team b` (easiest method), autojoin, or use a CFG classbind.
* Ravensoft decided to combine force powers and items into one menu/cycle in JK3; however, if you have both items and force powers, it will only display the force powers. So for example if you are using Korriban classes on Hoth and want to place a shield as D tesh, you need to use a `/use_field` bind.
* If the server is using teams/class that you don't have at all (like completely new classes, or classes for a map you don't have), you will see people as using Kyle skin with no sounds and no class icons.

#### `/g_autoResetCustomTeams`
0 = retain custom teams/classes between map change votes

1 = (default) `/g_redTeam` and `/g_blueTeam` are automatically reset to normal classes when map is changed via `/callvote`

#### `/g_requireMoreCustomTeamVotes`
0 = 51% yes votes required for all votes to pass (default JK3)

1 = (default) custom team/class votes require 75% yes votes. This does not apply if the argument is `0` or `none` (resetting to normal classes)

#### `/g_forceDTechItems`
This cvar helps custom team/class overrides by adding some extra weapons/items to the defense tech. Note: these do NOT apply to Korriban. The mod is hardcoded to ignore these values for Korriban. This cvar is only used when custom teams are in use, and does not affect any classes that already have demp/shield.

0 = no additional weapons/items

1 = only Hoth DTech gets demp only

2 = only Hoth DTech gets shield only

3 = only Hoth DTech gets demp and shield

4 = all DTechs get demp only

5 = (default) all DTechs get shield only

6 = all DTechs get demp and shield

7 = all DTechs get shield; only Hoth DTech gets demp also

#### Weapon spawn preference
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

#### `/join`
Clientside command. Use to join a specific class and team, e.g. `/join rj` for red jedi.

#### `/help` / `/rules` / `info`
Client command; displays some helpful commands and features that clients should be aware of (how to use `/whois`, `/class`, etc) as well as version number of currently-running server mod.

#### `/serverstatus2`
Client command; displays many cvars to the client that are not shown with basejka `/serverstatus` command.

#### Broadcast `siegeStatus` in serverinfo
base_entranced broadcasts some useful information, such as which round it currently is, what objective they are on, how much time is left, etc in the serverinfo. If you click to read the serverinfo from the game menu, you can see this information without connecting to the server.

#### Reset siege to round 1 on map change vote
No more changing maps with timer still going down.

#### Random teams/capts in siege
base_enhanced supports random teams/capts, but it doesn't work for siege mode. In base_entranced this is fixed and you can generate random teams/capts even in siege(players must set "ready" status by using `/ready` command)

#### Unlimited class-changing during countdown
Removed the 5-second delay for class-changing during the countdown.

#### Improved `/tell`
You can still use partial client names with `/tell` (for example, `/tell pada hi` will tell the player Padawan a message saying "hi")

#### Improved `/forceteam`
Use partial client name with `/forceteam`. Optionally, you can include an additional argument for the number of seconds until they can change teams again (defaults to 0); for example, `/rcon forceteam douchebag r 60`

#### `/forceclass` and `/unforceclass`
Teams can call special, team-only votes to force a teammate to a certain class for 60 seconds. Use the command `/callteamvote`. For example, `/callteamvote forceclass pad j` will force Padawan to play jedi for 60 seconds. Use `/callteamvote unforceclass pad` to undo this restriction. Use `/teamvote yes` and `/teamvote no` to vote on these special teamvotes. These commands can also be executed with rcon directly.

#### `/g_teamVoteFix`
(default: 1) There is a bug preventing the on-screen teamvote text from displaying on clients' screens if they are running certain client mods (such as SMod). This workaround prints some text on your screen so you can see what the vote is for.

#### `/forceready` and `/forceunready`
Use `/forceready <clientnumber>` and `/forceunready <clientnumber>` to force a player to have ready or not ready status. Use -1 to force everybody.

#### `/g_allow_ready`
(default: 1) Use to enable/disable players from using the `/ready` command.

#### `/rename`
Rcon command to forcibly rename a player. Use partial client name or client number. Optionally, you can include an additional argument for the number of seconds until they can rename again (defaults to 0); for example, `/rcon rename douchebag padawan 60`

#### Duplicate names fix
Players now gain a JA+-style client number appended to their name if they try to copy someone else's name.

#### Lockdown
Due to the possibility of troll players making trouble on the server and spamming reconnect under VPN IPs, you can lock down the server with `/g_lockdown` (default: 0). While enabled, only players who are whitelisted will be allowed to chat, join, rename, or vote. In addition, non-whitelisted players will be renamed to "Client 13" (or whatever their client number is) when they connect. You can add/remove whitelisted players by using the command `/whitelist`.

#### Probation
As a less severe alternative to banning troublemakers, you can simply place them under probation. Take their unique id from the server logs and write it into `probation.txt` in the server's /base/ folder (separate multiple ids with line breaks). Then the player will be treated according to the cvar `g_probation`:

0 = nothing unusual happens; treat players on probation normally

1 = players on probation cannot vote, call votes, use `/tell` to send or receive messages, or use teamchat in spec. their votes are not needed for votes to pass.

2 = (default) same as 1, but they also cannot change teams without being forceteamed by admin.

#### Siege captain dueling
You can now challenge and accept captain duels using the basejka `/engage_duel` command/bind (assuming server has `/g_privateDuel 1` enabled). Both players receive 100 HP, 0 armor, pistol only, 125% speed, no items, no force powers, offense can go through defense-only doors, and turrets are automatically destroyed.

#### Awards/medals support
Humiliation, impressive, etc. Extra awards are being implemented (work-in-progress) for siege maps. You can get rewards if you use a compatible clientside mod such as Smod and have `cg_drawRewards` enabled in your client game.

#### Public server / Pug server modes
Use `/callvote pug` to exec serverside `pug.cfg` or `/callvote pub` to exec serverside `pub.cfg` (server admin must obviously create and configure these cfg files). Allow vote with `/g_allow_vote_pug` and `/g_allow_vote_pub`

#### `/removePassword`
In basejka, it is impossible to remove an existing server password with rcon; the only way is by cfg. Now you can simply use the rcon command `/removePassword` to clear the value of `/g_password`.

#### More custom character colors
Some models allow you to use custom color shading (for example, trandoshan and weequay). Basejka had a lower limit of 100 for these settings(to ensure colors couldn't be too dark); this limit has been removed in base_entranced. Now you can play as a black trandoshan if you want. As in basejka, use the clientside commands `char_color_red`, `char_color_green`, and `char_color_blue` (valid values are between 0-255)

#### Selfkill lag compensation
Selfkilling with high ping can be frustrating on base servers when you accidentally max yourself due to selfkilling too late. base_entranced's lag compensation allows you to selfkill slightly closer to the respawn wave if you have high ping by subtracting your ping from the minimum delay of 1000 milliseconds. For example, if you have 200 ping, you will only have to wait 800 milliseconds before you can respawn instead of 1000. This is because you actually pressed your selfkill bind earlier and the server simply did not receive that packet until later.

#### Map updates/improvements
Some maps have hardcoded fixes in base_entranced in order to eliminate the need for releasing pk3 patches. For example, on siege_cargobarge2, defense demo was given double ammo via base_entranced hardcoding rather than releasing another pk3 patch. Yes, some of these are "hacky," but it's better than forcing everyone to redownload the maps.

#### Enhanced mapping framework
base_entranced provides siege mapmakers with powerful new tools to have more control over their maps. You can do interesting things with these capabilities that are not possible in base JK3.

Mapmakers can add some new extra keys to `worldspawn` entity for additional control over their maps:

Mapmakers can set the new class limits keys in `worldspawn`, which automatically sets the cvars such as `/dAssaultLimit` on servers with `/g_classLimits` set to `1`. For example, adding the key `dHWLimit` with value `1` will cause the server to set the cvar `/dHWLimit` to `1`.

Mapmakers can set the new `forceOnNpcs` key in `worldspawn` to 1-3, which forces the server to execute `/g_forceOnNpcs` to a desired number. If set, this cvar overrides `victimOfForce` for all NPCs on the map. If this key is not set, it will default to 0 (no force on NPCs - basejka setting).

Mapmakers can set the new `siegeRespawn` key in `worldspawn`, which forces the server to execute `/g_siegeRespawn` to a desired number. If this key is not set, it will default to 20 (JK3 default).

Mapmakers can set the new `siegeTeamSwitch` key in `worldspawn`, which forces the server to execute `/g_siegeTeamSwitch` to a desired number. If this key is not set, it will default to 1 (JK3 default).

Mapmakers can set the new `mapversion` key in `worldspawn`, which can be used in conjunction with custom base_entranced code to alter certain things for each map version (for example, if you move the map in a new update, you can automatically adjust anti-spam or any other custom features present in the mod that depend on coordinates). If you don't know what this means, ignore it.

Mapmakers can add some new extra flags to .scl siege class files for additional control over siege classes:
* `ammoblaster <#>`
* `ammopowercell <#>`
* `ammometallicbolts <#>`
* `ammorockets <#>`
* `ammothermals <#>`
* `ammotripmines <#>`
* `ammodetpacks <#>`

For example, adding `ammorockets 5` will cause a class to spawn with 5 rockets, and it will only be able to obtain a maximum of 5 rockets from ammo dispensers and ammo canisters. Note that the `CFL_EXTRA_AMMO` classflag still works in conjunction with these custom ammo amounts; for example, `ammodetpacks 3` combined with `CFL_EXTRA_AMMO` will give 6 detpacks (plus double ammo for all other weapons)

Mapmakers can add the new `drawicon` key to shield/health/ammo generators. Use `drawicon 0` to hide its icon from the radar display (defaults to 1). The main intent of this is to hide shield/health/ammo generators from the radar that are not yet accessible to the players. For example, hiding the Hoth infirmary ammo generators until offense has reached the infirmary objective (use an `info_siege_radaricon` with the icon of the generator and toggle it on/off).

Mapmakers can add some new extra keys to `misc_siege_item` for additional control over siege items:

`autorespawn 0` = item will not automatically respawn when return timer expires. Must be targeted again (e.g., by a hack) to respawn.

`autorespawn 1` = item will automatically respawn when return timer expires (default/basejka)

`respawntime <#>` = item will take this many milliseconds to return after dropped and untouched (defaults to 20000, which is the basejka setting). Use `respawntime -1` to make the item never return.

`hideIconWhileCarried 0` = item's radar icon will be shown normally (default/basejka)

`hideIconWhileCarried 1` = item's radar icon will be hidden while item is carried, and will reappear when dropped

`idealClassType` = this item can only be picked up by this class type on team 1. Use the first letter of the class, e.g. `idealClassType s` for scout.

`idealClassTypeTeam2` = this item can only be picked up by this class type on team 2. Use the first letter of the class, e.g. `idealClassTypeTeam2 s` for scout.

`speedMultiplier` = this item causes a carrier on team 1 to change their speed, e.g. `speedMultiplier 0.5` to make the carrier move at 50% speed.

`speedMultiplierTeam2` = this item causes a carrier on team 2 to change their speed, e.g. `speedMultiplierTeam2 0.5` to make the carrier move at 50% speed.

`removeFromOwnerOnUse 0` = player holding item will continue to hold item after using it

`removeFromOwnerOnUse 1` = player holding item will lose posessesion of item upon using it (default/basjeka)

`removeFromGameOnuse 0` = item entity will remain in existence after using it

`removeFromGameOnUse 1` = item entity will be completely deleted from the game world upon using it (default/basejka)

`despawnOnUse 0` = item will not undergo any special "despawning" upon use (default/basejka)

`despawnOnUse 1` = item will be "despawned" (made invisible, untouchable, and hidden from radar) upon use

An example use case of the "`onUse`" keys could be to allow an item to respawn and be used multiple times, using a combination of `removeFromOwnerOnUse 1`, `removeFromGameOnUse 0`, and `despawnOnUse 1`.

Mapmakers are advised to include the new `healingteam` key to healable `func_breakable`s. Because this key is missing from basejka, if the server is using custom team/class overrides, both teams are able to heal `func_breakable`s. For example, `healingteam 2` ensures only defense will be able to heal it. base_entranced includes hardcoded overrides for Hoth, Desert and Nar Shaddaa, which is why this bug is not noticeable there.

Mapmakers can use the new entity `target_delay_cancel` to cancel the pending target-firing of a `target_delay`. This can be used to create Counter-Strike-style bomb-defusal objectives in which one team must plant a bomb, and the other team must defuse it. For example, an offense hack(planting the bomb) could trigger a `target_delay` for a 10 second delay for the bomb detonation, and a defense hack(defusing the bomb) could trigger a `target_delay_cancel` to cancel the explosion.

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

`normalSaberDamage 1` = This NPC does not receive the basejka damage boost from sabers due to not having a saber equipped

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

Special note on `nodmgfrom`: you can use -1 as shortcut for complete damage immunity(godmode). Also note that using -1 or "demp freezing immunity" will prevent demp from damaging NPC, knockbacking an NPC, or causing electrocution effect.

Mapmakers can use `idealClassType` for triggers activated by red team; use the first letter of the class, e.g. `idealClassType s` for scout. Similarly, use `idealClassTypeTeam2` for blue team.

Note that if a map includes these new special features, and is then played on a non-base_entranced server, those features will obviously not work.

#### Additional control over vote-calling
In addition to the base_enhanced vote controls, you can use these:
* `/g_allow_vote_randomteams` (default: 1)
* `/g_allow_vote_randomcapts` (default: 1)
* `/g_allow_vote_cointoss` (default: 1)
* `/g_allow_vote_q` (default: 1)
* `/g_allow_vote_killturrets` (default: 1)
* `/g_allow_vote_pug` (default: 0)
* `/g_allow_vote_pub` (default: 0)
* `/g_allow_vote_customTeams` (default: 0)
* `/g_allow_vote_forceclass` (default: 1)
* `/g_allow_vote_zombies` (default: 1)
* `/g_allow_vote_lockteams` (default: 1)

#### Zombies
"Zombies" is an unnoficial quasi-gametype that has been played by the siege community to kill time over years. It is a hide-and-seek game that involves one offense jedi hunting down defense gunners after some initial setup time. Gunners who die join offense jedi and hunt until there is only one gunner left.

Zombies receives some much-needed help in base_entranced. To activate the zombies features, use `/zombies` (rcon command) or `/callvote zombies` (assuming you enabled `/g_allow_vote_zombies`). Zombies features include:
* All doors are automatically greened
* All turrets are automatically destroyed
* `/g_siegeRespawn` is automatically set to 5
* No changing to defense jedi
* No changing to offense gunners
* Non-selfkill deaths by blue gunners cause them to instantly switch to red jedi
* No joining the spectators(so you can't spec a gunner and then know where he is after you join red)
* Silences several "was breached" messages on some maps
* Removes forcefields blocking player movement on some maps (e.g. cargo2 command center door)
* Prevents some objectives from being completed (e.g.cargo 2 command center)
* Hides locations in blue team teamoverlay(so you can't instantly track them down after you join red)
* Hides locations in blue team teamchat(so you can't instantly track them down after you join red)
* No using jetpack on cargo2 2nd obj
* Removes auto-detonation timer for detpacks

#### Bugfixes and other changes:
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
* You can no longer switch teams in the moment after you died, or while being gripped by a Rancor.
* You can no longer be `/forceteam`ed to the same team you are already on (prevents admin abusing forced selfkill).
* Fixed a bug with `/forceteam`/`/specall`/`/randomteams`/`/randomcapts`/auto inactivity timeout not working on dead players.
* Fixed jumping on breakable objects with the `thin` flag not breaking them as a result of Sil's base_enhanced code.
* Fixed `target_print` overflow causing server crashes.
* Fixed bug with not regenerating force until a while after you captured an item with `forcelimit 1` (e.g. Korriban crystals)
* Fixed `/bot_minplayers` not working in siege gametype.
* Fixed bug with shield disappearing when placing it near a turret.
* Fixed bug that made it possible to teamkill with emplaced gun even with friendly fire disabled.
* Fixed a rare bug with everyone being forced to spec and shown class selection menu.
* Fixed a bug with final objective sounds (e.g. "primary objective complete") not being played(note: due to a clientside bug, these sounds currently do not play for the base maps).
* Cleaned up the displaying of radar icons on Hoth, Nar Shaddaa, Desert, and siege_codes. Fixes some icons being displayed when they shouldn't (for example, the only icon you should see at Hoth 1st obj is the 1st obj; you don't need to see any of the other objs or ammo gens or anything).
* Mind trick has been hardcoded to be removed from siege_codes, saving the need for me to release a new pk3 update for that map.
* Fixed a bug on the last objective of Desert caused by delivering parts within 1 second of each other.
* Fixed poor performance of force sight causing players to randomly disappear and reappear.
* Improved health bar precision.
* Fixed not regenerating force after force jumping into a vehicle.
* You can now taunt while moving.
* Teamoverlay data is now broadcast to spectators who are following other players.
* Fixed improper initialization of votes causing improper vote counts and improper display of teamvotes.
* Generic "you can only change classes once every 5 seconds" message has been replaced with a message containing the remaining number of seconds until you can change classes.
* Fixed bug with animal vehicles running in place after stopping.
* Fixed bug with "impressive" award being triggered for shooting NPCs.
* Added a workaround (Hoth only) for the bug where vehicles getting crushed cause their pilot to become invisible. Instead, the vehicle will instantly die. (see https://github.com/JACoders/OpenJK/issues/840)
* Fixed incorrect sentry explosion location when dropping it a long vertical distance.
* Voice chat is now allowed while dead.
* Fixed bug with siege items getting stuck when they respawn.
* Fixed double rocket explosion bug when shooting directly between someone's feet with rocket surfing disabled.
* Fixed primary mines not triggering if you were too far away.
* Fixed not being able to use icons for big Hoth turrets.
* Fixed not being able to re-activate inactive big Hoth turrets.
* Fixed not being able to trigger dead turrets.
* Fixed inactive turrets lighting up when respawning.
* Fixed issues with turrets tracking players who are cloaked or changed teams.
* Turret icons are now removed if the turret is destroyed and cannot respawn.
* A small sound is now played when dispensing an ammo canister.
* Fixed "using" mind trick when no enemy players were around.
* The correct pain sounds are now used for Siege (HP-percentage-based instead of HP-based).
* Fixed turrets teamkilling with splash damage.
* Fixed detpacks exploding when near a mover but not touching it.
* Fixed incorrect "station breached" messages on siege_narshaddaa.
* Fixed server crash when capturing an objective with a vehicle that has no pilot.
* Fixed being able to suicide by pistoling the walker.
* Lift kills now properly credit the killer.
* Falling deaths on certain maps now use the correct "fell to their death"/"thrown to their doom" message instead of generic "died"/"killed" message.
* 3-second-long fade-to-black falling deaths are now filtered to instant deaths in Siege.
* Filtered map callvotes to lowercase.
* Hoth map vote is automatically filtered to hoth2 if the server has it.
* Fixed SQL DB lag present in base_enhanced.

# Features that are also in Alpha's base_enhanced
These are features in base_entranced that are also available in Alpha's base_enhanced (https://github.com/Avygeil/base_enhanced), the official server mod of the CTF community. base_entranced and Alpha's base_enhanced share the same ancestor (Sil's base_enhanced), and they are both open source, so they share a number of features. Note that I have not attempted to list every base_enhanced feature here; only the ones that are most relevant to siege.

#### Chat tokens
Clients can use the following chat tokens:

* `$h` = current health
* `$a` = current armor
* `$f` = current force
* `$m` = current ammo
* `$l` = closest weapon spawn (not useful for siege)

#### Advanced random map voting
Instead of the traditional random map voting to have the server pick a map, `/g_allow_vote_maprandom`, if set to a number higher than `1`, will cause the server to randomly pick a few maps from a pool, after which players can vote to increase the weight of their preferred map with `/vote 1`, `/vote 2`, etc.

#### Improved projectile pushing/deflection
Instead of the buggy base JA behavior with pushing/deflecting projectiles, you now have some more options. `/g_breakRNG` (default: 0) controls whether to use old "broken" RNG system from base JA. `/g_randomConeReflection` (default: 0) controls whether to use an improved system of randomly generating a trajectory for the pushed/deflected projectile within a cone of the pusher/deflecter's FOV. `/g_coneReflectAngle` (default: 30) controls how wide of an angle to use for this.

#### `/sv_passwordlessSpectators`
0 = (default) normal server password behavior

1 = prevent people from joining red/blue team if they do not have the correct password entered in their client (using `/password` command or setting through the GUI). The server will automatically abolish its general password requirement if this is set(no password needed to connect to the server). This could be useful for opening up private/pug servers to the public for spectating.

#### `/lockteams`
Callvote or rcon command; shortcut for setting `/g_maxGameClients`. Use arguments `2s`, `3s`, `4s`, `5s`, `6s`, `7s`, or `reset` to specify amount. For example, `/lockteams 4s` is the same as setting `/g_maxGameClients` to 8.

Teams are automatically unlocked at intermission, or if there are 0 players in-game.

#### `/g_teamOverlayUpdateRate`
The interval in milliseconds for teamoverlay data to be updated and sent out to clients. Defaults to 250 (JK3 default is 1000).

#### `/g_balanceSaber` (bitflag cvar)
0 = (default) basejka saber moves

1 = all saber stances can use kick move

2 = all saber stances can use backflip move

4 = using saber offense level 1 grants you all three saber stances

8 = (just for screwing around, not recommended) using saber offense level 2/3 grants you Tavion/Desann stance

#### `/g_balanceSeeing`
0 = (default) basejka force sense behavior

1 = when crouching with saber up with sense 3, dodging disruptor shots is guaranteed

#### `/g_maxNameLength`
Sets the maximum permissible player name length. 35 is the basejka default; anything higher than that is untested (this cvar was intended to be set *lower* than 35).

#### `/clientDesc`
Rcon command to see client mods people are using, if possible.

#### `/shadowMute`
Rcon command to secretly mute people. Only shadowmuted players can see each others' chats.

#### Simplified private messaging
Instead of using tell, you can send private messages to people by pressing your normal chat bind and typing two @ symbols followed by their name and the message, e.g. `@@pad hello dude`

#### Bugfixes and other changes
* Troll/box characters (WSI fonts) are now disallowed from being in player names due to breaking formatting.

# Features that are also in Sil's base_enhanced
These are features in base_entranced that are also available in Sil's now-inactive base_enhanced mod (https://github.com/TheSil/base_enhanced), the legacy server mod of the CTF and siege communities. Since base_entranced was originally based on Sil's base_enhanced, and they are both open source, they share a number of features. Note that I have not attempted to list every base_enhanced feature here; only the ones that are most relevant to siege.

#### `/class`
Clientside command. Use first letter of class to change, like `/class a` for assault, `/class s` for scout, etc. For maps with more than 6 classes, you can use `/class 7`, `/class 8`, etc.

#### `/g_rocketSurfing`
0 = (default) no rocket surfing (ideal setting)

1 = bullshit rocket surfing enabled; landing on top of a rocket will not explode the rocket (JK3 default)

#### `/g_floatingItems`
0 = (default) no floating siege items (ideal setting for most maps)

1 = siege items float up walls when dropped - annoying bug on most maps, but classic strategy for korriban (JK3 default)

#### `/g_selfkillPenalty`
Set to 0 (the default) so you don't lose points when you SK.

#### `/g_fixPitKills`
0 = normal pit kills (JK3 default)

1 = (default) if you selfkill while above a pit, it grants a kill to whoever pushed you into the pit. This prevents people from denying enemies' kills with selfkill.

#### `/g_maxGameClients`
0 = (default) people can freely join the game

other number = only this many players may join the game; the reset must stay in spectators

####"Joined the red/blue team" message
See when someone joined a team in the center of your screen in siege mode.

#### `/pause` and `/unpause`
Use command `/pause` or `/unpause` (also can be called as vote) to stop the game temporarily. Useful if someone lags out. Stops game timer, siege timer, spawn timer, etc.

#### `/whois`
Use command `/whois` to see a list of everyone in the server as well as their most-used alias. Optionally specify a client number or partial name to see the top aliases of that particular player.

#### Auto-click on death
If you die 1 second before the spawn, the game now automatically "clicks" on your behalf to make the respawn.

#### Random teams/capts
Use `/randomteams 2 2` for random 2v2, etc. and `/randomcapts` for random captains. Use `/shuffleteams 2 2` for random teams that are different from the current teams. Make sure clients use `/ready` to be eligible for selection (or use `/forceready`/`/forceunready` through rcon)

#### `/specall`
Use the rcon command `/specall` to force all players to spec.

#### Start round with one player
No longer need two players to start running around ingame in siege mode.

#### Better logs
Log detailed user info, rcon commands, and crash attempts. Use `g_hacklog <filename>`, `g_logclientinfo 1`, and `g_logrcon 1`.

#### Coin toss
Call a `/cointoss` vote for random heads/tails result. Also works as an rcon command.

#### Polls
Ask everyone a question with `/callvote q`. For example, `/callvote q Keep same teams and restart?`

#### HTTP auto downloading
Set url with `/g_dlurl`; clients with compatible mods such as SMod can download

#### Quiet rcon
Mis-typed commands are no longer sent out as a chat message to everyone on the server.

#### Fixed siege chat
* Spectator chat can be seen by people who are in-game
* Chat from dead players can be seen

#### `/npc spawnlist`
Use this command to list possible npc spawns. Note that ingame console only lets you scroll up so far; use qconsole.log to see entire list.

#### Control vote-calling
Prevent calling votes for some things:
* `/g_allow_vote_gametype` (default: 1023)
* `/g_allow_vote_kick` (default: 1)
* `/g_allow_vote_restart` (default: 1)
* `/g_allow_vote_map` (default: 1)
* `/g_allow_vote_nextmap` (default: 1)
* `/g_allow_vote_timelimit` (default: 1)
* `/g_allow_vote_fraglimit` (default: 1)
* `/g_allow_vote_maprandom` (default: 4)
* `/g_allow_vote_warmup` (default: 1)

#### Bugfixes and other changes:
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
* Bugfix for invisible/super-bugged shield because someone bodyblocked your placement.
* Bugfix for teamnodmg for NPCs.
* Golan alternate fire properly uses "was shrapnelled by" now.
* Trip mine alternate fire properly uses "was mined by" now.
* Bugfix for mines doing less splash damage when killed by player in slot 0.
* Miscellaneous slot 0 bugfixes.
* Fixed bryar pistol animations.
* Fixed players rolling/running through mines unharmed
* Fixed weird bug when getting incinerated while rolling.
* Fixed telefragging jumping players when spawning.
* Fixed NPCs attacking players in temporary dead mode.
* Fixed sentries having retarded height detection for enemies.
* Fixed `CFL_SINGLE_ROCKET` being able to obtain >1 rocket.
* Fixed camera bug when speccing someone riding a swoop.
* Security/crash fixes.
* Probably more fixes.
