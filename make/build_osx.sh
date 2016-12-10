versions=('2015' '2016' '2016.5' '2017')
op_system='mac'
plugin_name='baseLoc'
make_dir='/Users/hunyadijanos/Documents/GIT/cc-maya-base_locator/make/build/'$op_system'/'
destination_dir='/Users/hunyadijanos/Documents/GIT/cc-maya-base_locator/build/'$op_system'/'

/Users/hunyadijanos/Documents/GIT/cc-maya-base_locator/make/build/mac/2015/src/Release/baseLoc.bundle
/Users/hunyadijanos/Documents/GIT/cc-maya-base_locator/make/build/mac/2017/src/Release/baseLoc.bundle

for i in "${versions[@]}"
	do
		
		if [ -e $make_dir$i ]
		then
			# echo $destination_dir$i
			cmake --build $make_dir$i --config Release
			
			bundle_file=$make_dir$i'/src/Release/'$plugin_name'.bundle'
			
			
			if [ -e $bundle_file ]
			then
				mkdir -p $destination_dir$i
				
				cp $bundle_file $destination_dir$i
				
#				echo "--------"
#				echo "--------"
#				echo "yolo" $bundle_file
#				echo "--------"
#				echo "--------"
			fi
			
		fi
		
	done



#
#if ((Test-Path $destination_dir) -eq $true)
##{
#	
#	$plugin_file_name = $plugin_name + ".bundle"
#
#	For ($i=0; $i -lt $versions.Length; $i++) 
#		{
#		
#			if ((Test-Path (".\build\win\" + $versions[$i])) -eq $true)
#			{
#				echo ""
#				echo "-------------------------------------" -foregroundcolor "magenta"
#				echo "[ Building -" $plugin_name "] - Maya" $versions[$i] -foregroundcolor "magenta"
#				echo "-------------------------------------" -foregroundcolor "magenta"
#				echo ""
#				
#				cmake --build (".\build\win\" + $versions[$i]) --config Release
#				
#				$source_dir = (".\build\win\" + $versions[$i] + "\bin\Release\" + $plugin_file_name)
#				$destination_dir_version = ($destination_dir + $versions[$i] + "/" + $plugin_file_name)
#				
#				if ((Test-Path $source_dir) -eq $true)
#				{
#					# Write-Host "Binary found -" $plugin_name -foregroundcolor "green"
#					
#					Write-Host "Copying -" $source_dir "->" $destination_dir -foregroundcolor "green"
#					New-Item -Force $destination_dir_version
#					copy-item $source_dir $destination_dir_version -Recurse -Force
#				}
#				
#			}
#		}
#		
#}
#
#else
##{
#	Write-Host "[Error] Destination dir not found:" -foregroundcolor "red"
#	Write-Host $destination_dir -foregroundcolor "red"
#}