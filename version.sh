#!/bin/bash

header_file="src/version.h"
output_file="version.json"

# Start JSON output
echo "{" > "$output_file"
echo '  "version": {' >> "$output_file"

# Use grep to filter out lines starting with #define, then use awk to print the second and third fields
first=1
grep "^#define" "$header_file" | awk '{print $2, $3}' | while read -r var_name var_value; do
    if [ $var_name == "AER_VERSION_H" ]; then
        continue
    fi
    if [ $first -eq 0 ]; then
        echo "," >> "$output_file"
    else
        first=0
    fi
    echo -n '    "'"$var_name"'": "'"$var_value"'"' >> "$output_file"
done

# End JSON output
echo "" >> "$output_file"
echo "  }" >> "$output_file"
echo "}" >> "$output_file"

echo "JSON output saved to $output_file"