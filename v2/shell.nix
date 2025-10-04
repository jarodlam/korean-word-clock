{
  mkShell,
  clang-tools,
  platformio,
}:
mkShell {
  name = "v2";
  packages = [
    clang-tools
    platformio
  ];
}
