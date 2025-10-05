{
  description = "Dev shell for building DPF + NAM CLAP (X11/OpenGL/Cairo, etc.)";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        tools = with pkgs; [
          gnumake
          pkg-config
          gcc
          binutils
          gdb
          cmake
          ninja
          python3
          git
        ];
        # GUI + audio libs DPF can use (provide pkg-config .pc files)
        libs = with pkgs; [
          # X11 / OpenGL / GUI stack
          xorg.libX11
          xorg.libXext
          xorg.libXrandr
          xorg.libXcursor
          mesa              # libGL
          mesa_glu          # libGLU (some setups still look for it)
          cairo
          freetype
          fontconfig
          dbus
          SDL2

          # Audio backends
          jack2
          alsa-lib
          pulseaudio
        ];
      in {
        devShells.default = pkgs.mkShell {
          packages = tools ++ libs;

          # Helpful for some build systems; Nix usually sets PKG_CONFIG_PATH automatically,
          # but we print what DPF detects so you get instant feedback.
          shellHook = ''
            echo ">>> Entered NAM/DPF dev shell"
            echo ">>> pkg-config sees:"
            pkg-config --list-all | grep -E '^(x11|xext|xrandr|xcursor|gl|glu|cairo|freetype2|fontconfig|jack|alsa|pulse|sdl2)\b' || true
            if [ -d libs/DPF ]; then
              echo ">>> DPF feature probe:"
              (cd libs/DPF && make -s features || true)
            else
              echo ">>> (Tip) Clone/init submodule: git submodule update --init --recursive libs/DPF"
            fi
          '';
        };
      });
}
