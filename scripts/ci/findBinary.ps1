param (
    [string]$DirectoryPath
)

# Check if the directory path is provided and exists
if (-not (Test-Path -Path $DirectoryPath)) {
    exit
}

# Get all .exe .dll and .vst3 files recursively from the specified directory
$files = Get-ChildItem -Path $DirectoryPath -Recurse -Include *.exe, *.dll, *.vst3

# Format the output
$output = $files | ForEach-Object {
    '"' + $_.FullName + '"'
} 

# Join the file names with a comma separator
$joinedOutput = $output -join ","

# Print the output
Write-Output $joinedOutput
