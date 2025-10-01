{
  mkShell,
  platformio,
}:
mkShell {
  name = "v2";
  packages = [
    platformio
  ];
}
