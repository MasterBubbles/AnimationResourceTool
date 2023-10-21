param(
    [string]$modIDs,
    [string]$modPath
)

Write-Host "modIDs value: $modIDs"

# Check for byml-to-yaml.exe existence
$convertExe = Join-Path $PSScriptRoot "byml-to-yaml.exe"

if (-Not (Test-Path $convertExe -PathType Leaf)) {
    Write-Host ""
    Write-Host "byml-to-yaml.exe not found in script directory:"
    Write-Host "$PSScriptRoot"
    Write-Host ""
    Write-Host "Please ensure it is present and try again."
    Write-Host ""
    Write-Host "It can be downloaded from GitHub on this link:"
    Write-Host ""
    Write-Host "https://github.com/ArchLeaders/byml_to_yaml/releases/latest"
    Write-Host ""
    exit 1
}

function Get-ModFromIni($filePath) {
    $iniContent = Get-Content $filePath
    $filename = [System.IO.Path]::GetFileNameWithoutExtension($filePath)

    # Change the regex to match only the numeric ID
    if ($filename -match "^(\d+)$") { # This regex now only checks for numeric filenames (IDs)
        $id = [int]$Matches[1]
    } else {
        Write-Host "File $filename does not have a valid ID. Skipping..."
        return $null
    }

    $modInfo = @{
        Name = ""
        Id = $id
        ModList = @()
    }

    foreach ($line in $iniContent) {
        if ($line -match "^Name=(.*)$") {
            $modInfo.Name = $Matches[1]
        } elseif ($line -match "^\d+=(- \{.*\})$") {
            $modInfo.ModList += $Matches[1]
        }
    }

    return $modInfo
}

function CreatePatch($combination, $modPath) {
    # Define the target directory for the patch
    $patchDir = Join-Path $modPath "A_A_Multiple_Animation_Mods"

    Remove-Item -Path $patchDir -Recurse -ErrorAction SilentlyContinue

    $patchRomfsDirectory = Join-Path $patchDir "romfs\Component\AnimationParam"
    New-Item -ItemType Directory -Path $patchRomfsDirectory -Force | Out-Null
    $patchFile = Join-Path $patchRomfsDirectory "Player.engine__component__AnimationParam.yaml"
    Copy-Item -Path $PSScriptRoot\Resources\romfs -Destination $patchDir -Recurse -ErrorAction SilentlyContinue
    $isRetargetModList = @()
    $nonIsRetargetModList = @()

    foreach ($modId in $combination) {
        $mod = $sortedMods | Where-Object { $_.Id -eq $modId }
        if ($mod -ne $null) {
            $mod.ModList | ForEach-Object {
                if ($_ -match "IsRetarget: true") {
                    $isRetargetModList += $_
                } else {
                    $nonIsRetargetModList += $_
                }
            }
        }
    }

    $patchContent = @()
    $patchContent += "AnimationResources:"
    $patchContent += $isRetargetModList
    $patchContent += "- {ModelProjectName: Player_Animation}"
    $patchContent += "- {ModelProjectName: Player_Demo_Animation}"
    $patchContent += "- {ModelProjectName: Player_Skin_Animation}"
    $patchContent += $nonIsRetargetModList

    $patchContent -join "`r`n" | Set-Content -Path $patchFile

    & $convertExe to-byml $patchFile --output ($patchFile -replace ".yaml$", ".bgyml")
    if ($LASTEXITCODE -eq 0) {
        Remove-Item -Path $patchFile -Force
    }
}


# Define the mods and combinations here
$modFiles = Get-ChildItem -Path "$PSScriptRoot\..\Mods" -Filter "*.ini"
$mods = $modFiles | ForEach-Object { Get-ModFromIni $_.FullName }

# Sort the mods by ID
$sortedMods = $mods | Sort-Object Id

# Split the comma-separated mod IDs into an array and sort them
$selectedModIds = $modIDs -split ',' | ForEach-Object { [int]$_ } | Sort-Object

# Call CreatePatch with $selectedModIds and $modPath
CreatePatch $selectedModIds $modPath

