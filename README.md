# tpaq

tpaq is a simple to use command line texture packing utility.

## Example
```
tpaq -d textures1 textures2 -f image.png -s 2048 -b 1 -o atlas
```

## List of commands
```
-h,--help                   Print this help message and exit
--version                   Display program version information and exit
-f,--files TEXT ...         Texture files
-d,--directories TEXT ...   Directories containing texture files (non-recursive by default)
-o,--output TEXT REQUIRED   Output file (.png & .json)
-s,--size UINT REQUIRED     Output texture maximum size
-b,--border UINT            Border between textures (0 by default)
--keep_extensions           Keep texture extensions in .json file
--recursive                 Recursive directory search
```
