{
  description = "Build Shell with any dependency of the project";

  inputs.flake-utils.url = "github:numtide/flake-utils";
  inputs.nixpkgs.url = "github:NixOs/nixpkgs";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let pkgs = nixpkgs.legacyPackages.${system};
        in
        {
            devShells.default = pkgs.mkShell {
              nativeBuildInputs = with pkgs; [
                man
              ];
            shellHook = ''
              HISTFILE="$(pwd)/.HISTFILE"
              alias clr='clear'
              alias _='sudo'
            '';
          };
        }
      );
}
