/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   beat2_wav;
    const int            beat2_wavSize = 211140;

    extern const char*   bigFiles_txt;
    const int            bigFiles_txtSize = 224586;

    extern const char*   blip_wav;
    const int            blip_wavSize = 18964;

    extern const char*   README;
    const int            READMESize = 3042;

    extern const char*   snare_wav;
    const int            snare_wavSize = 26734;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 5;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}
