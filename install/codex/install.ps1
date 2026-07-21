param(
    [string]$Destination = "",
    [switch]$CoreOnly,
    [switch]$NoGovernance,
    [switch]$NoCompanions
)

$ErrorActionPreference = "Stop"

$packageRoot = Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..\..")
if ([string]::IsNullOrWhiteSpace($Destination)) {
    if (-not [string]::IsNullOrWhiteSpace($env:CODEX_HOME)) {
        $Destination = Join-Path $env:CODEX_HOME "skills"
    } else {
        $Destination = Join-Path $HOME ".codex\skills"
    }
}

New-Item -ItemType Directory -Force -Path $Destination | Out-Null

function Copy-SkillGroup {
    param(
        [Parameter(Mandatory = $true)][string]$GroupPath
    )

    if (-not (Test-Path -LiteralPath $GroupPath)) {
        throw "Missing skill group: $GroupPath"
    }

    Get-ChildItem -LiteralPath $GroupPath -Directory | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination $Destination -Recurse -Force
        Write-Host "Installed $($_.Name) -> $Destination"
    }
}

Copy-SkillGroup -GroupPath (Join-Path $packageRoot "skills\core")
if (-not $CoreOnly) {
    if (-not $NoGovernance) {
        Copy-SkillGroup -GroupPath (Join-Path $packageRoot "skills\governance")
    }
    if (-not $NoCompanions) {
        Copy-SkillGroup -GroupPath (Join-Path $packageRoot "skills\companions")
    }
}

Write-Host "Codex skills installed to $Destination"
