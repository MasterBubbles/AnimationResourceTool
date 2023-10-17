param (
    [string]$userInputFilePath,
    [string]$modName,
    [string]$modDescription
)

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

function CreateModIniFileFromBGYML {
    param (
        [string]$bgymlFilePath
    )

    # Convert BGYML to YAML using $convertExe
    $yamlFilePath = $bgymlFilePath -replace ".bgyml$", ".yaml"
    $convertOutput = & $convertExe to-yaml $bgymlFilePath --output $yamlFilePath 2>&1

    # Check if YAML file was created successfully
    if (-Not (Test-Path $yamlFilePath)) {
        Write-Host "Failed to convert BGYML to YAML"
        exit
    }

    Write-Host "BGYML converted to YAML successfully. Conversion Output: $convertOutput"

    # Load content
    $content = Get-Content $yamlFilePath

    # Filter out unnecessary lines
    $filteredContent = $content | Where-Object {
        $_ -notmatch "AnimationResources" -and
        $_ -notmatch "Player_Animation" -and
        $_ -notmatch "Player_Demo_Animation" -and
        $_ -notmatch "Player_Skin_Animation"
    }

    # Get the next available ID for the new mod .ini file
    $existingInis = Get-ChildItem "$PSScriptRoot\..\Mods" -Filter *.ini
    $newId = ($existingInis.Count + 1).ToString("000")

    # Construct new .ini content
    $iniContent = @("Name=$modName", "Description=$modDescription")
    $iniContent += $filteredContent | ForEach-Object -Begin { $i = 1 } -Process {
        "$i=$($_.Trim())"
        $i++
    }

    # Write to new .ini file
    $iniFilePath = Join-Path "$PSScriptRoot\..\Mods" "$newId.$modName.ini"
    $iniContent | Out-File -FilePath $iniFilePath -Encoding ASCII

    Write-Host "Mod .ini file created successfully!"

    # Rest of the function...

    # You can add any additional processing or functionality here.
}

# Call the CreateModIniFileFromBGYML function with the inputFilePath parameter
CreateModIniFileFromBGYML -bgymlFilePath $userInputFilePath

# Add any further code or functionality here, if needed.
