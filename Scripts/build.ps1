If (!(test-path Build))
{
    mkdir Build
}
pushd Build

$CompileOptions = @(
    "/EHsc"  # EH = Exception Handling.
             #   s - Enable stack unwinding
             #   c - Functions declared as extern "C" never throw a C++ exception
             
    "/std:c++20" # Compile with C++20 standard
    "/WX"        # Treat warnings as errors
    "/W4"        # Highest warning-level
)

cl ..\Src\main.cpp $CompileOptions
$ReturnCode = $LastExitCode

popd
exit $ReturnCode
