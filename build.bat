@echo off

cl /nologo /EHsc -Zi win32_game.cpp -D ASSET_BUILD /I"D:\Work\Libs" /link /LIBPATH:D:\Work\Libs\SDL2\libs\x86 SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib SDL2_mixer.lib

mv *.obj build\
mv *.exe build\
mv *.pdb build\
mv *.ilk build\
