# Creating-modules-and-Hooking-syscalls
## Creating modules
## Hooking syscalls
A kernel module hooking onto sys_write and sys_open.

Created in Zorin 12.4 on 64 bit, with kernel version 4.15.0-30-generic.
### Setup
- Open 2 terminals.
- On the first terminal:
  + Compile `test.c`.
  
    ```console
    $ gcc test.c -o test
    ```
    
  + Compile module `hook`.
  
    ```console
    $ make
    ```
    
  + Insert moudule into Kernel.
  
    ```console
    $ sudo insmod hook.ko
    ```
    
  + Run `test`
  
    ```console
    $ ./test
    ```
    
- On the second terminal: see messages on the logging

  ```console
  $ sudo dmesg -c
  $ sudo dmesg -wH
  ```
  
- Remove module from kernel on the first terminal

  ```console
  $ rm hook
  ```
