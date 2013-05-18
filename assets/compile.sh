#!/bin/bash
STEAM_DIR=/C/Program\ Files\ \(x86\)/Steam/steamapps
ASSET_DIR=D:/dev/hl2-vr-bkp/assets

cd "$STEAM_DIR/killdog84/sourcesdk/bin/orangebox/bin"
studiomdl $ASSET_DIR/weapons/$1

cd "$STEAM_DIR/sourcemods"
cp starwars-source/models/weapons/* halflife-vr/models/weapons
echo "Weapon models compiled and placed in halflife-vr"
