[CmdletBinding()]
param(
  [ValidateSet("Debug", "Release")]
  [string]$Configuration = "Debug",
  [string]$VcpkgRoot = "",
  [switch]$Clean,
  [int]$Jobs = 8
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$WorkspaceRoot = Split-Path -Parent $RepoRoot
$WindowsDependenciesRoot = Join-Path $WorkspaceRoot "windows-dependencies"
$CommonScript = Join-Path $WindowsDependenciesRoot "vcpkg-script\dali-build-common.ps1"
if(-not (Test-Path -LiteralPath $CommonScript))
{
  throw "windows-dependencies must be beside dali-ui: $WindowsDependenciesRoot"
}
. $CommonScript

$Context = New-DaliBuildContext -WindowsDependenciesRoot $WindowsDependenciesRoot -VcpkgRoot $VcpkgRoot
Initialize-DaliBuildEnvironment -Context $Context
$CorePackage = Join-Path $Context.InstallPrefix "share\dali3-core"
$AdaptorPackage = Join-Path $Context.InstallPrefix "share\dali3-adaptor"
Assert-DaliPaths -Paths @(
  (Join-Path $CorePackage "dali3-core-config.cmake"),
  (Join-Path $AdaptorPackage "dali3-adaptor-config.cmake")
) -Description "prerequisite package; build dali-core and dali-adaptor first"

$Arguments = (Get-DaliCommonCMakeArguments -Context $Context -Configuration $Configuration) + @(
  "-DENABLE_PKG_CONFIGURE=OFF",
  "-DINSTALL_CMAKE_MODULES=ON",
  "-DENABLE_VECTOR_BASED_TEXT_RENDERING=OFF",
  "-Ddali3-core_DIR=$CorePackage",
  "-Ddali3-adaptor_DIR=$AdaptorPackage"
)
Invoke-DaliCMakeProject `
  -Name "dali-ui" `
  -SourceDirectory (Join-Path $RepoRoot "build\tizen") `
  -BuildDirectory (Join-Path $RepoRoot "_build\windows") `
  -ConfigureArguments $Arguments `
  -Clean:$Clean `
  -Jobs $Jobs

Install-DaliRuntimeScripts -Context $Context -Configuration $Configuration
Assert-DaliPaths -Paths @(
  (Join-Path $Context.InstallPrefix "bin\dali3-ui-foundation.dll"),
  (Join-Path $Context.InstallPrefix "bin\dali3-ui-components.dll"),
  (Join-Path $Context.InstallPrefix "share\dali3-ui-foundation\dali3-ui-foundation-config.cmake"),
  (Join-Path $Context.InstallPrefix "share\dali3-ui-components\dali3-ui-components-config.cmake")
) -Description "dali-ui installation output"
Write-Host "`ndali-ui installed in $($Context.InstallPrefix)." -ForegroundColor Green
