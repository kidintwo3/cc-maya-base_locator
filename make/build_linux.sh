versions=('2015', '2016')
op_system='linux'
plugin_name='baseLoc'
make_dir='/home/jani/Desktop/baseLoc/build/'$op_system'/'
destination_dir='/home/jani/Desktop/baseLoc/build/'$op_system'/'

echo "Starting build..."
echo ""

for i in "${versions[@]}"
	do
		
		if [ -e $make_dir$i ]
		then
			# echo $destination_dir$i
			cmake --build $make_dir$i --config Release
			
			bundle_file=$make_dir$i'/src/Release/'$plugin_name'.su'
			
			
			if [ -e $bundle_file ]
			then
				mkdir -p $destination_dir$i
				
				cp $bundle_file $destination_dir$i
			fi
			
		fi
		
	done

echo ""
echo "Build finished..."

