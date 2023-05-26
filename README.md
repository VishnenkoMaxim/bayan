# bayan

Utility to find file duplicates

## Arguments

-h [ --help ] &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; Help info\
-s [ --min-size ] arg (=1) &emsp; &emsp; &ensp; Min file size, byte\
-b [ --block-size ] arg (=256) &emsp; Reading block size\
-d [ --scan-dirs ] arg         &emsp; &emsp; &emsp; &emsp; Directories to scan\
-e [ --exclude-dirs ] arg      &emsp; &emsp; &emsp; Excluded directories\
-r [ --recursive ]             &emsp; &emsp; &emsp; &emsp; &emsp; &ensp; Make it recursively\
-a [ --hash-alg ] arg (=crc32) &emsp; Hash algorithm\
-m [ --mask ] arg              &emsp; &emsp; &emsp; &emsp; &emsp; Regular expression mask

## Usage

```python

#find duplicates in /home (recursively) 
	
	bayan -d /home -r

#find duplicates in several folders 

	bayan -d /home /tmp  -r

#using  MD5 hash algorithm (default CRC32, supports only md5 or crc32)

	bayan -d /home /tmp  -r -a md5

#exclude directory "Task_2" from searching

	bayan -d /home -r -e Task_2

#you can use regular expressions to filter some files
	
	bayan -d /home -r -m .*.iml$    		- search files which end ".iml"

	bayan -d /home -r -m .*Task.*		- search files whose full path contains "Task"
	
```