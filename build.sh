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
        echo "  -a <x86|x64>        - select architecture (default: x64)"
        echo "  -c <Debug|Release>  - select configuration (default: Debug)"
        echo "  -d                  - do not compile dependencies automatically"
        echo "  -w                  - do not compile WSim automatically"
        echo "  -p                  - production build (seek for shaders in ./Kernels)"
    )

    # Available option values
    architectures=(x64 x86)
    configurations=(Debug Release)

    # Set default values
    architecture=${architectures[0]}
    configuration=${configurations[0]}
    build_dependencies=1
    build_wsim=1
    update_dependencies=1
    production_build=0

    # Override values
    while getopts "a:c:dwhp" opt; do
      case ${opt} in
        a ) validateOption "${architectures[*]}" "$OPTARG"
            architecture="$OPTARG" ;;
        c ) validateOption "${configurations[*]}" "$OPTARG"
            configuration="$OPTARG" ;;
        d ) build_dependencies=0 ;;
        w ) build_wsim=0 ;;
        p ) production_build=1 ;;
        h ) printHelp; exit 0 ;;
        \?) printHelp; exit 1 ;;
      esac
    done
}

function run_cmake() (
    # Get args
    src_dir=`realpath "$1" -m`
    build_dir=`realpath "$2" -m`
    architecture="$3"
    configuration="$4"
    shift; shift; shift; shift # rest of arguments goes to CMake

    # Go to destination directory
    mkdir -p "$build_dir"
    cd "$build_dir"

    # Do not set architecture flag on Linux
    is_unix=`uname -a | grep -E "Linux|Unix" | wc -l`
    if [ $is_unix == 0 ]; then architecture_flag="-A x64 -T host=x64"; fi

    # Run CMake
    echo "Running CMake for \"$src_dir\" in \"$build_dir\""
    cmake "$src_dir" $architecture_flag -DWSIM_PRODUCTION=$production_build -DCMAKE_BUILD_TYPE=$configuration -DCMAKE_CONFIGURATION_TYPES=$configuration $@
    if [ $? != 0 ]; then
        exit 1
    fi
    echo
)

function compile() (
    target_name="$1"
    cmake --build "$build_path" --target $target_name --config $configuration
)

# Initialize
getParameters $@
build_path=`realpath .build -m`/"$architecture"_"$configuration"

# Build
run_cmake "." "$build_path" $architecture $configuration
if [ $? != 0 ]; then
    exit
fi
if [ "$build_dependencies" == 1 ]; then
    compile gtest
    compile glfw
    compile glad
    compile glm_static
fi
if [ "$build_wsim" == 1 ]; then
    compile WSimStandalone
    if [ -e run_mpi.sh ]; then
        compile WSimMPI
    fi
    compile WSimRendererExe
fi

# CMake created run.sh script, set execute permissions
chmod +x run_standalone.sh  2>/dev/null
chmod +x run_mpi.sh         2>/dev/null
chmod +x run_renderer.sh    2>/dev/null
