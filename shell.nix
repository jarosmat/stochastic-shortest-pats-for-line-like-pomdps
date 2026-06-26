with import <nixpkgs> {};
mkShell {
  packages = [
    python3
    python3Packages.matplotlib
    python3Packages.numpy
  ];
  shellHook = ''
    export PYTHONPATH=${python3Packages.matplotlib}/lib/python3.13/site-packages:$PYTHONPATH
    export NUMPY_INCLUDE_PATH=${python3Packages.numpy}/lib/python3.13/site-packages/numpy/core/include
  '';
}
