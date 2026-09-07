# Auzio (Editore)
My attempt at writing a basic graphical text editor.

// FIXME: needs dependencies e.g. glfw + clang - also missing windows build
To compile from source just run `./build.sh`. 


## Developing

This project uses a simple C unity build system with the classic build.sh.
The project is setup to go with clang and clangd lsp. Because the clang tooling
leaves some thing on the table some workarounds are necessary to properly use
the lsp with a unity build.
For clangd to properly scan your files the .clangd-File is
generated from the build.sh file if you run:
```bash
./build.sh dev-setup
```
