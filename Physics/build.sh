function getParameters() {
    function isInArray() (
        for item in $1; do
            if [ "$item" == "$2" ]; then
                echo 1
                return
            fi
        done
        echo 0
    )
    function validateOption() {
        option_names="$1"
        option_name="$2"
        if [ `isInArray "${option_names[*]}" "${option_name}"` == 0 ]; then
            printHelp
            exit 1
        fi
    }
    function printHelp() (
        echo "Usage: build.sh [options]"
        echo "Options:"
        echo "  -a <x86|x64>"
        echo "  -c <Debug|Release>"
        echo "  -t <All|WSim|Dependencies>"
    )

    # Available option values
    architectures=(x64 x86)
    configurations=(Debug Release)
    targets=(All WSim Dependencies)

    # Set default values
    architecture=${architectures[0]}
    configuration=${configurations[0]}
    target=${targets[0]}

    # Override values
    while getopts "a:c:t:h" opt; do
      case ${opt} in
        a ) validateOption "${architectures[*]}" "$OPTARG"
            architecture="$OPTARG" ;;
        c ) validateOption "${configurations[*]}" "$OPTARG"
            configuration="$OPTARG" ;;
        t ) validateOption "${targets[*]}" "$OPTARG"
            target="$OPTARG" ;;
        h ) printHelp; exit 0 ;;
        \?) printHelp; exit 1 ;;
      esac
    done
}

function build() (
    # Get args
    src_dir=`realpath "$1" -m`
    build_dir=`realpath "$2" -m`
    architecture="$3"
    configuration="$4"
    shift; shift; shift; shift # rest of arguments goes to CMake

    # Go to destination directory
    mkdir -p "$build_dir"
    cd "$build_dir"

    # Run CMake
    echo "Building \"$src_dir\" in \"$build_dir\""
    cmake "$src_dir" -A $architecture -DCMAKE_BUILD_TYPE=$configuration -DCMAKE_CONFIGURATION_TYPES=$configuration $@
    if [ $? != 0 ]; then
        exit 1
    fi
    echo
)

# Initialize
getParameters $@
git submodule update --init --recursive
build_path=`realpath .build -m`/"$architecture"_"$configuration"

# Build and compile dependencies
if [ $target == "Dependencies" -o $target == "All" ]; then
    # Gtest
    build "ThirdParty/googletest" "$build_path/googletest" $architecture $configuration \
        -Dgtest_force_shared_crt=OFF                                                     \
        -DBUILD_GMOCK=OFF
    cmake --build "$build_path/googletest" --target gtest --config $configuration
fi

# Build WSim
if [ $target == "WSim" -o $target == "All" ];  then
    if [ $configuration == 'Debug' ]; then debug_suffix='d'; fi
    build "." "$build_path/WSim" $architecture $configuration                    \
        -DGTEST_INCLUDE_PATH=`realpath ThirdParty/googletest/googletest/include` \
        -DGTEST_LIBRARY_PATH="$build_path/googletest/lib/$configuration/gtest$debug_suffix.lib"
fi
