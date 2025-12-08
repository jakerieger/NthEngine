#!/bin/bash
# NthEngine Build and Install Script
# This script automates the process of building and installing NthEngine

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
INSTALL_PREFIX=""
BUILD_DIR="build"
CLEAN_BUILD=false
CREATE_PACKAGE=false
PACKAGE_TYPE=""
COMPONENTS="all"

# Function to print colored messages
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to display usage
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Build and install NthEngine game engine.

Options:
    -h, --help              Show this help message
    -c, --clean             Clean build directory before building
    -b, --build-type TYPE   Build type: Debug, Release, RelWithDebInfo (default: Release)
    -p, --prefix PATH       Installation prefix (default: system default)
    -d, --build-dir DIR     Build directory (default: build)
    -i, --install           Install after building
    -u, --uninstall         Uninstall from previous installation
    -C, --component COMP    Install specific component: Runtime, Development, Tools, all (default: all)
    -P, --package TYPE      Create package: DEB, RPM, NSIS, TGZ (requires CPack)
    
Examples:
    # Build in release mode
    $0 -b Release
    
    # Build and install to /opt/NthEngine
    $0 -b Release -p /opt/NthEngine -i
    
    # Clean build and install
    $0 -c -b Release -i
    
    # Install only runtime component
    $0 -i -C Runtime
    
    # Create a Debian package
    $0 -P DEB
    
    # Uninstall
    $0 -u

EOF
    exit 0
}

# Function to check if running as root (for system installation)
check_root() {
    if [ "$INSTALL_PREFIX" = "" ] && [ "$EUID" -ne 0 ]; then
        print_warning "Installing to system directories requires root privileges"
        print_info "Run with 'sudo' or specify a custom prefix with -p"
        return 1
    fi
    return 0
}

# Parse command line arguments
INSTALL=false
UNINSTALL=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            usage
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -b|--build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -p|--prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        -d|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -i|--install)
            INSTALL=true
            shift
            ;;
        -u|--uninstall)
            UNINSTALL=true
            shift
            ;;
        -C|--component)
            COMPONENTS="$2"
            shift 2
            ;;
        -P|--package)
            CREATE_PACKAGE=true
            PACKAGE_TYPE="$2"
            shift 2
            ;;
        *)
            print_error "Unknown option: $1"
            usage
            ;;
    esac
done

# Handle uninstall
if [ "$UNINSTALL" = true ]; then
    print_info "Uninstalling NthEngine..."
    
    if [ ! -f "$BUILD_DIR/install_manifest.txt" ]; then
        print_error "install_manifest.txt not found in $BUILD_DIR"
        print_error "Cannot proceed with uninstallation"
        exit 1
    fi
    
    if ! check_root; then
        exit 1
    fi
    
    while IFS= read -r file; do
        if [ -f "$file" ] || [ -L "$file" ]; then
            print_info "Removing: $file"
            rm -f "$file"
        fi
    done < "$BUILD_DIR/install_manifest.txt"
    
    print_info "Uninstallation complete"
    exit 0
fi

# Validate build type
case $BUILD_TYPE in
    Debug|Release|RelWithDebInfo|MinSizeRel)
        ;;
    *)
        print_error "Invalid build type: $BUILD_TYPE"
        print_error "Valid types: Debug, Release, RelWithDebInfo, MinSizeRel"
        exit 1
        ;;
esac

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    print_info "Cleaning build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
print_info "Creating build directory: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
print_info "Configuring with CMake (Build Type: $BUILD_TYPE)..."
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if [ -n "$INSTALL_PREFIX" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
    print_info "Install prefix: $INSTALL_PREFIX"
else
    print_info "Install prefix: system default"
fi

cmake .. $CMAKE_ARGS

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed"
    exit 1
fi

# Build
print_info "Building NthEngine..."
cmake --build . --config $BUILD_TYPE -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

if [ $? -ne 0 ]; then
    print_error "Build failed"
    exit 1
fi

print_info "Build completed successfully"

# Create package if requested
if [ "$CREATE_PACKAGE" = true ]; then
    print_info "Creating $PACKAGE_TYPE package..."
    cpack -G $PACKAGE_TYPE
    
    if [ $? -ne 0 ]; then
        print_error "Package creation failed"
        exit 1
    fi
    
    print_info "Package created successfully"
    ls -lh *.deb *.rpm *.tar.gz *.zip 2>/dev/null || true
fi

# Install if requested
if [ "$INSTALL" = true ]; then
    print_info "Installing NthEngine..."
    
    if ! check_root; then
        exit 1
    fi
    
    INSTALL_ARGS="--config $BUILD_TYPE"
    
    if [ "$COMPONENTS" != "all" ]; then
        INSTALL_ARGS="$INSTALL_ARGS --component $COMPONENTS"
        print_info "Installing component: $COMPONENTS"
    fi
    
    cmake --install . $INSTALL_ARGS
    
    if [ $? -ne 0 ]; then
        print_error "Installation failed"
        exit 1
    fi
    
    print_info "Installation completed successfully"
    
    # Run ldconfig on Linux
    if [ "$(uname)" = "Linux" ] && [ "$INSTALL_PREFIX" = "" ]; then
        print_info "Updating shared library cache..."
        ldconfig
    fi
fi

print_info "All operations completed successfully!"
print_info ""
print_info "Next steps:"

if [ "$INSTALL" = true ]; then
    echo "  - The 'nth' tool should now be in your PATH"
    echo "  - Use 'nth --help' to see available commands"
    echo "  - Include NthEngine in your CMake projects with: find_package(NthEngine REQUIRED)"
else
    echo "  - Run with -i flag to install: $0 -i"
    echo "  - Or manually install: cd $BUILD_DIR && sudo cmake --install ."
fi

cd ..
