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

# Symbol link
function Resolve-Symlink {
    param (
        [string]$Path
    )

    $currentPath = $Path
    while ($true) {
        $item = Get-Item -Path $currentPath -Force
        if ($item.Attributes -band [System.IO.FileAttributes]::ReparsePoint) {
            $currentPath = Join-Path -Path $item.DirectoryName -ChildPath $item.Target
        } else {
            return $currentPath
        }
    }
}

# Process each file
foreach ($file in $allFiles) {
    # Copy destination
    $destination = Join-Path -Path $targetDir -ChildPath $file.Name

    # Use the 'file' command to check the file type
    $fileType = & file -b $file.FullName

    # If the file type contains "shared object", consider it a dynamic library
    if ($fileType -match "shared object") {
        Write-Host "Copying dynamic library: $($file.FullName)"
        
        # Copy the file to the target directory
        Copy-Item -Path $file.FullName -Destination $destination -Force
    }
    # Check if it's a symbolic link
    elseif ($file.LinkType -eq 'SymbolicLink') {
        $linkTarget = Resolve-Symlink -Path $file.FullName

        $targetType = & file -b $linkTarget

        if ($targetType -match "shared object") {
            Write-Host "Copying symbolic link target for: $($file.FullName)->$($linkTarget)"
        
            # Copy the symbolic link target to the target directory
            Copy-Item -Path $linkTarget -Destination $destination -Force
        }
    }
}

Write-Host "Copy operation completed!"
