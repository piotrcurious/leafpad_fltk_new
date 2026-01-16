CXXFLAGS="-Os -flto -fuse-linker-plugin -ffunction-sections -fdata-sections -fno-exceptions"
LDFLAGS="-flto -fuse-linker-plugin -Wl,--as-needed -Wl,--no-undefined -Wl,--gc-sections -Wl,-O1"
make CXXFLAGS="$CXXFLAGS" LDFLAGS="$LDFLAGS"
strip --strip-unneeded -s  src/leafpad
