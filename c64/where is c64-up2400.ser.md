# External Binary Requirement: `c64-up2400.ser`

This project **requires** the binary file `c64-up2400.ser` to enable full functionality.
The file is **not included** in this repository and **is not redistributed** with this project.

The source code for `c64-up2400.ser` is provided by a third party.
**No license is specified** for that source code. For this reason, users must obtain and build the binary **themselves**.
This project does **not** grant, imply, or transfer any license for that code or its binary output.
This separation ensures compatibility with this project’s **MIT license**.

## Obtaining and Building `c64-up2400.ser`

### 1. Clone the upstream repository

Clone the official repository using Git:
```bash
git clone https://github.com/nanoflite/c64-up2400-cc65
cd c64-up2400-cc65
```

### 2. Build the project (initial attempt)

Run `make`:
```bash
make
```

During the build process, the following error will occur:
```bash
cl65 -c -t c64 -O -o c64-up2400.o c64-up2400.s
c64-up2400.s:197: Error: Symbol ‘STATUS’ is already defined
C:\cc65\asminc/c64.inc:12: Note: The symbol was previously defined here
make: *** [c64-up2400.o] Error 1
```
This error is caused by a symbol name conflict in the assembly source file.

### 3. Fix the build error

Open the file `c64-up2400.s` in a text editor and apply the following changes:

* **Line 57**
  Change:
  ```
  .word STATUS
  ```
  To:
  ```
  .word SERSTATUS
  ```
* **Line 197**\
  Change:
  ```
  STATUS:
  ```
  To:
  ```
  SERSTATUS:
  ```

Save the file.

### 4. Build again

Run `make` again:
```bash
make
```
If the changes were applied correctly, the build will complete successfully and the file
`c64-up2400.ser` will be generated.

## Installing the Binary into the C64AI Disk Image

After the build completes, the generated `c64-up2400.ser` file must be written into the C64AI disk image.
Ensure you are in the directory containing `c64-up2400.ser`, then run:
```bash
c1541.exe -silent on -attach .\c64ai.d64 -write c64-up2400.ser
```

## Important Notes

* This project **does not include** `c64-up2400.ser`.
* This project **does not redistribute** the source code or binary.
* The user is fully responsible for obtaining, modifying, and building the binary.
* No license for the third-party code is granted or implied by this project.
* This procedure is required to keep the project compatible with the **MIT license**.

