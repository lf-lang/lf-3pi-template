{ pkgs ? import <nixpkgs> {} } :
let
  # lf = pkgs.callPackage ./nix/lf.nix {};
  # lingo = pkgs.callPackage ./nix/lingo.nix {};
in 
pkgs.mkShell {
  packages = with pkgs; [
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

  # TODO: integrate dependencies into nix
  shellHook = ''
    echo "[shell] hook start"
    echo "[shell] setup pico-sdk"
    git submodule update --init
    cd pico-sdk/
    git submodule update --init
    export PICO_SDK_PATH="$PWD"
    cd ../
    echo "[shell] copy robot header"
    # FIXME: Is this needed?
    cp robot-lib/pololu_3pi_2040_robot.h pico-sdk/src/boards/include/boards/
    echo "[shell] setup testbed"
    cd test/
    npm install
    echo "[shell] hook end"
    cd ../
    '';
}
