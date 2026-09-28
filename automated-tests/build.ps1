[CmdletBinding()]
param(
  [ValidateSet("Debug", "Release")]
  [string]$Configuration = "Debug",
  [string[]]$Modules = @(),
  [string]$VcpkgRoot = "",
  [switch]$Clean,
  [int]$Jobs = 8
)

$ErrorActionPreference = "Stop"
$TestRoot = $PSScriptRoot
$RepoRoot = Split-Path -Parent $TestRoot
$WorkspaceRoot = Split-Path -Parent $RepoRoot
$WindowsDependenciesRoot = Join-Path $WorkspaceRoot "windows-dependencies"
$CommonScript = Join-Path $WindowsDependenciesRoot "vcpkg-script\dali-build-common.ps1"
$SupportedModules = @("dali-ui-foundation", "dali-ui-foundation-internal", "dali-ui-components")

if($Modules.Count -gt 0)
{
  $UnsupportedModules = @($Modules | Where-Object { $SupportedModules -notcontains $_ })
  if($UnsupportedModules.Count -gt 0)
  {
    throw "Unsupported Windows test module: $($UnsupportedModules -join ', '). Supported modules: $($SupportedModules -join ', ')"
  }
}

if($Modules.Count -eq 0)
{
  $Modules = @("dali-ui-foundation-internal")
}

if(-not (Test-Path -LiteralPath $CommonScript))
{
  throw "windows-dependencies must be beside dali-ui: $WindowsDependenciesRoot"
}
. $CommonScript

$Context = New-DaliBuildContext `
  -WindowsDependenciesRoot $WindowsDependenciesRoot `
  -VcpkgRoot $VcpkgRoot
Initialize-DaliBuildEnvironment -Context $Context

$CorePackage = Join-Path $Context.InstallPrefix "share\dali2-core"
$AdaptorPackage = Join-Path $Context.InstallPrefix "share\dali2-adaptor"
$FoundationPackage = Join-Path $Context.InstallPrefix "share\dali2-ui-foundation"
$ComponentsPackage = Join-Path $Context.InstallPrefix "share\dali2-ui-components"
Assert-DaliPaths -Paths @(
  (Join-Path $CorePackage "dali2-core-config.cmake"),
  (Join-Path $AdaptorPackage "dali2-adaptor-config.cmake"),
  (Join-Path $FoundationPackage "dali2-ui-foundation-config.cmake"),
  (Join-Path $ComponentsPackage "dali2-ui-components-config.cmake")
) -Description "DALi package; build dali-core, dali-adaptor, and dali-ui first"

$Arguments = (Get-DaliCommonCMakeArguments -Context $Context -Configuration $Configuration) + @(
  "-Ddali2-core_DIR=$CorePackage",
  "-Ddali2-adaptor_DIR=$AdaptorPackage",
  "-Ddali2-ui-foundation_DIR=$FoundationPackage",
  "-Ddali2-ui-components_DIR=$ComponentsPackage",
  "-DDALI_UI_WINDOWS_TEST_MODULES=$($Modules -join ';')"
)

Invoke-DaliCMakeProject `
  -Name "dali-ui UTCs on Windows" `
  -SourceDirectory (Join-Path $TestRoot "windows-common") `
  -BuildDirectory (Join-Path $TestRoot "_build\windows") `
  -ConfigureArguments $Arguments `
  -Clean:$Clean `
  -Jobs $Jobs

Install-DaliRuntimeScripts -Context $Context -Configuration $Configuration
$TestRunners = @(
  [pscustomobject]@{ Module = "dali-ui-foundation"; Executable = Join-Path $Context.InstallPrefix "bin\tct-dali-ui-foundation-core.exe" }
  [pscustomobject]@{ Module = "dali-ui-foundation-internal"; Executable = Join-Path $Context.InstallPrefix "bin\tct-dali-ui-foundation-internal-core.exe" }
  [pscustomobject]@{ Module = "dali-ui-components"; Executable = Join-Path $Context.InstallPrefix "bin\tct-dali-ui-components-core.exe" }
)
$TestExecutables = @($TestRunners | Where-Object { $Modules -contains $_.Module } | ForEach-Object { $_.Executable })
Assert-DaliPaths -Paths $TestExecutables -Description "Windows test executables"

Write-Host "`nDALi UI UTCs installed for Windows: $($Modules -join ', ')" -ForegroundColor Green
Write-Host "Run from the workspace root: .\dali-ui\automated-tests\execute.ps1"
