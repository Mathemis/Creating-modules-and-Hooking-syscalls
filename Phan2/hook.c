#include <asm/unistd.h>
#include <asm/cacheflush.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/pgtable_types.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <asm/cacheflush.h>
#include <linux/security.h>
#include <linux/fdtable.h> 
#include <linux/kallsyms.h>

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Lam Ba Thinh - 1712791 - 17CNTN" );

void	**sys_call_table;

asmlinkage int ( *original_open  ) ( const char __user *, int, mode_t );
asmlinkage int ( *original_write ) ( unsigned int, const char __user *, size_t );

asmlinkage int	new_open( const char __user * pathname, int flags, mode_t mode )
{
  printk(KERN_INFO "HOOKING SYS_OPEN!!!\n");
  printk(KERN_INFO "Process name: %s\n", current->comm);
  printk(KERN_INFO "Filename: %s\n\n", pathname);
  
  return (*original_open)( pathname, flags, mode );
}

asmlinkage int 	new_write( unsigned int fd, const char __user *buf, size_t count )
{
  if(fd == 3){
    char *tmp;
    char *pathname;
    struct file *file;
    struct path *path;
    struct files_struct *files = current->files;
    spin_lock(&files->file_lock);
    file = fcheck_files(files, fd);
    if (!file) {
      spin_unlock(&files->file_lock);
      return -ENOENT;
    }

    path = &file->f_path;
    path_get(path);
    spin_unlock(&files->file_lock);

    tmp = (char *)__get_free_page(GFP_KERNEL);

    if (!tmp) {
      path_put(path);
      return -ENOMEM;
    }

    pathname = d_path(path, tmp, PAGE_SIZE);
    path_put(path);

    if (IS_ERR(pathname)) {
       free_page((unsigned long)tmp);
       return PTR_ERR(pathname);
    }

    free_page((unsigned long)tmp);
    printk(KERN_INFO "HOOKING SYS_WRITE!!!\n");
    printk(KERN_INFO "Process name: %s\n", current->comm);
    printk(KERN_INFO "Filename: %s\n", pathname);
    printk(KERN_INFO "Bytes written: %zu\n\n", count);
  }

  return (*original_write) ( fd, buf, count );
}


/*Make page writeable*/
int make_rw(unsigned long address){
  unsigned int level;
  pte_t *pte = lookup_address(address, &level);
  if(pte->pte &~_PAGE_RW){
    pte->pte |=_PAGE_RW;
  }
  return 0;
}
 
/* Make the page write protected */
int make_ro(unsigned long address){
  unsigned int level;
  pte_t *pte = lookup_address(address, &level);
  pte->pte = pte->pte &~_PAGE_RW;
  return 0;
}

static int	init_mod( void )
{
  printk(KERN_ALERT "Captain Hook loaded successfully..\n");
  sys_call_table = (void*)kallsyms_lookup_name("sys_call_table");

  original_open  = (void *)sys_call_table[ __NR_open  ];
  original_write = (void *)sys_call_table[ __NR_write ];

  make_rw((unsigned long)sys_call_table);

  sys_call_table[ __NR_open  ] = new_open;
  sys_call_table[ __NR_write ] = new_write;

  return 0;
}

static void	exit_mod( void )
{
  printk(KERN_ALERT "Unloaded Hookopen successfully\n");

  sys_call_table[ __NR_open  ] = original_open;
  sys_call_table[ __NR_write ] = original_write;

  make_ro((unsigned long)sys_call_table);
}

module_init( init_mod );
module_exit( exit_mod );