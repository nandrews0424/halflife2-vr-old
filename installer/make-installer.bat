#this is currently only git bash compatible but that's all I use

cp ../src/game/client/Release_episodic/client.dll package/
cp ../src/game/server/Release_episodic/server.dll package/
cp ../src/libfreespace/lib/libfreespace.dll package/

#TODO: what's the actual install structure going to be?

/c/Program\ Files\ \(x86\)/NSIS/makensis.exe installer.nsi