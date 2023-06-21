$directoryName = "luapower-all-master"
$currentDirectory = (Get-Location).Path
$targetDirectory = $currentDirectory

if (-not (Test-Path -Path $currentDirectory\$directoryName -PathType Container)) {
    $downloadUrl = "https://github.com/luapower/all/archive/master.zip"
    $zipFilePath = Join-Path -Path $currentDirectory -ChildPath "master.zip"

    Write-Host "Downloading $downloadUrl..."
    Invoke-WebRequest -Uri $downloadUrl -OutFile $zipFilePath

    Write-Host "Unzipping $zipFilePath to $currentDirectory..."
    Expand-Archive -Path $zipFilePath -DestinationPath $currentDirectory -Force

    # # Rename the extracted directory to 'luapower-all-master'
    # $extractedDirectory = Get-ChildItem -Path $targetDirectory -Directory | Select-Object -First 1
    # Rename-Item -Path $extractedDirectory.FullName -NewName $directoryName -Force

    Write-Host "Cleaning up..."
    Remove-Item -Path $zipFilePath -Force
}

Write-Host "Script execution completed."
