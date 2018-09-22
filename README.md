# PS2 Save Utility

Converter for PS2 game save files.

## Supported File Formats

Extension|Action|Supported Devices
---|---|---
.cbs|Read|CodeBreaker, Cheat Device
.max, .pws|Read|Action Replay MAX, MAX Drive
.psu|Read/Write|uLaunchELF, Cheat Device
.sps, .xps|Read|SharkPort, X-Port V2

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./PS2SaveUtility <input file> <output file>
```

Planned:

```bash
./PS2SaveUtility <input file> -l
./PS2SaveUtility <input file> -f <format>
./PS2SaveUtility <input directory> -f <format>
```

When using the `-f` option, `<format>` can be any of the extensions listed above
under *Supported File Formats*. It must be specified only by the 3-character
extension, without the preceding period.

### Examples

>`./PS2SaveUtility GTA3.cbs GTA3.psu`

Convert `GTA3.cbs` from CodeBreaker format to `GTA3.psu` in PSU format.

**The following usage example are planned but not yet implemented!**

>`./PS2SaveUtility GTA3.cbs -f max`

Convert `GTA3.cbs` from CodeBreaker format to `GTA3.max` in Action Replay MAX
format.

>`./PS2SaveUtility GTA3.cbs -l`

List the contents of `GTA3.cbs`.

>`./PS2SaveUtility Saves/ -f psu`

Convert all supported save files in `Saves/` to PSU format.
