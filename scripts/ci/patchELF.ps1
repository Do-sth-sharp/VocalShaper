param (
    [string]$DirectoryPath,
    [string]$RPath
)

# Get all files in the directory (recursively)
$allFiles = Get-ChildItem -Path $DirectoryPath -Recurse

# Loop through each file and check if it is an ELF file
foreach ($file in $allFiles) {
    # Check if the file is an ELF file using the 'file' command
    $fileType = & file $file.FullName
    
    if ($fileType -match "ELF") {
        Write-Host "Patching ELF file: $file"
        
        # Use patchelf to set the rpath to $ORIGIN
        $RPathStr = "'" + $RPath + "'"
        patchelf --set-rpath $RPathStr $file.FullName
    }
}

Write-Host "Finished patching ELF files."