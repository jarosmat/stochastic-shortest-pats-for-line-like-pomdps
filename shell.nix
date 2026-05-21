{ pkgs ? import <nixpkgs> {} }:

let
  vtk = pkgs.vtk;
  vtkDir = "${vtk}/lib/cmake/vtk";
  utf8cppDir = "${pkgs.utf8cpp}/share/utf8cpp/cmake";
  tbb = pkgs.tbb;
  pugixml = pkgs.pugixml;
  verdict = pkgs.verdict;
  workingDir = builtins.getEnv "PWD";
  cmakeDir = "${workingDir}/nix-cmake-config";
  nlohmann_json = pkgs.nlohmann_json;
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
    mkdir -p ${cmakeDir}
    cat > ${cmakeDir}/TBBConfig.cmake <<EOF
    set(TBB_FOUND TRUE)
    # set(TBB_INCLUDE_DIRS "${tbb}/include")
    set(TBB_LIBRARIES "${tbb}/lib")
    set(TBB_LIBRARY_DIRS "${tbb}/lib")
    set(TBB_VERSION_STRING "2021.11.0")

    add_library(TBB::tbb UNKNOWN IMPORTED)
    set_target_properties(TBB::tbb PROPERTIES
      IMPORTED_LOCATION "${tbb}/lib"
      # INTERFACE_INCLUDE_DIRECTORIES "${tbb}/include"
    )
    EOF

    cat > ${cmakeDir}/nlohmann_jsonConfig.cmake <<EOF
    # nlohmann_jsonConfig.cmake for Nix
    set(nlohmann_json_FOUND TRUE)
    set(nlohmann_json_INCLUDE_DIRS "${nlohmann_json}/include")

    # Create the INTERFACE target VTK expects: nlohmann_json::nlohmann_json
    add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
    set_target_properties(nlohmann_json::nlohmann_json PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${nlohmann_json}/include"
    )
    EOF

    cat > ${cmakeDir}/tbb-config.cmake <<EOF
    include("''${CMAKE_CURRENT_LIST_DIR}/TBBConfig.cmake")
    EOF

    export VTK_DIR="${vtkDir}"
    export CMAKE_PREFIX_PATH="${vtk}/lib/cmake/vtk:${utf8cppDir}/share/utf8cpp/cmake:${cmakeDir}:${pugixml}/lib/cmake/pugixml:${verdict}/lib/cmake/verdict:$CMAKE_PREFIX_PATH"
    export utf8cpp_DIR="${utf8cppDir}"
    export TBB_DIR="${cmakeDir}"
    export pugixml_DIR="${pugixml}/lib/cmake/pugixml"
    export Verdict_DIR="${verdict}/lib/cmake/verdict"
  '';
}
