# Plumblism
> Image processing with brain damage.

Plumblism is a PBMplus image file processing library in C.

## Introduction to PBMplus
PBMplus is a family of closely related image file formats.
Their core design is identical,
they differ in either being ASCII/binary
and or B&W/greyscale/RGB.
The 6 combinations make up the family.

It is safe to say that PBMplus has the simplest structure imaginable.
Put simply, they are arrays of color values.
No compression, no color tables, no whatever the hell is BMP is doing.
If you would like to learn the exacts,
consult [documentation/PBMplus_-_Overview.html](the specification).

While the original use case has been long deprecated (email attachments),
due to its simplicity, the format remains useful.
For example, tile maps for simple purposes or rapid prototyping.

> [!TIP]
> GIMP supports PBMplus.

## API
The [header file](source/plumblism.h) has been extensively commented.
Consult it for documentation.

## Compiling
* Plumblism has no dependencies
* Plumblism is standard C99
* a classic UNIX tool-chain is required

Invoking `make` will produce both a static and dynamic library.

You could also just copy the source files.

## Related work
* [https://netpbm.sourceforge.net/doc/index.html](https://netpbm.sourceforge.net/doc/index.html) original proper implementation
* [https://github.com/nkkav/libpnmio](https://github.com/nkkav/libpnmio) the implementation I wanted to use, then patch, but ended up rewritting
