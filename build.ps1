# Windows build script for Numberwang Pro VST3
# Builds Release and installs VST3 to C:\Program Files\Common Files\VST3
# Run from the repo root: .\build.ps1
# Dev mode (hot-reload UI): .\build.ps1 -Dev

param(
    [switch]$Dev,
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

# Bump patch version in CMakeLists.txt
$cmakeFile = Join-Path $PSScriptRoot "CMakeLists.txt"
$content = Get-Content $cmakeFile -Raw
if ($content -match 'project\(Numberwang VERSION (\d+)\.(\d+)\.(\d+)\)') {
    $major = $Matches[1]; $minor = $Matches[2]; $patch = [int]$Matches[3] + 1
    $newVersion = "$major.$minor.$patch"
    $content = $content -replace 'project\(Numberwang VERSION \d+\.\d+\.\d+\)', "project(Numberwang VERSION $newVersion)"
    Set-Content $cmakeFile $content -NoNewline
    Write-Host "Version bumped to $newVersion"

    # Append to NOTES.md
    $date = Get-Date -Format "yyyy-MM-dd"
    Add-Content (Join-Path $PSScriptRoot "NOTES.md") "`n## $newVersion ($date)`n- Windows build"
}

if ($Clean -and (Test-Path "build")) {
    Remove-Item -Recurse -Force "build"
    Write-Host "Cleaned build directory"
}

$devUI = if ($Dev) { "-DNUMBERWANG_DEV_UI=ON" } else { "-DNUMBERWANG_DEV_UI=OFF" }

cmake -B build -G "Visual Studio 16 2019" -A x64 $devUI
if ($LASTEXITCODE -ne 0) { Write-Error "CMake configure failed"; exit 1 }

cmake --build build --config Release --parallel
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

Write-Host ""
Write-Host "Build complete. VST3 installed to: C:\Program Files\Common Files\VST3\Numberwang Pro.vst3"
if ($Dev) {
    Write-Host "Dev mode ON — UI files served from Source/UI/ on disk (hot-reload via standalone restart)"
    $exe = Join-Path $PSScriptRoot "build\Numberwang_artefacts\Release\Standalone\Numberwang Pro.exe"
    if (Test-Path $exe) {
        Write-Host "Launching standalone: $exe"
        Start-Process $exe
    }
}
