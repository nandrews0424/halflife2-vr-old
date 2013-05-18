#!/bin/bash
STEAM_DIR=/C/Program\ Files\ \(x86\)/Steam/steamapps
ASSET_DIR=D:/dev/hl2-vr-bkp/assets

cd "$STEAM_DIR/killdog84/sourcesdk/bin/orangebox/bin"

studiomdl $ASSET_DIR/weapons/357/Python_357_reference.qc
studiomdl $ASSET_DIR/weapons/BugBait/Bugbait_reference.qc
studiomdl $ASSET_DIR/weapons/Crossbow/Crossbow_reference3.qc
studiomdl $ASSET_DIR/weapons/Crowbar/crowbar_reference.qc
studiomdl $ASSET_DIR/weapons/grenade/Grenade_reference.qc
studiomdl $ASSET_DIR/weapons/Hands/Hands_reference.qc
studiomdl $ASSET_DIR/weapons/IRifle/IRifle_reference.qc
studiomdl $ASSET_DIR/weapons/physcannon/Physcannon_reference.qc
studiomdl $ASSET_DIR/weapons/pistol/Pistol_reference.qc
studiomdl $ASSET_DIR/weapons/RPG/RPG_reference.qc
studiomdl $ASSET_DIR/weapons/Shotgun/Shotgun_reference.qc
studiomdl $ASSET_DIR/weapons/SuperPhyscannon/SuperPhyscannon_reference.qc


cd "$STEAM_DIR/sourcemods"
cp starwars-source/models/weapons/* halflife-vr/models/weapons
echo "Weapon models compiled and placed in halflife-vr"
