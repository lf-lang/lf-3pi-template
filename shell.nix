{ pkgs ? import <nixpkgs> {} } :
let
  # lf = pkgs.callPackage ./nix/lf.nix {};
  # lingo = pkgs.callPackage ./nix/lingo.nix {};
in 
pkgs.mkShell {
  packages = with pkgs; [
    lolcat
    picotool
    cmake
    gcc-arm-embedded
    openocd
    git
    nodejs
    zellij
    screen
    gdb
    #fzf
  ];
  buildInputs = [
    # lf
    # lingo
  ];

  PICO_BOARD = "pololu_3pi_2040_robot";
  PICO_PLATFORM = "";  # intentionally empty valued, allowing board header to set it

  # TODO: integrate dependencies into nix
  shellHook = ''
    echo "[shell] hook start"
    echo "[shell] setup pico-sdk"
    git submodule update --init
    cd pico-sdk/
    git submodule update --init
    export PICO_SDK_PATH="$PWD"
    echo "[shell] PICO_SDK_PATH: $PICO_SDK_PATH" | lolcat
    cd ../
    export PICO_BOARD_HEADER_DIRS="$PWD/robot-lib"
    echo "[shell] PICO_BOARD: $PICO_BOARD" | lolcat
    echo "[shell] PICO_PLATFORM: $PICO_PLATFORM" | lolcat
    echo "[shell] PICO_BOARD_HEADER_DIRS: $PICO_BOARD_HEADER_DIRS" | lolcat
    echo "[shell] setup testbed"
    cd test/
    npm install
    cd ../
    echo '[shell] To exit the shell, type `exit` or press `Ctrl`+`D`'
    echo "[shell] hook end"
    '';
}
