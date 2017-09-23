$versions = @("2016","2016.5","2017","2018")
$op_system = "win"
$plugin_name = "baseLoc"
$destination_dir = "c:/WORK/_admin/git/cc-maya-base_locator/build/"+$op_system+"/"




if ((Test-Path $destination_dir) -eq $true)
{
	
	$plugin_file_name = $plugin_name + ".mll"

	For ($i=0; $i -lt $versions.Length; $i++) 
		{
		
			if ((Test-Path (".\build\win\" + $versions[$i])) -eq $true)
			{
				Write-Host ""
				Write-Host "-------------------------------------" -foregroundcolor "magenta"
				Write-Host "[ Building -" $plugin_name "] - Maya" $versions[$i] -foregroundcolor "magenta"
				Write-Host "-------------------------------------" -foregroundcolor "magenta"
				Write-Host ""
				
				cmake --build (".\build\win\" + $versions[$i]) --config Release
				
				$source_dir = (".\build\win\" + $versions[$i] + "\bin\Release\" + $plugin_file_name)
				$destination_dir_version = ($destination_dir + $versions[$i] + "/" + $plugin_file_name)
				
				if ((Test-Path $source_dir) -eq $true)
				{
					# Write-Host "Binary found -" $plugin_name -foregroundcolor "green"
					
					Write-Host "Copying -" $source_dir "->" $destination_dir -foregroundcolor "green"
					New-Item -Force $destination_dir_version
					copy-item $source_dir $destination_dir_version -Recurse -Force
				}
				
			}
		}
		
}

else
{
	Write-Host "[Error] Destination dir not found:" -foregroundcolor "red"
	Write-Host $destination_dir -foregroundcolor "red"
}