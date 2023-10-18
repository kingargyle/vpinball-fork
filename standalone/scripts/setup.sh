#!/bin/bash

#
# Usage: source setup.sh
#
# This will make the VPinballX_GL executable and clear the download quarantine flags on MacOS 
#

chmod +x ./VPinballX_GL

if [[ "$(uname)" == "Darwin" ]]; then
   xattr -d com.apple.quarantine VPinballX_GL
   xattr -d com.apple.quarantine libbass.dylib
fi
