Mifare-Tools
============

Library and command line tools for Mifare classic 1K NFC tags.
It can:
- List NFC devices
- List NFC tags
- Read a Mifare Classic 1K tag
- Write a Mifare Classic 1K tag

Usage
=====

List devices:
```shell
mifare-tools-cli --devices
```

List tags:
```shell
mifare-tools-cli --tags
```

Read a tag with some command line keys:
```shell
mifare-tools-cli --read --key ffffffffffff --key a1a2a3a4a5a6
```

Read a tag with a key file and save dump to a file:
```shell
mifare-tools-cli --read --key-file ~/my_key_file --output ~/my_dump
```

Write a tag with a command line key, from stdin:
```shell
cat ~/my_dump | mifare-tools-cli --write --key-file ffffffffffff
```

Write a tag with a command line key, from a file:
```shell
mifare-tools-cli --write --key-file ffffffffffff --input ~/my_dump
```

Write a tag with a key file, from a file, on a specified device, on a specified tag:
```shell
mifare-tools-cli --write --key-file ffffffffffff --input ~/my_dump --device pn53x_usb:003:008 --tag a1a2a3
```

Build and Install
=================
Requirements
------------
Libnfc          https://github.com/nfc-tools/libnfc
Libfreefare     https://github.com/nfc-tools/libfreefare

Build and Install
-----------------
```shell
git clone https://git.rthoni.com/robin.thoni/mifare-tools
cd mifare-tools
mkdir build
cd build
cmake ..
make
sudo make install
```