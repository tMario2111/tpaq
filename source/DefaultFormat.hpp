#pragma once

#include <string_view>

// The default json mustache format
// Used if "format.mustache" is not found

constexpr std::string_view DEFAULT_MUSTACHE_FORMAT = 
R"(
{{!
  The default template is a simple json file. Modify it as you wish.

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
)";