{
  description = "DevShell with Clang, CMake, and Ninja";

  outputs = { self, nixpkgs }: {
    devShell.x86_64-linux =
      let
        pkgs = nixpkgs.legacyPackages.x86_64-linux;
      in pkgs.mkShell {
        buildInputs = [
          pkgs.cmake
          pkgs.llvmPackages.clang
          pkgs.ninja
        ];
      };
  };
}
