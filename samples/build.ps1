[CmdletBinding()]
param(
  [ValidateSet("Debug", "Release")]
  [string]$Configuration = "Debug",
  [string[]]$Samples = @(),
  [string[]]$ImageViewTargets = @(),
  [string]$VcpkgRoot = "",
  [switch]$Clean,
  [int]$Jobs = 8
)

$ErrorActionPreference = "Stop"
$SampleRoot = $PSScriptRoot
$UiRoot = Split-Path -Parent $SampleRoot
$WorkspaceRoot = Split-Path -Parent $UiRoot
$WindowsDependenciesRoot = Join-Path $WorkspaceRoot "windows-dependencies"
$CommonScript = Join-Path $WindowsDependenciesRoot "vcpkg-script\dali-build-common.ps1"
if(-not (Test-Path -LiteralPath $CommonScript))
{
  throw "windows-dependencies must be beside dali-ui: $WindowsDependenciesRoot"
}
. $CommonScript

$Context = New-DaliBuildContext -WindowsDependenciesRoot $WindowsDependenciesRoot -VcpkgRoot $VcpkgRoot
Initialize-DaliBuildEnvironment -Context $Context
if($Samples.Count -eq 0)
{
  $Samples = @(
    Get-ChildItem -LiteralPath $SampleRoot -Directory |
      Where-Object { Test-Path -LiteralPath (Join-Path $_.FullName "CMakeLists.txt") } |
      Sort-Object Name |
      Select-Object -ExpandProperty Name
  )
  if($Samples.Count -eq 0) { throw "No sample CMake projects were found in $SampleRoot." }
  Write-Host "Building all $($Samples.Count) sample directories." -ForegroundColor Cyan
}

$CorePackage = Join-Path $Context.InstallPrefix "share\dali2-core"
$AdaptorPackage = Join-Path $Context.InstallPrefix "share\dali2-adaptor"
$FoundationPackage = Join-Path $Context.InstallPrefix "share\dali2-ui-foundation"
$ComponentsPackage = Join-Path $Context.InstallPrefix "share\dali2-ui-components"
Assert-DaliPaths -Paths @(
  (Join-Path $CorePackage "dali2-core-config.cmake"),
  (Join-Path $AdaptorPackage "dali2-adaptor-config.cmake"),
  (Join-Path $FoundationPackage "dali2-ui-foundation-config.cmake"),
  (Join-Path $ComponentsPackage "dali2-ui-components-config.cmake")
) -Description "DALi package; build core, adaptor, and UI first"

$Arguments = (Get-DaliCommonCMakeArguments -Context $Context -Configuration $Configuration) + @(
  "-DDALI_UI_SAMPLE_LIST=$($Samples -join ';')",
  "-Ddali2-core_DIR=$CorePackage",
  "-Ddali2-adaptor_DIR=$AdaptorPackage",
  "-Ddali2-ui-foundation_DIR=$FoundationPackage",
  "-Ddali2-ui-components_DIR=$ComponentsPackage"
)
if($ImageViewTargets.Count -gt 0)
{
  $Arguments += "-DIMAGE_VIEW_SAMPLE_LIST=$($ImageViewTargets -join ';')"
}
Invoke-DaliCMakeProject `
  -Name "dali-ui samples" `
  -SourceDirectory $SampleRoot `
  -BuildDirectory (Join-Path $SampleRoot "_build\windows") `
  -ConfigureArguments $Arguments `
  -Clean:$Clean `
  -Jobs $Jobs

Install-DaliRuntimeScripts -Context $Context -Configuration $Configuration
Write-Host "`nSamples installed in $($Context.InstallPrefix)\bin." -ForegroundColor Green
