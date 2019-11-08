# Creating-modules-and-Hooking-syscalls
## Creating modules
A kernel module create character device for process in user space read a random integer

*Environment*: Virtual Box with Ubuntu 18.04 (64 bit), kernel version 5.0.0-32-generic.
### Setup
- Open 2 terminal windows.
- On the first terminal:
  + Compile `main.c`
  
    ```console
    $ gcc -o main main.c
    ```
    
  + Build module `hung_chrdev`
  
    ```console
    $ make
    ```
    
  + Insert moudule into Kernel.
  
    ```console
    $ sudo insmod hung_chrdev.ko
    ```
    
  + Run `main`
  
    ```console
    $ ./main
    ```
    
- On the second terminal: see messages on the logging.

  ```console
  $ sudo dmesg -c
  $ sudo dmesg -wH
  ```
  
- Remove module from kernel on the first terminal.

  ```console
  $ sudo rmmod hung_chrdev
  ```

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
    
  + Run `test`.
  
    ```console
    $ ./test
    ```
    
- On the second terminal: see messages on the logging.

  ```console
  $ sudo dmesg -c
  $ sudo dmesg -wH
  ```
  
- Remove module from kernel on the first terminal.

  ```console
  $ rm hook
  ```
