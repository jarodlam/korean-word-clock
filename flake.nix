{
  description = "Korean word clock";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-25.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        shell-v1 = pkgs.callPackage ./v1/shell.nix { };
        shell-v2 = pkgs.callPackage ./v2/shell.nix { };
        devShell = pkgs.mkShell {
          name = "korean-word-clock";
          inputsFrom = [
            shell-v1
            shell-v2
          ];
        };
      in
      {
        devShells = {
          default = devShell;
          v1 = shell-v1;
          v2 = shell-v2;
        };
      }
    );
}
