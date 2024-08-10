This is my attempt on a password manager.

I plan on writing it in c (to learn c) and maby later use some FFI (to learn FFI)

Planed features:

- Password generation
- Log in with YubiKey
- multiple YubiKey support
- save passwords, notes, images, documents
- be in one file so you can back it up easily
- probably no backup login method because screw you.
  You should have registered multiple YubiKeys and backed up one of them!

# How to build

0. install requirements
0. run `make`

## How to include headers in nvim?

0. have clangd installed
0. install *bear*
0. run `bear -- make`

on my machine it works. If it doesn't at yours, that's not my problem!
