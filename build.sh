
clang++ -std=c++11 mac_platform.cpp -o tetroman -g -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

mv tetroman build/
# cp -rf tetroman.dSYM build/tetroman.dSYM
# rm -r tetroman.dSYM