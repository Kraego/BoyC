#!/bin/bash
mkdir -p ../tests/test_roms
cd ../tests/test_roms

# downloads a ZIP of the entire gb-test-roms repository
curl -L -o game-boy-test-roms.zip \
     https://github.com/c-sp/game-boy-test-roms/archive/refs/heads/master.zip

unzip game-boy-test-roms.zip