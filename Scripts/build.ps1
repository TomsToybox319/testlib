If (!(test-path Build))
{
    mkdir Build
}
pushd Build

$CompileOptions = @(
    "/EHsc"  # EH = Exception Handling.
             #   s - Enable stack unwinding
             #   c - Functions declared as extern "C" never throw a C++ exception
             
    "/std:c++23preview" # Compile with C++23 standard
    "/WX"        # Treat warnings as errors
    "/W4"        # Highest warning-level
    "/wd4127"    # Disable "conditional expression is constant" warning

    "/I"         # Include directory
    "..\Src\" 
)

cl ..\Test\test_utest.cpp ..\Src\unittest.cpp $CompileOptions
$ReturnCode = $LastExitCode

popd
exit $ReturnCode
