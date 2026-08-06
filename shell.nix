with import <nixpkgs> {};
mkShell {
  packages = [
    python3
    python3Packages.matplotlib
    python3Packages.numpy
    python3Packages.pybind11
    python3Packages.blessed
  ];
  shellHook = ''
    export PYTHONPATH=${python3Packages.matplotlib}/lib/python3.13/site-packages:$PYTHONPATH
    export NUMPY_INCLUDE_PATH=${python3Packages.numpy}/lib/python3.13/site-packages/numpy/core/include
    export PYTHONPATH=${python3Packages.pybind11}/lib/python3.13/site-packages:$PYTHONPATH
    export PYTHONPATH=$PWD/src/build:$PYTHONPATH
    export PYTHONPATH=$PWD/src/cmake-build-debug:$PYTHONPATH
    echo "Python interpreter path: $(which python)"
  '';
}
