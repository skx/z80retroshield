#!/bin/bash
#
# This script is executed every time a pull-request is created, or updated,
# or when a push is made to the repository's master branch.
#
# We reformat the code, and fail if changes are present.
#



#
# Install the astyle tool
#
apt-get update
apt-get install --yes    astyle


#
# Process each pattern
#
for pattern in *.ino *.h *.cpp; do

    #
    # Format matching files.
    #
    astyle --style=allman -A1 --indent=spaces=4   --break-blocks --pad-oper --pad-header --unpad-paren --max-code-length=200 $(find . -name $pattern)

done


#
# OK now we've formatted files.
#
# If any changes were applied then we'll have files with an ".orig" suffix
#
count=$(find . -name '*.orig' -print | wc -l)

if [ "${count}" -gt "0" ]; then
    echo "Files not formatted correctly"
    exit 1
fi

#
# All good
#
exit 0
