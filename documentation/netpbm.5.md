# NetPBM

> The NetPBM Image File Formats
 
The PBMplus file formats were developed by Jef Poskanzer as a
means of including images into e-mail messages long before such
things as MIME types were popular. But since then they have
become very popular amongst computer graphics and computer vision
people for image input and output. After Poskanzer stopped
maintaining the formats, the net gathered it up, with corrections,
extensions and additions and created NetPBM.

The formats are extremely simple, and have absolutely no advanced
features. There is no data compression. This simplicity makes
it easy to code programs that can read large
collections of existing images or write images that can be
immediately used by others.

## Image Types
PBMplus is actually a collection of 6 very similar formats that
can be classified in two ways: by image type and by data type.

The formats describe three types of images: black and white
bitmaps, gray scale images, and full color images.

Bitmaps are usually called PBM files and often have a
`.pbm` extension. (A specific extension is
*not* required for any of the PBMplus formats.) In this
format, a `1` means black and `0` means
white. 

Grayscale images in the PBMplus formats are referred to as PGM
files, with a `.pgm` extension. Grayscale intensities
are integer values going from 0 for black up to a maximum value
for white specified in the image itself. 
Color images are called PPM files, and usually have a
`.ppm` extension. Pixels in PPM files are given as
integer amounts of red, green, and blue, on a scale from 0 to a
maximum value given in the file.

The data in each image type can be stored in either ASCII or
binary form.

## Format
Each PBMplus image file consists of a header followed by the image
data for the image.

## Headers
Each PBMplus image file has a ASCII header that consists of three
parts, one of which is optional depending on the image type.

### Magic
The first part of the header consists of a unique identifying string,
colloquially known as the file magic, which identifies the file as a
PBMplus image file. The magic for these files consists of the
uppercase character `P`, followed by an ASCII digit 1
through 6. These characters *must* be the first two
characters in the file. The magic values can be summarized in the
table below:

|                 | ASCII | Binary |
|:---:            | :---: | :----: |
| B/W (pbm)       |    P1 |     P4 |
| Grayscale (pgm) |    P2 |     P5 |
| Color (ppm)     |    P3 |     P6 |

Whitespace separates the magic characters from the sections
below. A single new line character is often used.

### Size
The second part of the header consists of two numerical ASCII
values that give the width and height, respectively, of the image
in pixels.

### Intensity
The last part of the header is a positive numerical ASCII value which
indicates the maximum possible value that a pixel can have. In
the case of color images, it is the maximum value that a red,
green or blue component of a pixel may have. In black and white
images this value is not needed. In binary grayscale and color
images (P5 and P6), this intensity value can never be greater than 255.
ASCII files do not have this restriction.

### Comments
The header of a PBMplus image is allowed to have comments, which
are ignored by a program processing the file. Comments consist of
the # character and continue until the end of the line. The
comment may not come before the magic. There are no comments
after the intensity value in a binary file. Comments are allowed
in the image data section of an ASCII file. Multiple line
comments are allowed as long as each line starts with a #
character.

## Image Data and Types
The format of the image data for each image type is described
below. The image data describes the intensity values for the
image pixels in row major order. The image origin is in the upper
left hand corner and proceeds in positive x to the right and and
positive y towards the bottom. The first pixel value will be the
pixel in the top left corner of the image. The next pixel value
will be to the right of that and so on until all the values of the
topmost pixel row are given. These are followed immediately by
the values of the second row and so on until the bottom of the
image is reached.

### Black and White ASCII (P1)
After the header, the intensity values of the pixels are given as
sequences of the ASCII characters 1 and 0. There may or may not
be white space between these characters. The value 1 represents
black and the value 0 represents white. Comments are allowed in
the image data.

### Grayscale ASCII (P2)
After the header, the intensity values of the pixels are given as
ASCII numbers separated by whitespace. Comments are allowed in the
image data.

### Color ASCII (P3)
After the header, the values of each pixel are given as 3 ASCII
numbers representing the red, green, and blue components of the
pixel. The componenets are separated by white space as are the
pixel values themselves. Comments are allowed in the image data.

### Black and White Binary (P4)
After the height of the image is given in the header, one
character of white space is allowed. This is usually a newline
character. The image data bytes immediately follow. Each byte in
this format represents eight pixels, with 1 bits representing
black and 0 bits representing white. The bits are in
little-endian order, i.e., the most significant bit is to the left and
the least significant bit is to the right. No comments are
allowed in the image data.

### Grayscale Binary (P5)
After the maximum intensity value of the image is given in the
header, one character of whitespace is allowed, usually a newline
character. The image data immediately follows, one byte per pixel
value. No space or comments of any kind are allowed in the image
data section. All characters are treated as intensities corresponding
to their ASCII values. Because there is a correspondence of one
byte per pixel, the maximum allowed intensity value can be no greater
than 255.

### Color Binary (P6)
After the maximum intensity value of the image is given in the
header, exactly one character of whitespace is allowed, usually a
newline character. The image data immediately follows, three bytes
per pixel value. The three bytes represent the red, green and blue
values of the pixel respectively. No space or comments of any kind
are allowed in the image data section.

## Commentary
Here is a brief summary about some of the pros and cons of ASCII
and binary PBMplus files and of the format in general:

* ASCII files are *huge*. There can be as many as 4
bytes to describe the intensity value of a single pixel or
component. Color `ppm` images could easily have at least
12 bytes per pixel. In ASCII `pbm` files there are at least
8 bytes per pixel.
* ASCII files can allow sample values greater than 255. This
can be very important in applications where high intensity
resolution is needed, such as medical imaging or cinematic
special effects.
* This format has no means to represent indexed color images.
* This format has no compression.
* This format has no means to represent
"transparent" values which are very useful in creating
shaped images such as icons.
* ASCII files are readable by humans and can be easily
included in generic e-mail messages. With today's MIME types
for e-mail attachments, this is no longer an issue.
* Binary files are not readable by humans. But usually only
the header is of human interest. 

---

Last modified: Mon Jan 13 21:26:12 CST 2025 
