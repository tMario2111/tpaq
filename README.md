# tpaq

tpaq is a simple to use command line texture packing utility. It was made using `stb_rect_pack`, `SFML` and
`Mustache`

## Example
```
tpaq -d textures1 textures2 -f image.png -s 2048 -b 1 -o atlas
```

## Custom formats
The `format.mustache` file can be modified to use any format. This is the default one:
```
{{!
  $rects - list of texture rects
  $name  - texture name
  $x, $y - left & top position
  $w, $h - width & height
  $last  - inverted section to check for the last item in list (useful for the json format)
}}
{
  "frames": {
    {{#rects}}
    "{{{name}}}": {
      "frame": {
        "x": {{{x}}},
        "y": {{{y}}},
        "w": {{{w}}},
        "h": {{{h}}}
      }
    }{{^last}},{{/last}}
    {{/rects}}
  }
}
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
-e,--extension TEXT         Data file extension (json by default)
--keep_extensions           Keep texture extensions in .json file
--recursive                 Recursive directory search
```
