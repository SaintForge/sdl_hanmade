@echo off

cl /EHsc win32_game.cpp -D ASSET_BUILD /I"W:\libs" /link /LIBPATH:W:\libs\SDL2\lib\x86 SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib SDL2_mixer.lib

mv *.obj build\
mv *.exe build\
