/*
 *  Zip_Helper.h.h
 *  Base
 *
 *  Created by Brent Wilson on 12/1/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _ZIP_HELPER_H_
#define _ZIP_HELPER_H_

static const int ZIP_EndOfCDRSig  = 0x06054b50;
static const int ZIP_LocalFileSig = 0x04034b50;
static const int ZIP_FileSig      = 0x02014b50;

#pragma pack(1)
struct ZIP_LocalFileHeader {
    int   signature;              //4 bytes  (0x04034b50)
    short versionNeededToExtract; //2 bytes
    short generalPurposeBitFlag;  //2 bytes
    short compressionMethod;      //2 bytes
    short lastModFileTime;        //2 bytes
    short lastModFileDate;        //2 bytes
    int   crc32;                  //4 bytes
    int   compresseSize;          //4 bytes
    int   uncompresseSize;        //4 bytes
    short fileNameLength;         //2 bytes
    short extraFieldLength;       //2 bytes
    // file name (variable size)
    // extra field (variable size)
};

struct ZIP_FileHeader {
    int   signature;                   //4 bytes  (0x02014b50)
    short versionMadeBy;               //2 bytes
    short versionNeededToExtract;      //2 bytes
    short generalPurposeBitFlag;       //2 bytes
    short compressionMethod;           //2 bytes
    short lastModFileTime;             //2 bytes
    short lastModFileDate;             //2 bytes
    int   crc32;                       //4 bytes
    int   compressedSize;              //4 bytes
    int   uncompressedSize;            //4 bytes
    short fileNameLength;              //2 bytes
    short extraFieldLength;            //2 bytes
    short fileCommentLength;           //2 bytes
    short diskNumberStart;             //2 bytes
    short internalFileAttributes;      //2 bytes
    int   externalFileAttributes;      //4 bytes
    int   relativeOffsetOfLocalHeader; //4 bytes
    // file name (variable size)
    // extra field (variable size)
    // file comment (variable size)

    int extraSize() {
        return fileCommentLength + extraFieldLength;
    }
};

struct ZIP_EndOfCDR {
    int   signature;                      //4 bytes  (0x06054b50)
    short numberOfThisDisk;               //2 bytes
    short numberOfTheDiskWithStartOfCDR;  //2 bytes
    short numberOfEntriesInCDROnThisDisk; //2 bytes
    short numberOfEntriesInCDR;           //2 bytes
    int sizeOfCDR;                        //4 bytes
    int offsetOfStartOfCDR;               //4 bytes
    short commentLength;                  //2 bytes
    // .ZIP file comment (variable size)
};
#pragma pack()
#endif

/*
  A.  Local file header:
        local file header signature     4 bytes  (0x04034b50)
        version needed to extract       2 bytes
        general purpose bit flag        2 bytes
        compression method              2 bytes
        last mod file time              2 bytes
        last mod file date              2 bytes
        crc-32                          4 bytes
        compressed size                 4 bytes
        uncompressed size               4 bytes
        file name length                2 bytes
        extra field length              2 bytes
        file name (variable size)
        extra field (variable size)

  B.  File header:
        central file header signature   4 bytes  (0x02014b50)
        version made by                 2 bytes
        version needed to extract       2 bytes
        general purpose bit flag        2 bytes
        compression method              2 bytes
        last mod file time              2 bytes
        last mod file date              2 bytes
        crc-32                          4 bytes
        compressed size                 4 bytes
        uncompressed size               4 bytes
        file name length                2 bytes
        extra field length              2 bytes
        file comment length             2 bytes
        disk number start               2 bytes
        internal file attributes        2 bytes
        external file attributes        4 bytes
        relative offset of local header 4 bytes
        file name (variable size)
        extra field (variable size)
        file comment (variable size)

  C.  End of central directory record:
        end of central dir signature    4 bytes  (0x06054b50)
        number of this disk             2 bytes
        number of the disk with the
        start of the central directory  2 bytes
        total number of entries in the
        central directory on this disk  2 bytes
        total number of entries in
        the central directory           2 bytes
        size of the central directory   4 bytes
        offset of start of central
        directory with respect to
        the starting disk number        4 bytes
        .ZIP file comment length        2 bytes
        .ZIP file comment       (variable size)
*/
