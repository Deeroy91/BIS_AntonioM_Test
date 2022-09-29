# BIS_AntonioM_Test
 
In this demo you will find Unreal's Shooter game project, with the addition of a grenade with the ability to stick on to targets or bounce off of surfaces, and along with a weapon that launches grenades. The grenade can stick to characters even if it bounces off a surface, like the floor or a wall.

The Shooter Character was given an interaction component that detects and allows the player to interact with actors that implement Interactable interface.
The grenade implements this interface. When detect it gets highlighted and a prompt appears. Interacting with it will increment the grenade launchers ammo count. If the weapon has full ammo, the grenade will not be picked up. 

When the grenade is fired, it will explode after a certain amount of time has passed. This time can be changed on a BP of type ShooterWeapon_Projectile, "under Projectile Config" by editting "ProjectileLife" property. If the grenade hasn't exploded and the character that it was attached to dies, the grenade will simply fall to the ground and roll away until in explodes. An explosion vfx and sfx was added to improve experience. As well as impact and pick up sounds.

Along the default level there are some ammmo pickups for grenades that where scatterd.

As an extra feature, the aiming ability for the grenade launcher was replaced with a display of the path the grenade will perform, as well as the direction it will take after the first bounce. Needless to say, the bounce path will only be displayed if a character is not on the projectiles path.

////////////////////////////////////////////////////////////////////////

Projectiles are server authorative and use Unreal's replication and movement replication to handle client/server synchronization. Spawning and destruction are all done server side. 

The actor that handles effects is also server authorative and uses unreal replication. However the vfx and sfx only run on the client side. Impact and pickup sounds are also only played on the client side.

Player interaction is done by pressing the "E" key, and as mentioned before a prompt is displayed by a widget component. This widget is updated only on the client side, and when the player presses the key a RPC is sent to the server to perform a raycast and handle the pickup and following destruction of the interactable (grenade).

Ammo pickups and ammo count is replicated and updated on both server and client.

////////////////////////////////////////////////////////////////////////

I adhered to the UE4 and the projects code standards. Although I prefer to write my member variables with a "m_" prefix, bI kept instead the same naming convention already used on the project.

The original shooter game project files that were editted have code wrapped in comment lines with "AntonioM BEGIN" and "AntonioM END".

Files editted in VS solution:

- Weapons/ShooterWeapon.h/.cpp
- Weapons/ShooterWeapon_Projectile.h/.cpp
- Weapons/ShooterProjectile.h/.cpp
- Player/ShooterCharacter.h/.cpp

Files editted in Unreal content:

- Blueprints/Pawn/PlayerPawn
- Blueprints/Weapons/WeapRocketLauncher

Files added to VS solution:

- Weapons/ShooterGrenadeProjectile.h/.cpp
- Weapons/ShooterRocketProjectile.h/.cpp
- Weapons/ShooterWeapon_GrenadeLauncher.h/.cpp
- Weapons/ShooterWeapon_RocketLauncher.h/.cpp
- Player/InteractionComponent.h/.cpp
- Interfaces/InteractableInterface.h/.cpp
- UI/KeyPromptWidget.h/.cpp

Files added to Unreal content:

- Blueprints/Weapons/ProjStickyGrenade
- Blueprints/Weapons/ProjGrenade_Explosion
- Blueprints/Weapons/WeapGrenadeLauncher
- Blueprints/Pickups/Pickup_AmmoStickyGrenade
- UI/BP_KeyPromptWidget
- Effects/Materials/M_ImpactMarker
- Effects/Materials/MI_ImpactMarker
- Effects/Materials/M_InteractableOutline
- Effects/Materials/M_ProjectilePath

////////////////////////////////////////////////////////////////////////

Thoughts on additional and extras features and improvements that could be added to this demo:

- Beeping sounds that increase in frequency when the grenade is close to exploding.
- Flashy UI element to signal grenade location and proximity when player isn't looking at it directly. 
- Key prompts change based on type of controller input (gamepad vs keyboard/mouse), additionally displayed text should be marked for Localization.

Invested time: 48 hours
