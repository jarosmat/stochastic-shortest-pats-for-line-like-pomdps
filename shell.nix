{ pkgs ? import <nixpkgs> {} }:

let
  vtk = pkgs.vtk;
  vtkDir = "${vtk}/lib/cmake/vtk";
  utf8cppDir = "${pkgs.utf8cpp}/share/utf8cpp/cmake";
  tbb = pkgs.tbb;
  pugixml = pkgs.pugixml;
  verdict = pkgs.verdict;
  workingDir = builtins.getEnv "PWD";
  tbbCmakeDir = "${workingDir}/tbb-cmake";
in

pkgs.mkShell {
  name = "vtk-env";

  buildInputs = with pkgs; [
    vtk
    utf8cpp
    onetbb
    pugixml
    verdict
    cmake
    gcc
    direnv
  ];

  shellHook = ''
    mkdir -p ${tbbCmakeDir}
    cat > ${tbbCmakeDir}/TBBConfig.cmake <<EOF
    set(TBB_FOUND TRUE)
    set(TBB_INCLUDE_DIRS "${tbb}/include")
    set(TBB_LIBRARIES "${tbb}/lib")
    set(TBB_LIBRARY_DIRS "${tbb}/lib")
    set(TBB_VERSION_STRING "2021.11.0")
    EOF

    cat > ${tbbCmakeDir}/tbb-config.cmake <<EOF
    include("''${CMAKE_CURRENT_LIST_DIR}/TBBConfig.cmake")
    EOF

    export VTK_DIR="${vtkDir}"
    export CMAKE_PREFIX_PATH="${vtk}/lib/cmake/vtk:${utf8cppDir}/share/utf8cpp/cmake:${tbbCmakeDir}:${pugixml}/lib/cmake/pugixml:${verdict}/lib/cmake/verdict:$CMAKE_PREFIX_PATH"
    export utf8cpp_DIR="${utf8cppDir}"
    export TBB_DIR="${tbbCmakeDir}"
    export pugixml_DIR="${pugixml}/lib/cmake/pugixml"
    export Verdict_DIR="${verdict}/lib/cmake/verdict"
  '';
}