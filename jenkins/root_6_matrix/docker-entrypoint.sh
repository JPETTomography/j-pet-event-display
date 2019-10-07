#!/bin/bash

function executeCommand {
    $@
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    echo "Exit code[" $@ "]: $rc"
}

executeCommand "mkdir -p build"
executeCommand "cd build"
executeCommand "export CMAKE_LIBRARY_PATH=$CMAKE_LIBRARY_PATH:/framework-dependencies/lib"
executeCommand "export CMAKE_INCLUDE_PATH=$CMAKE_INCLUDE_PATH:/framework-dependencies/include"
executeCommand "source /root-system/bin/thisroot.sh"

executeCommand "rm -rf j-pet-framework || true"
executeCommand "git clone --single-branch --branch master https://github.com/JPETTomography/j-pet-framework.git"
executeCommand "mkdir -p j-pet-framework/build"
executeCommand "cd j-pet-framework/build"
executeCommand "cmake .."
executeCommand "cmake --build ."
executeCommand "sudo make install"
executeCommand "cd ../.."

executeCommand "cmake .."
executeCommand "make"
