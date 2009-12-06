#!/bin/bash

echo "Source file;Description;RC of preprocessor;RC of translation;RC of indentation;RC of compilation"
find . -name '*.dbl' | while read test; do
	DESCRIPTION=$(grep "^;[[:space:]]*description:.*" $test | cut -d: -f 2- | sed 's/^[[:space:]]*//g')
	RC=$(grep "^;[[:space:]]*rc:.*" $test | cut -d: -f 2-  | sed 's/^[[:space:]]*//g')
	echo "$test;$DESCRIPTION;$RC"
done
