## xml parsing library

basic library for reading XML files
this will form the basis of a DAE importer for lwcgl
- probably not standards-compliant
- doesn't handle multi-byte characters or unicode correctly
    + any files containing characters outside utf-8 `[x00-xFF]` will probably not load correctly
- doesn't support a few of the zanier xml features like entity declarations (are these needed for DAE?)

### get started

this is already set up to run on Visual Studio Code (for OSX)

run the 

use "Cmd+R" to debug

you will need clang++ and lldb (from xcode?)