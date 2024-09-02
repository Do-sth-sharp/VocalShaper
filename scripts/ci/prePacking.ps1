param (
    [string]$DirectoryPath
)

# Check if the directory path is provided and exists
if (-not (Test-Path -Path $DirectoryPath)) {
    exit
}

# Remove audio plugin list
$AUDIO_PATH = $DirectoryPath + "/data/audio/*"
Remove-Item $AUDIO_PATH -Include "blackPlugins.txt", "pluginPaths.txt", "plugins.xml"

$DEAD_PLUGINS_PATH = $AUDIO_PATH + "/deadPlugins"
if ((Test-Path $DEAD_PLUGINS_PATH) -eq "True") {
	Remove-Item $DEAD_PLUGINS_PATH -Recurse
}

# Remove build temp
$REMOVE_PATH = $DirectoryPath + "/*"

Remove-Item $REMOVE_PATH -Include "*.vshl.dmp"
Remove-Item $REMOVE_PATH -Include "*.vsml.dmp"
Remove-Item $REMOVE_PATH -Include "*.vsll.dmp"

Remove-Item $REMOVE_PATH -Include "*.iobj"
Remove-Item $REMOVE_PATH -Include "*.ipdb"
Remove-Item $REMOVE_PATH -Include "*.exp"
Remove-Item $REMOVE_PATH -Include "*.ilk"
Remove-Item $REMOVE_PATH -Include "*.lib"
Remove-Item $REMOVE_PATH -Include "*.a"

# Remove engine build temp
$ENGINE_REMOVE_PATH = $DirectoryPath + "/synth-engines/*"

Remove-Item $ENGINE_REMOVE_PATH -Include "*.iobj"
Remove-Item $ENGINE_REMOVE_PATH -Include "*.ipdb"
Remove-Item $ENGINE_REMOVE_PATH -Include "*.exp"
Remove-Item $ENGINE_REMOVE_PATH -Include "*.ilk"
Remove-Item $ENGINE_REMOVE_PATH -Include "*.lib"
Remove-Item $ENGINE_REMOVE_PATH -Include "*.a"
