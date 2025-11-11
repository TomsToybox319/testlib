.\Build\main.exe
$returnCode = $LastExitCode
if ($returnCode -eq 0) {
    Write-Host "Test passed!"
}
else {
    Write-Host "Test failed with error code" $returnCode
}
exit $returnCode
