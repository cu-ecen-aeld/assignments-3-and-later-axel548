#!/bin/bash


if [ "$#" -ne 2 ]; then
    echo "Error: Both directory path and search string must be specified."
    exit 1
fi

filesdir="$1"
searchstr="$2"

if [ -z "$filesdir" ]; then
    echo "Error: Directory path not specified."
    exit 1
fi

if [ ! -d "$filesdir" ]; then
    echo "Error: '$filesdir' is not a directory."
    exit 1
fi

match_count=0
while IFS= read -r -d '' file; do
    if [ -f "$file" ]; then
        match_lines=$(grep -c "$searchstr" "$file")
        if [ "$match_lines" -gt 0 ]; then
            match_count=$((match_count + match_lines))
        fi
    fi
done < <(find "$filesdir" -type f -print0)

file_count=$(find "$filesdir" -type f | wc -l)

echo "The number of files are $file_count and the number of matching lines are $match_count"
exit 0
