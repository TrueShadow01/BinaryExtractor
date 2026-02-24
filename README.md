# BinaryExtractor

A structured binary archive extractor written in C++.
This project is built for educational purposes to understand:

- Binary file parsing
- Offset-based extraction
- Format validation
- Defensive parsing techniques

Currently supports a custom `.bin` archive format.

---

## Features

- Custom `BinaryReader` abstraction
- Little-endian integer parsing
- Structured archive header parsing
- File table parsing
- Offset-based file extraction
- Metadata boundary validation
- Overflow-safe range validation
- Name length validation
- File size validation
- Command-line archive loading
- Exception-based error handling

---

## Architecture

The extractor is structured into parsing layers:

### BinaryReader
Low-level binary reading abstraction.

### Archive Parsing
- Header parsing and validation
- FileEntry parsing and validation

### Extraction
- Offset validation
- Payload extraction
- Binary-safe file writing

Each layer validates its own data to ensure structural integrity.

---

### Archive Format
```
[Magic](4 bytes)
[Version](4 bytes)
[FileCount](4 bytes)
```

For each file:
```
[NameLength](4 bytes)
[Name](NameLength bytes)
[Size](4 bytes)<br>
[Offset](4 bytes)<br>

[File Data...]
```

### Rules
- Data region must not overlap metadata
- Offsets must be within file bounds
- Name length limited to 255 bytes
- File count limited to 10.000

---

## Usage

```bash
BinaryExtractor.exe <archive file>
```

Extracted files are written to the current directory.

## Safety
The extractor performs strict validation:
- Validates archive magic
- Validates file count
- Validates name length
- Prevents metadata overlap
- Prevents out-of-bounds reads
- Prevents integer overflow in offset calculations
Corrupted or malicious archives are rejected safely.

## Future Improvements
- Archive creation (packer) - DONE
- Compression support
- Alignment handling
- Hash-based file indexing
- Directory structure support
- Improved error handling
- Unit testing
