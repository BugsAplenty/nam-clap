{
  description = "NAM CLAP plugin via DPF using BugsAplenty/NeuralAmpModelerCore (no submodules)";

  inputs = {
    nixpkgs.url     = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
    dpf.url   = "github:DISTRHO/DPF";
    dpf.flake = false;
    nam.url   = "github:BugsAplenty/NeuralAmpModelerCore";
    nam.flake = false;
  };

  outputs = { self, nixpkgs, flake-utils, dpf, nam }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        nativeTools = with pkgs; [ gnumake pkg-config cmake ninja ];
        toolchain   = with pkgs; [ stdenv.cc.cc ];
        eigen        = pkgs.eigen;
        nlohmannJson = pkgs.nlohmann_json;
      in
      {
        devShells.default = pkgs.mkShell {
          packages = nativeTools ++ toolchain ++ [ eigen nlohmannJson ];
          # Export for manual builds only (safe in the shell)
          DPF_PATH = dpf;
          NAM_PATH = nam;
          EIGEN_INCLUDE    = "${eigen}/include/eigen3";
          NLOHMANN_INCLUDE = "${nlohmannJson}/include";
        };

        packages.nam-clap = pkgs.stdenv.mkDerivation {
          pname = "nam-clap";
          version = "0.1.0";
          src = ./.;

          # only what we actually need for make + pkg-config
          nativeBuildInputs = [ pkgs.gnumake pkgs.pkg-config ];
          buildInputs       = [ pkgs.stdenv.cc.cc ];

          # IMPORTANT: prevent CMake auto-configure
          dontConfigure = true;

          buildPhase = ''
            runHook preBuild
            make -C plugins/nam \
              CLAP=true \
              DPF_PATH=${dpf} \
              NAM_PATH=${nam} \
              EXTRA_CFLAGS="-I${pkgs.eigen}/include/eigen3 -I${pkgs.nlohmann_json}/include"
            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall
            mkdir -p $out/lib/clap
            cp -r plugins/nam/bin/NAM.clap $out/lib/clap/
            echo "Installed -> $out/lib/clap/NAM.clap"
            runHook postInstall
          '';

          meta = with pkgs.lib; {
            description = "Neural Amp Modeler (NAM) CLAP plugin via DPF using NAM Core fork";
            license     = licenses.mit;
            platforms   = platforms.linux ++ platforms.darwin;
          };
        };

        packages.default = self.outputs.packages.${system}.nam-clap;
      });
}
