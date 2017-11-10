## xml parsing library

basic library for reading XML files
this will form the basis of a DAE importer for lwcgl
- probably not standards-compliant
- doesn't handle multi-byte characters or unicode correctly
    + any files containing characters outside utf-8 `[x00-xFF]` will probably not load correctly
- doesn't support a few of the zanier xml features like entity declarations (are these needed for DAE?)

