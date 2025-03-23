{
  description = "DevShell with Clang";

  outputs = { self, nixpkgs }: {
    devShell.x86_64-linux = nixpkgs.lib.mkDevShell {
      packages = [
        nixpkgs.pkgs.cmake
        nixpkgs.pkgs.llvmPackages.clang
        nixpkgs.pkgs.ninja
      ];
    };
  };
}

