# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

cmake_minimum_required(VERSION 3.15)
project(TDPC)

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH2Bin)
include(Fxconv)
find_package(Gint 2.9 REQUIRED)

set(SOURCES
  src/main.c
  src/menu.c
  src/game.c
  src/turret.c
  src/bot.c
  src/map.c
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
# List image/sound files to be converted by fxconv. Metadata files
# (fxconv-metadata.txt) in the same directories provide names and options.
set(ASSETS
  assets/sprites/environment/blank.bmp
  assets/sprites/environment/grass.bmp
  assets/sprites/environment/rock.bmp
  assets/sprites/points/spawn.bmp
  assets/sprites/points/goal.bmp
  assets/sprites/turrets/turret_1.bmp
  assets/sprites/bots/bot_1.bmp
  assets/sprites/roads/road_bl.bmp
  assets/sprites/roads/road_br.bmp
  assets/sprites/roads/road_tl.bmp
  assets/sprites/roads/road_tr.bmp
  assets/sprites/roads/road_h.bmp
  assets/sprites/roads/road_v.bmp
)
set(ASSETS_fx
  # ...
)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

add_executable(tdpc ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
target_compile_options(tdpc PRIVATE -Wall -Wextra -Os -g -flto)
target_link_libraries(tdpc Gint::Gint)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET tdpc OUTPUT "TDPC.g1a"
    NAME "TDPC" ICON assets/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET tdpc OUTPUT "TDPC.g3a"
    NAME "TDPC" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET tdpc OUTPUT "TDPC-fx.g3a"
    NAME "TDPC-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh2_bin(TARGET tdpc OUTPUT "TDPC-hh2.bin")
endif()
