//*******************************************************************************************************
//*                                                                                                     *
//*     Date: 2023-05-30                                                                                *
//*     Prog: example.c                                                                                 *
//*                                                                                                     *
//*     Desc: This program is used to practice how to write driver                                      *
//*                                                                                                     *
//*                                                                                                     *
//*     Auth: David Wang.                                                                               *
//*     Dept: GRDC RDC4 RDD3 SDD2                                                                       *
//*                                                                                                     *
//*******************************************************************************************************
/* example.c */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/stddef.h>
#include <linux/libgcc.h>

#include <asm/uaccess.h>


#define EXAMPLE_MAJOR 60
#define EXAMPLE_NAME "example"



/**
 *  =====================================
 *          Module Declare
 *  =====================================
**/
MODULE_LICENSE("Dual BSD/GPL");


int             g_counter;

static int example_open(
    struct inode    *inode, 
    struct file     *filp
    ) 
{
    g_counter++;
    printk("<SAMPLE> Example[%d]:       open file and start operation\n" , g_counter);
    return 0;
}

static int example_close(
    struct inode    *inode, 
    struct file     *filp
    ) 
{
    g_counter++;
    printk("<SAMPLE> Example[%d]:       close\n", g_counter);
    return 0;
}

static ssize_t example_read(
    struct file     *filp, 
    char            *buf, 
    size_t          size, 
    loff_t          *f_pos)
{
    g_counter++;
    printk("<SAMPLE> Example[%d]:       read  (size=%zu)\n", g_counter, size);
    return 0;
}

static ssize_t example_write(
    struct file     *filp, 
    const char      *buf, 
    size_t          size, 
    loff_t          *f_pos
    ) 
{
    size_t          index;
    uint8_t         bytes;
    char            data[10];
    

    g_counter++;
    for (index = 0; index < size; ++index){
        data[index] = buf[index];
    }
    printk("<SAMPLE> Example[%d]:       data = %s\n", g_counter, data);


    for (index = 0; index < size; ++index){
        if (copy_from_user(&bytes, buf + index, 1) != 0){
            break;
        }
        g_counter++;
        printk("<SAMPLE> Example[%d]:       input  (buf[%zu] = %02x)\n", g_counter,index, bytes);
    }
    return index;
}

/**
 *  =====================================
 *          File Operation struct
 *  =====================================
**/
static struct file_operations example_fops = {
    .open           = example_open,
    .release        = example_close,
    .read           = example_read,
    .write          = example_write,
};


static int example_init(
    void
    ) 
{
    int             result;
    g_counter = 0;



    printk("<SAMPLE> Example[%d]:       init\n", g_counter);
    /* Register character device */
    result = register_chrdev(EXAMPLE_MAJOR, EXAMPLE_NAME, &example_fops);
    if (result < 0) {
        printk("<SAMPLE> Example[%d]: Failed to register character device\n", g_counter);
        return result;
    }

    return 0;
}

static void example_exit(
    void
    ) 
{

    g_counter++;
    printk("<SAMPLE> Example[%d]:       exit\n", g_counter);

    unregister_chrdev(EXAMPLE_MAJOR, EXAMPLE_NAME);
}

module_init(example_init);
module_exit(example_exit);