#!/bin/sh

set -e

VARIANT=debug
# VARIANT=release

detect_git()
{
    if command -v git > /dev/null; then
        return 0
    else
        echo "error: git not found"
        return 1
    fi
}

detect_llvm()
{
    if command -v clang++ > /dev/null; then
        CLANG_VERSION=$(clang++ --version | grep -oE 'version [0-9]+' | cut -d' ' -f2)
        if [ $CLANG_VERSION -lt 19 ]; then
            echo "error: clang++ version is less than 19"
            echo ""
            echo "Try following:"
            echo ""
            echo "    wget https://apt.llvm.org/llvm.sh"
            echo "    chmod +x llvm.sh"
            echo "    sudo ./llvm.sh 20 all"
            echo " "
            echo " and make sure clang++ command points to that version."
            return 1
        fi
    else
        echo "error: clang++ not found"
        return 1
    fi
}

build()
{
    if [ "$VARIANT" = "release" ]; then
        OPTIONS="-O3 -DNDEBUG"
        echo WARNING: OPTIMIZATION=ON. Compilation in Release mode is extremely slow
    else
        OPTIONS="-O0 -g"
        echo WARNING: OPTIMIZATION=OFF, Executable in Debug mode is extremely slow
    fi

    cd "$(dirname "$0")"

    mkdir -p output

    detect_llvm

    if [ -d cut ]; then
        # echo has cut
        :
    else
        detect_git
        git clone https://github.com/segfault-survivor/cut
    fi

    if [ -f cut/cut-clang++ ]; then
        # echo Found cut-clang++
        :
    else
        cd cut
        echo Building Cut! Please wait.
        ./build.sh
        cd ..
    fi

    if [ -f cut/cut-clang++ ]; then
        COMPILER=cut/cut-clang++
    else
        COMPILER=clang++
        echo WARNING: cut-clang++ not found, incremental compilation will be extremely slow
    fi

    $COMPILER -c $OPTIONS -std=c++2c main-linux.cpp      -o output/main-linux.o   `pkg-config --cflags gtkmm-3.0`
    $COMPILER             -std=c++2c output/main-linux.o -o output/main-linux.exe `pkg-config --libs   gtkmm-3.0` -ltbb

    if [ "$1" = "build_only" ]; then
        echo "Complete"
    else
        echo ""
        set +e
        output/main-linux.exe
        printf "\nExit code: %b\n" $?
    fi
}

if [ "$1" = "detect_git" ]; then
    detect_git
elif [ "$1" = "detect_llvm" ]; then
    detect_llvm
else
    build $1
fi
