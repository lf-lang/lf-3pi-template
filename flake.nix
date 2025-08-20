{
  description = "lf-pico dev env";
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-25.05";
    flake-utils.url = "github:numtide/flake-utils";
    # lingo
    # lf binaries
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.simpleFlake {
      inherit self nixpkgs;
      name = "lf-pico";
      shell = ./shell.nix;
    };
}
