# Plumblism
> Image processing with brain damage.

Plumblism is a PBMplus image file processing library in C.

# Introduction to PBMplus
PBMplus is a family of closely related image file formats.
Their core design is identical,
they differ in either being ASCII/binary
and or B&W/greyscale/RGB.
The 6 combinations make up the family.

It is safe to say that PBMplus has the simplest structure imaginable.
Put simply, they are arrays of color values.
No compression, no color tables, no whatever the hell is BMP is doing.
If you would like to learn the exacts,
consult [documentation/PBMplus\_-\_Overview.html](the specification).

While the original use case has been long deprecated (email attachments),
due to its simplicity, the format remains useful.
For example, tile maps for simple purposes or rapid prototyping.

> [!TIP]
> GIMP supports PBMplus.

# API
```c
```
