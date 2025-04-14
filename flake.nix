{
  description = "DevShell with Clang, CMake, Ninja, and a nixified build for nam-clap";

  outputs = { self, nixpkgs }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
  in {
    devShell.x86_64-linux = pkgs.mkShell {
      buildInputs = [
        pkgs.cmake
        pkgs.llvmPackages.clang
        pkgs.ninja
        pkgs.gtest
      ];
    };

    packages.x86_64-linux = {
      nam-clap = pkgs.stdenv.mkDerivation {
        pname = "nam-clap";
        version = "0.1.0";
        # Instead of using your local checkout, fetch from Git with submodules enabled:
        src = builtins.fetchGit {
          url = "https://github.com/BugsAplenty/nam-clap.git";
          rev = "b334a2a6931a0ed98e6e72e439e365f1212858ed";  # replace with a fixed revision
          submodules = true;
        };
        patches = [
          (pkgs.writeText "fix-install-path.patch" ''
            --- a/CMakeLists.txt
            +++ b/CMakeLists.txt
            @@
            - install(TARGETS nam_clap DESTINATION /homeless-shelter/.clap)
            + install(TARGETS nam_clap DESTINATION lib/clap)
          '')
        ];
        nativeBuildInputs = [
          pkgs.cmake
          pkgs.ninja
          pkgs.llvmPackages.clang
        ];
        buildInputs = [
          pkgs.gtest
        ];
        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=Release"
          "-DCMAKE_INSTALL_PREFIX=$out"
        ];
        meta = with pkgs.lib; {
          description = "A nixified build of nam-clap using CMake";
          license = licenses.mit;
        };
      };
    };
  };
}
