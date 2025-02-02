param (
    [string]$sourceDir,  # First parameter: source directory
    [string]$targetDir   # Second parameter: target directory
)

# Check if the source directory exists
if (-Not (Test-Path -Path $sourceDir)) {
    Write-Host "Source directory '$sourceDir' does not exist!"
    exit 1
}

# Check if the target directory exists, if not, create it
if (-Not (Test-Path -Path $targetDir)) {
    Write-Host "Target directory '$targetDir' does not exist, creating it..."
    New-Item -Path $targetDir -ItemType Directory
}

# Get all files from the source directory (recursively)
$allFiles = Get-ChildItem -Path $sourceDir -Recurse -File -Exclude "*.vst3"

# Process each file
foreach ($file in $allFiles) {
    # Use the 'file' command to check the file type
    $fileType = & file $file.FullName

    # If the file type contains "shared object", consider it a dynamic library
    if ($fileType -match "shared object") {
        Write-Host "Copying dynamic library: $($file.FullName)"
        
        # Copy the file to the target directory
        $destination = Join-Path -Path $targetDir -ChildPath $file.Name
        Copy-Item -Path $file.FullName -Destination $destination -Force
    }
    # Check if it's a symbolic link
    elseif ($file.LinkType -eq 'SymbolicLink') {
        Write-Host "Creating symbolic link for: $($file.FullName)"
        
        # Create the symbolic link in the target directory
        $linkDestination = Join-Path -Path $targetDir -ChildPath $file.Name
        New-Item -ItemType SymbolicLink -Path $linkDestination -Target $file.FullName
    }
}

Write-Host "Copy operation completed!"
