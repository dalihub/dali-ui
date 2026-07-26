[CmdletBinding()]
param(
  [ValidateSet("Debug", "Release")]
  [string]$Configuration = "Release",
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
$CorePackage = Join-Path $Context.InstallPrefix "share\dali2-core"
$AdaptorPackage = Join-Path $Context.InstallPrefix "share\dali2-adaptor"
Assert-DaliPaths -Paths @(
  (Join-Path $CorePackage "dali2-core-config.cmake"),
  (Join-Path $AdaptorPackage "dali2-adaptor-config.cmake")
) -Description "prerequisite package; build dali-core and dali-adaptor first"

$Arguments = (Get-DaliCommonCMakeArguments -Context $Context -Configuration $Configuration) + @(
  "-DENABLE_PKG_CONFIGURE=OFF",
  "-DINSTALL_CMAKE_MODULES=ON",
  "-DENABLE_VECTOR_BASED_TEXT_RENDERING=OFF",
  "-Ddali2-core_DIR=$CorePackage",
  "-Ddali2-adaptor_DIR=$AdaptorPackage"
)
Invoke-DaliCMakeProject `
  -Name "dali-ui" `
  -SourceDirectory (Join-Path $RepoRoot "build\tizen") `
  -BuildDirectory (Join-Path $RepoRoot "_build\windows") `
  -ConfigureArguments $Arguments `
  -Clean:$Clean `
  -Jobs $Jobs

Install-DaliRuntimeScripts -Context $Context
Assert-DaliPaths -Paths @(
  (Join-Path $Context.InstallPrefix "bin\dali2-ui-foundation.dll"),
  (Join-Path $Context.InstallPrefix "bin\dali2-ui-components.dll"),
  (Join-Path $Context.InstallPrefix "share\dali2-ui-foundation\dali2-ui-foundation-config.cmake"),
  (Join-Path $Context.InstallPrefix "share\dali2-ui-components\dali2-ui-components-config.cmake")
) -Description "dali-ui installation output"
Write-Host "`ndali-ui installed in $($Context.InstallPrefix)." -ForegroundColor Green
