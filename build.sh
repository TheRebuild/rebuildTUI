#!/bin/bash

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}"
echo "rebuildTUI [Build]"
echo "===================================="
echo -e "${NC}"

if ! command -v cmake &> /dev/null; then
    echo -e "${RED}❌ Error: cmake is not installed or not in PATH${NC}"
    echo "Please install cmake and try again."
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_DIR/build"

BUILD_TYPE="Release"
CLEAN_BUILD=false
BUILD_EXAMPLES=true
BUILD_LIBRARY=true
BUILD_INSTALL=false
BUILD_EXECUTABLE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -e|--examples)
            BUILD_EXAMPLES=true
            shift
            ;;
        --no-examples)
            BUILD_EXAMPLES=false
            shift
            ;;
        -bi|--build-install)
            BUILD_INSTALL=true
            shift
            ;;
        -l|--library)
            BUILD_LIBRARY=true
            shift
            ;;
        --no-library)
            BUILD_LIBRARY=false
            shift
            ;;
        --executable)
            BUILD_EXECUTABLE=true
            shift
            ;;
        --no-executable)
            BUILD_EXECUTABLE=false
            shift
            ;;
        --all)
            BUILD_EXAMPLES=true
            BUILD_LIBRARY=true
            BUILD_EXECUTABLE=true
            BUILD_INSTALL=true
            shift
            ;;
        --minimal)
            BUILD_EXAMPLES=false
            BUILD_LIBRARY=true
            BUILD_EXECUTABLE=false
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Build Type Options:"
            echo "  -d, --debug         Build in Debug mode (default: Release)"
            echo "  -c, --clean         Clean build directory before building"
            echo ""
            echo "Component Options:"
            echo "  -e, --examples      Build examples (default: ON)"
            echo "  --no-examples       Don't build examples"
            echo "  -l, --library       Build static library (default: ON)"
            echo "  --no-library        Don't build static library"
            echo "  --executable        Build main executable (default: OFF)"
            echo "  --no-executable     Don't build main executable"
            echo "  -bi, --build-install Build and install the library (default: OFF)"
            echo ""
            echo "Preset Options:"
            echo "  --all               Build everything (library + examples + executable)"
            echo "  --minimal           Build only library (no examples, no executable)"
            echo ""
            echo "Examples:"
            echo "  $0                      # Build library + examples (default)"
            echo "  $0 --all               # Build everything"
            echo "  $0 --minimal           # Build only library"
            echo "  $0 -d --executable     # Debug build with main executable"
            echo "  $0 --no-examples -l    # Build only library, no examples"
            echo ""
            echo "  -h, --help          Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}❌ Unknown option: $1${NC}"
            echo "Use -h or --help for usage information."
            exit 1
            ;;
    esac
done

echo -e "${BLUE}🔧 Build Configuration:${NC}"
echo "   Build Type: $BUILD_TYPE"
echo "   Clean Build: $CLEAN_BUILD"
echo "   Build Library: $BUILD_LIBRARY"
echo "   Build Examples: $BUILD_EXAMPLES"
echo "   Build Executable: $BUILD_EXECUTABLE"
echo "   Build Install: $BUILD_INSTALL"
echo ""

if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${YELLOW}🧹 Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

echo -e "${BLUE}📁 Creating build directory...${NC}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if [ "$BUILD_EXAMPLES" = true ]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_EXAMPLES=ON"
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_EXAMPLES=OFF"
fi

if [ "$BUILD_LIBRARY" = true ]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_LIBRARY=ON"
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_LIBRARY=OFF"
fi

if [ "$BUILD_EXECUTABLE" = true ]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_EXECUTABLE=ON"
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_EXECUTABLE=OFF"
fi

if [ "$BUILD_INSTALL" = true ]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DINSTALL_REBUILDTUI=ON"
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DINSTALL_REBUILDTUI=OFF"
fi

echo -e "${BLUE}⚙️  Configuring project with CMake...${NC}"
cmake $CMAKE_OPTIONS "$PROJECT_DIR"

echo -e "${BLUE}🔨 Building project...${NC}"
cmake --build . --config $BUILD_TYPE

if [ $? -eq 0 ]; then
    echo -e "${GREEN}"
    echo "✅ Build completed successfully!"
    echo "================================"
    echo -e "${NC}"

    echo ""
    echo -e "${CYAN}Build Results:${NC}"

    if [ "$BUILD_LIBRARY" = true ] && [ -f "./lib/libtui_lib.a" ]; then
        echo "   📚 Library: ./lib/libtui_lib.a"
    fi

    if [ "$BUILD_EXECUTABLE" = true ] && [ -f "./bin/custom_tui" ]; then
        echo "   🎯 Executable: ./bin/custom_tui"
    fi

    if [ "$BUILD_EXAMPLES" = true ]; then
        echo "   📋 Examples:"
        for exe in ./bin/*; do
            if [ -f "$exe" ] && [ -x "$exe" ] && [[ $(basename "$exe") != "custom_tui" ]]; then
                echo "       ./bin/$(basename "$exe")"
            fi
        done
    fi
    echo ""
else
    echo -e "${RED}"
    echo "❌ Build failed!"
    echo "==============="
    echo -e "${NC}"
    echo "Please check the error messages above and try again."
    exit 1
fi
    